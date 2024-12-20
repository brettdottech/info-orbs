#include "MainHelper.h"
#include "config_helper.h"
#include "icons.h"

static Button buttonLeft;
static Button buttonMiddle;
static Button buttonRight;
static WiFiManager *s_wifiManager = nullptr;
static ConfigManager *s_configManager = nullptr;
static ScreenManager *s_screenManager = nullptr;
static WidgetSet *s_widgetSet = nullptr;
static int s_widgetCycleDelay = WIDGET_CYCLE_DELAY;
static unsigned long s_widgetCycleDelayPrev = 0;
static int s_orbRotation = ORB_ROTATION;
static std::string s_timezoneLocation = TIMEZONE_API_LOCATION;
static std::string s_ntpServer = NTP_SERVER;
static int s_tftBrightness = TFT_BRIGHTNESS;
static bool s_nightMode = DIM_ENABLED;
static int s_dimStartHour = DIM_START_HOUR;
static int s_dimEndHour = DIM_END_HOUR;
static int s_dimBrightness = DIM_BRIGHTNESS;

void MainHelper::init(WiFiManager *wm, ConfigManager *cm, ScreenManager *sm, WidgetSet *ws) {
    s_wifiManager = wm;
    s_configManager = cm;
    s_screenManager = sm;
    s_widgetSet = ws;
}

/**
 * The ISR handlers must be static
 */
void MainHelper::isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void MainHelper::isrButtonChangeMiddle() { buttonMiddle.isrButtonChange(); }
void MainHelper::isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void MainHelper::setupButtons() {
    bool invertButtons = s_orbRotation == 1 || s_orbRotation == 2;
    int leftPin = BUTTON_LEFT_PIN;
    int middlePin = BUTTON_MIDDLE_PIN;
    int rightPin = BUTTON_RIGHT_PIN;

    if (invertButtons) {
        leftPin = BUTTON_RIGHT_PIN;
        rightPin = BUTTON_LEFT_PIN;
    }

    buttonLeft.begin(leftPin);
    buttonMiddle.begin(middlePin);
    buttonRight.begin(rightPin);

    attachInterrupt(digitalPinToInterrupt(leftPin), isrButtonChangeLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(middlePin), isrButtonChangeMiddle, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rightPin), isrButtonChangeRight, CHANGE);
}

void MainHelper::setupConfig() {
    s_configManager->addConfigString("General", "timezoneLoc", &s_timezoneLocation, 30, "Timezone Location, use one from <a href='https://timezonedb.com/time-zones' target='blank'>this list</a>");
    s_configManager->addConfigInt("General", "widgetCycDelay", &s_widgetCycleDelay, "Automatically cycle widgets every X seconds, set to 0 to disable");
    s_configManager->addConfigString("General", "ntpServer", &s_ntpServer, 30, "NTP server", true);

    String optRotation[] = {"No rotation", "Rotate 90° clockwise", "Rotate 180°", "Rotate 270° clockwise"};
    s_configManager->addConfigComboBox("TFT Settings", "orbRotation", &s_orbRotation, optRotation, 4, "Orb rotation");
    s_configManager->addConfigBool("TFT Settings", "nightmode", &s_nightMode, "Enable Nighttime mode");
    s_configManager->addConfigInt("TFT Settings", "tftBrightness", &s_tftBrightness, "TFT Brightness [0-255]", true);
    String optHours[] = {"0:00", "1:00", "2:00", "3:00", "4:00", "5:00", "6:00", "7:00", "8:00", "9:00", "10:00", "11:00",
                         "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00", "20:00", "21:00", "22:00", "23:00"};
    s_configManager->addConfigComboBox("TFT Settings", "dimStartHour", &s_dimStartHour, optHours, 24, "Nighttime Start [24h format]", true);
    s_configManager->addConfigComboBox("TFT Settings", "dimEndHour", &s_dimEndHour, optHours, 24, "Nighttime End [24h format]", true);
    s_configManager->addConfigInt("TFT Settings", "dimBrightness", &s_dimBrightness, "Nighttime Brightness [0-255]", true);
}

void MainHelper::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (state == BTN_NOTHING) {
        // nothing to do
        return;
    }
    // Reset cycle timer whenever a button is pressed
    if (buttonId == BUTTON_LEFT && state == BTN_SHORT) {
        // Left short press cycles widgets backward
        Serial.println("Left button short pressed -> switch to prev Widget");
        s_widgetCycleDelayPrev = millis();
        s_widgetSet->prev();
    } else if (buttonId == BUTTON_RIGHT && state == BTN_SHORT) {
        // Right short press cycles widgets forward
        Serial.println("Right button short pressed -> switch to next Widget");
        s_widgetCycleDelayPrev = millis();
        s_widgetSet->next();
    } else {
        // Everying else that is not BTN_NOTHING will be forwarded to the current widget
        if (buttonId == BUTTON_LEFT) {
            Serial.printf("Left button pressed, state=%d\n", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_LEFT, state);
        } else if (buttonId == BUTTON_MIDDLE) {
            Serial.printf("Middle button pressed, state=%d\n", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_MIDDLE, state);
        } else if (buttonId == BUTTON_RIGHT) {
            Serial.printf("Right button pressed, state=%d\n", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_RIGHT, state);
        }
    }
}

void MainHelper::checkButtons() {
    ButtonState leftState = buttonLeft.getState();
    if (leftState != BTN_NOTHING) {
        buttonPressed(BUTTON_LEFT, leftState);
    }
    ButtonState middleState = buttonMiddle.getState();
    if (middleState != BTN_NOTHING) {
        buttonPressed(BUTTON_MIDDLE, middleState);
    }
    ButtonState rightState = buttonRight.getState();
    if (rightState != BTN_NOTHING) {
        buttonPressed(BUTTON_RIGHT, rightState);
    }
}

void MainHelper::checkCycleWidgets() {
    if (s_widgetSet && s_widgetCycleDelay > 0 && (s_widgetCycleDelayPrev == 0 || (millis() - s_widgetCycleDelayPrev) >= s_widgetCycleDelay * 1000)) {
        s_widgetSet->next();
        s_widgetCycleDelayPrev = millis();
    }
}

// Handle simulated button state
void MainHelper::handleEndpointButton() {
    if (s_wifiManager->server->hasArg("name") && s_wifiManager->server->hasArg("state")) {
        String inButton = s_wifiManager->server->arg("name");
        String inState = s_wifiManager->server->arg("state");
        uint8_t buttonId = Utils::stringToButtonId(inButton);
        ButtonState state = Utils::stringToButtonState(inState);

        if (buttonId != 0 && state != BTN_NOTHING) {
            buttonPressed(buttonId, state);
            s_wifiManager->server->send(200, "text/plain", "OK " + inButton + "/" + inState + " -> " + String(buttonId) + "/" + String(state));
            return;
        }
    }
    s_wifiManager->server->send(500, "text/plain", "ERR");
}

// Show button web page
void MainHelper::handleEndpointButtons() {
    String msg = WEBPORTAL_BUTTONS_PAGE_START1;
    msg += WEBPORTAL_BUTTONS_STYLE;
    msg += WEBPORTAL_BUTTONS_PAGE_START2;
    String buttons[] = {"left", "middle", "right"};
    String states[] = {"short", "medium", "long"};
    int numButtons = 3; // number of buttons
    int numStates = 3; // number of states
    for (int s = 0; s < numStates; s++) {
        msg += "<tr>";
        for (int b = 0; b < numButtons; b++) {
            msg += "<td><button class='sim' onclick=\"sendReq('" + buttons[b] + "', '" + states[s] + "')\">" + buttons[b] + "<br>" + states[s] + "</button></td>";
        }
        msg += "</tr>";
    }
    msg += WEBPORTAL_BUTTONS_PAGE_END1;
    msg += WEBPORTAL_BUTTONS_SCRIPT;
    msg += WEBPORTAL_BUTTONS_PAGE_END2;
    s_wifiManager->server->send(200, "text/html", msg);
}

void MainHelper::setupWebPortalEndpoints() {
    // To simulate button presses call e.g. http://<ip>/button?name=right&state=short
    s_wifiManager->server->on("/button", handleEndpointButton);
    s_wifiManager->server->on("/buttons", handleEndpointButtons);
}

void MainHelper::showWelcome() {
    s_screenManager->fillAllScreens(TFT_BLACK);
    s_screenManager->setFontColor(TFT_WHITE);

    s_screenManager->selectScreen(0);
    s_screenManager->drawCentreString("Welcome", ScreenCenterX, ScreenCenterY, 29);
    if (GIT_BRANCH != "main" && GIT_BRANCH != "unknown" && GIT_BRANCH != "HEAD") {
        s_screenManager->setFontColor(TFT_RED);
        s_screenManager->drawCentreString(GIT_BRANCH, ScreenCenterX, ScreenCenterY - 40, 15);
        s_screenManager->drawCentreString(GIT_COMMIT_ID, ScreenCenterX, ScreenCenterY + 40, 15);
        s_screenManager->setFontColor(TFT_WHITE);
    }

    s_screenManager->selectScreen(1);
    s_screenManager->drawCentreString("Info Orbs", ScreenCenterX, ScreenCenterY - 50, 22);
    s_screenManager->drawCentreString("by", ScreenCenterX, ScreenCenterY - 5, 22);
    s_screenManager->drawCentreString("brett.tech", ScreenCenterX, ScreenCenterY + 30, 22);
    s_screenManager->setFontColor(TFT_RED);
    s_screenManager->drawCentreString("version: 1.1.0", ScreenCenterX, ScreenCenterY + 65, 15);

    s_screenManager->selectScreen(2);
    s_screenManager->drawJpg(0, 0, logo_start, logo_end - logo_start);
}

void MainHelper::resetCycleTimer() {
    s_widgetCycleDelayPrev = millis();
}

void MainHelper::updateBrightnessByTime(uint8_t hour24) {
    uint8_t newBrightness;
    if (s_nightMode) {
        bool isInDimRange;
        if (s_dimStartHour < s_dimEndHour) {
            // Normal case: the range does not cross midnight
            isInDimRange = (hour24 >= s_dimStartHour && hour24 < s_dimEndHour);
        } else {
            // Case where the range crosses midnight
            isInDimRange = (hour24 >= s_dimStartHour || hour24 < s_dimEndHour);
        }
        newBrightness = isInDimRange ? s_dimBrightness : s_tftBrightness;
    } else {
        newBrightness = s_tftBrightness;
    }

    if (s_screenManager->setBrightness(newBrightness)) {
        // brightness was changed -> update widget
        s_screenManager->clearAllScreens();
        s_widgetSet->drawCurrent(true);
    }
}

void MainHelper::restartIfNecessary() {
    if (s_configManager->requiresRestart()) {
        uint32_t start = millis();
        while (millis() - start < 1000) {
            // Answer webportal requests for a short time before restarting
            // to avoid a browser timeout
            s_wifiManager->process();
        }
        Serial.println("Restarting ESP now");
        ESP.restart();
    }
}