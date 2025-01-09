#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <vector>
#include "GlobalTime.h"
#include "Utils.h"
#include "PageSettings.h"


// TODO: serialize!

struct Settings {
private:
    uint8_t _GadgetCount;
    std::vector<PageSettings> _PageSettings;

    ScreenMode _ScreenMode = Dark;
    uint8_t _LightStartHour = 7;
    uint8_t _LightEndHour = 22;
    ScreenMode getActualScreenMode() {
        if (_ScreenMode == ScreenMode::Auto)
        {
            // TODO: doesn't work if someone flips for night shift
            auto lightestScreenMode = _ScreenMode;
            auto hour = GlobalTime::getInstance()->getHour24();
            if (!(hour >= _LightStartHour && hour < _LightEndHour))
            {
                return ScreenMode::Dark;
            }
        }
        return _ScreenMode;
    }

// TODO
//#define DIM_BRIGHTNESS 128 // Dim brightness (0-255)


public: 
    Settings() {
        // TODO temporary static
        _GadgetCount = 1;
    }


    void setScreenMode(ScreenMode screenMode) { _ScreenMode = screenMode; }
    uint16_t getBackgroundColor() { 
        auto sm = getActualScreenMode();
        return sm == ScreenMode::Dark ? TFT_BLACK : TFT_WHITE; 
    }
    uint16_t getForegroundColor() {
        auto sm = getActualScreenMode();
        return sm == ScreenMode::Dark ? TFT_WHITE : TFT_BLACK; 
    }
    uint16_t getInvertedBackgroundColor();
    uint16_t getInvertedForegroundColor();
};

#endif