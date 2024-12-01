#ifndef WIFIMGR_CUSTOM_PARAMETERS_H
#define WIFIMGR_CUSTOM_PARAMETERS_H

#include <WiFiManager.h>

const char checkboxHtml[] = "type='checkbox'";
const char checkboxHtmlChecked[] = "type='checkbox' checked";
const char colorHtml[] = "type='color'";
const char numberHtml[] = "type='number'";

class IPAddressParameter : public WiFiManagerParameter {
public:
    IPAddressParameter(const char *id, const char *placeholder, IPAddress address)
        : WiFiManagerParameter("") {
        init(id, placeholder, address.toString().c_str(), 16, "", WFM_LABEL_BEFORE);
    }

    bool getValue(IPAddress &ip) {
        return ip.fromString(WiFiManagerParameter::getValue());
    }
};

class IntParameter : public WiFiManagerParameter {
public:
    IntParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10)
        : WiFiManagerParameter("") {
        init(id, placeholder, String(value).c_str(), length, numberHtml, WFM_LABEL_BEFORE);
    }

    long getValue() {
        return String(WiFiManagerParameter::getValue()).toInt();
    }
};

class BoolParameter : public WiFiManagerParameter {
public:
    BoolParameter(const char *id, const char *placeholder, bool value, const uint8_t length = 2)
        : WiFiManagerParameter("") {
        Serial.printf("creating bool param: %s %d\n", id, value);
        // We use 1 as value here and check later if the argument was passed back when submitting the form
        // If the checkbox is checked, the argument will be there
        init(id, placeholder, "1", length, value ? checkboxHtmlChecked : checkboxHtml, WFM_LABEL_AFTER);
    }

    bool getValue() {
        Serial.printf("bool get val %s %s\n", WiFiManagerParameter::getID(), WiFiManagerParameter::getValue());
        return String(WiFiManagerParameter::getValue()).toInt() == 1;
    }
};

class ColorParameter : public WiFiManagerParameter {
public:
    ColorParameter(const char *id, const char *placeholder, int value, const uint8_t length = 10)
        : WiFiManagerParameter("") {
        Serial.printf("creating color param: %s %d %s\n", id, value, String(value, 16));
        // We use 1 as value here and check later if the argument was passed back when submitting the form
        // If the checkbox is checked, the argument will be there
        init(id, placeholder, ("#" + String(Utils::rgb565ToRgb888(value), 16)).c_str(), length, colorHtml, WFM_LABEL_BEFORE);
    }

    int getValue() {
        String value = String(WiFiManagerParameter::getValue()).substring(1);
        int rgb888 = strtol(value.c_str(), 0, 16);
        int rgb565 = Utils::rgb888ToRgb565(rgb888);
        Serial.printf("color get val %s %s %s %d %d\n", WiFiManagerParameter::getID(), WiFiManagerParameter::getValue(), value, rgb888, rgb565);
        return rgb565;
    }
};

class FloatParameter : public WiFiManagerParameter {
public:
    FloatParameter(const char *id, const char *placeholder, float value, const uint8_t length = 10)
        : WiFiManagerParameter("") {
        init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
    }

    float getValue() {
        return String(WiFiManagerParameter::getValue()).toFloat();
    }
};

#endif // WIFIMGR_CUSTOM_PARAMETERS_H