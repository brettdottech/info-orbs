#ifndef WIFIMGR_CUSTOM_PARAMETERS_H
#define WIFIMGR_CUSTOM_PARAMETERS_H

#include <WiFiManager.h>

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
        init(id, placeholder, String(value).c_str(), length, "", WFM_LABEL_BEFORE);
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
        const char *checkboxHtml = value ? "type='checkbox' checked" : "type='checkbox'";
        init(id, placeholder, String(value).c_str(), length, checkboxHtml, WFM_LABEL_AFTER);
    }

    bool getValue() {
        Serial.printf("bool get val %s %d\n", WiFiManagerParameter::getID(), *WiFiManagerParameter::getValue());
        return String(WiFiManagerParameter::getValue()).toInt() == 1;
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