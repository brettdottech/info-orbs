#include "DimmingManager.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <algorithm>
#include <esp_timer.h>

// The Dimming Manager can automatically adjust screen brightness in response to changes
// in ambient light intensity. The basic approach is to periodically measure the ambient
// light intensity, quantize it into a dimming level, then set the screen brightness to
// a value corresponding to the dimming level. To prevent the screen brightness from constantly
// changing due to small light variations, filtering of the light measurements and hysteresis
// are performed.

// Validate the relevant configuration parameters at compile time
static_assert(AUTO_BRIGHTNESS_MIN >= 0, "AUTO_BRIGHTNESS_MIN must not be negative");
static_assert(AUTO_BRIGHTNESS_MIN < AUTO_BRIGHTNESS_MAX,
              "AUTO_BRIGHTNESS_MIN must be less than AUTO_BRIGHTNESS_MAX");
static_assert(AUTO_BRIGHTNESS_MAX < 256, "AUTO_BRIGHTNESS_MAX must be less than 256");
static_assert(LIGHT_MIN >= 0, "LIGHT_MIN must not be negative");
static_assert(LIGHT_MIN < LIGHT_MAX, "LIGHT_MIN must be less than LIGHT_MAX");
static_assert(LIGHT_MAX < 4096, "LIGHT_MAX must be less than 4096");
static_assert(NUM_BRIGHTNESS_LEVELS > 1, "NUM_BRIGHTNESS_LEVELS must be greater than one");

// Local definitions
namespace {
    // These count the number of integer values in the range and are guaranteed greater than one
    constexpr uint16_t lightDiscreteValues = LIGHT_MAX - LIGHT_MIN + 1;
    constexpr uint16_t brightnessDiscreteValues = AUTO_BRIGHTNESS_MAX - AUTO_BRIGHTNESS_MIN + 1;

    // numDimmingLevels can be no greater than any of (NUM_BRIGHTNESS_LEVELS, lightDiscreteValues,
    // brightnessDiscreteValues). It is guaranteed greater than one.
    constexpr uint16_t temp = ((NUM_BRIGHTNESS_LEVELS < lightDiscreteValues) ?
                               NUM_BRIGHTNESS_LEVELS : lightDiscreteValues);
    constexpr uint16_t numDimmingLevels = ((temp < brightnessDiscreteValues) ?
                                           temp : brightnessDiscreteValues);

    // Used to divide lightScale into quantized brightness levels. Guaranteed greater than zero.
    constexpr uint16_t lightStepSize = ((lightDiscreteValues - 1) / numDimmingLevels) + 1;

    // Used to multiply brightness levels into screen brightness values. Brightest value possible
    // may be slightly less than AUTO_BRIGHTNESS_MAX due to quantization. Guaranteed greater
    // than zero.
    constexpr uint16_t brightnessStepSize = (brightnessDiscreteValues - 1) / (numDimmingLevels - 1);

    // Return the integer closest to val, subject to the interval [min, max]
    template <typename T>
    inline T clamp(T val, T min, T max) {
        return std::max(min, std::min(max, val));
    }
}

/////////////////////////////////////////////////////////////
// Static member function that returns the singleton instance
DimmingManager &DimmingManager::getInstance() {
    static DimmingManager instance;
    return instance;
}

/////////////////////////
// Setup the hardware for auto dimming
void DimmingManager::setup() {
    if (!AUTO_DIMMING_ENABLED) {
        return;
    }
    adcAttachPin(LIGHT_SENSE_PIN);
    this->lightIntensity_ = analogRead(LIGHT_SENSE_PIN);

    Log.info("Auto Dimming Enabled***********************************\n");
    Log.info("Light (min/max): %d/%d\n", LIGHT_MIN, LIGHT_MAX);
    Log.info("Brightness (min/max): %d/%d\n", AUTO_BRIGHTNESS_MIN, AUTO_BRIGHTNESS_MAX);
    Log.info("Levels (brightness/dimness): %d/%d\n", NUM_BRIGHTNESS_LEVELS, numDimmingLevels);
    Log.info("Discrete values (light/brightness): %d/%d\n", lightDiscreteValues,
             brightnessDiscreteValues);
    Log.info("Step sizes (light/brightness): %d %d\n", lightStepSize, brightnessStepSize);
    Log.info("*******************************************************\n");
}

///////////////////////////////////////////////////////////////////////////
// Adjust the display brightness to account for the ambient light intensity
void DimmingManager::updateBrightness(ScreenManager *sm, WidgetSet *ws) {
    // Update only at regular time intervals, so filtering has predictable behavior
    if (!AUTO_DIMMING_ENABLED || !this->isTimeToUpdate()) {
        return;
    }

    // Get the measured light intensity
    int16_t lightIntensity = analogRead(LIGHT_SENSE_PIN);

    // Filter to remove transients
    int16_t filtered = this->filter(lightIntensity);

    Log.info("Dimming Lgt:%d Filt:%d Lo:%d Hi:%d Lvl:%d Brt:%d\n",
             lightIntensity, filtered, this->lowerHysteresis_, this->upperHysteresis_,
             this->brightnessLevel_, this->screenBrightness_);

    // Apply hysteresis to avoid flickering on brightness boundaries. Brightness level will
    // be in the interval [0, numDimmingLevels).
    int16_t level = this->hysteresis(filtered);
    if (level < 0) {
        return;
    }
    this->brightnessLevel_ = static_cast<uint16_t>(level);

    // Compute new brightness value in the interval [AUTO_BRIGHTNESS_MIN, AUTO_BRIGHTNESS_MAX]
    this->screenBrightness_ = (this->brightnessLevel_ * brightnessStepSize) + AUTO_BRIGHTNESS_MIN;

    // Update the display
    sm->setBrightness(this->screenBrightness_);
    ws->drawCurrent(true);
}

///////////////////////////////////////////////////////////////////////////
// Returns whether or not it is time to update and, if so, resets the timer
bool DimmingManager::isTimeToUpdate() {
    constexpr uint64_t updateInterval_uS = 100000; // 0.1 seconds
    uint64_t currentTime_uS = esp_timer_get_time();

    // Compute elapsed time using unsigned arithmetic to correctly handle rollover
    uint64_t elapsedTime_uS = currentTime_uS - this->lastUpdateTime_uS_;

    if (elapsedTime_uS <= updateInterval_uS) {
        return false;
    }

    this->lastUpdateTime_uS_ = currentTime_uS;
    return true;
}

//////////////////////////////////////////////////////////////////////////////////
// Filter the measured light intensity to smooth transients (passing shadows, etc)
uint16_t DimmingManager::filter(uint16_t newLightIntensity) {
    // 0 <= alpha < 1; larger alpha produces more smoothing, but slower response time
    constexpr float alpha = 0.95;
    constexpr float oneMinusAlpha = 1.0 - alpha;

    this->lightIntensity_ = static_cast<uint16_t>((alpha * this->lightIntensity_) +
                                                  (oneMinusAlpha * newLightIntensity) + 0.5);
    return this->lightIntensity_;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Add hysteresis (stickiness) to brightness levels. This prevents flickering back and forth
// when the ambient light is near a brightness boundary. Compute brightness level in the
// interval [0, numDimmingLevels). Return brightness level or -1 if no change.
int16_t DimmingManager::hysteresis(uint16_t newLightIntensity) {
    if (newLightIntensity >= this->lowerHysteresis_ && newLightIntensity <= this->upperHysteresis_) {
        // Hysteresis thresholds not met, so don't update
        return -1;
    }

    // Quantize intensity to yield a discrete brightness level in the interval [0, numDimmingLevels)
    int16_t brightnessLevel =
        (clamp<uint16_t>(newLightIntensity, LIGHT_MIN, LIGHT_MAX) - LIGHT_MIN) / lightStepSize;

    // Compute the intensity cooresponding to the lower boundary of this brightness level
    uint16_t boundary = brightnessLevel * lightStepSize + LIGHT_MIN;

    // Compute new hysteresis thresholds
    uint16_t tolerance = lightStepSize / 10;
    this->lowerHysteresis_ = (boundary > tolerance) ? boundary - tolerance : 0;
    this->upperHysteresis_ = boundary + lightStepSize + tolerance;

    return static_cast<int16_t>(brightnessLevel);
}
