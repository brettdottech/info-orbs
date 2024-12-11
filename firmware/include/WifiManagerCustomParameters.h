#ifndef WIFIMGR_CUSTOM_PARAMETERS_H
#define WIFIMGR_CUSTOM_PARAMETERS_H

#include "OrbsWiFiManager.h"

const char checkboxHtml[] = "type='checkbox'";
const char checkboxHtmlChecked[] = "type='checkbox' checked";
const char colorHtml[] = "type='color'";
const char numberHtml[] = "type='number'";

class StringParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
public:
    StringParameter(const char *id, const char *placeholder, std::string value, const uint8_t length = 30)
        : WiFiManagerParameter(id, placeholder, value.c_str(), length) {
    }

    std::string getValue() {
        return WiFiManagerParameter::getValue();
    }
};

class IPAddressParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
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
    using WiFiManagerParameter::getValue; // make parent function private
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
    using WiFiManagerParameter::getValue; // make parent function private
public:
    BoolParameter(const char *id, const char *placeholder, bool value, const uint8_t length = 2)
        : WiFiManagerParameter("") {
        // We use 1 as value here and check later if the argument was passed back when submitting the form
        // If the checkbox is checked, the argument will be there
        init(id, placeholder, "1", length, value ? checkboxHtmlChecked : checkboxHtml, WFM_LABEL_BEFORE);
    }

    bool getValue(WiFiManager &wm) {
        // If the checkbox is selected, the call to "/paramsave" will have an argument with our id
        return wm.server->hasArg(_id);
    }
};

class ColorParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
public:
    ColorParameter(const char *id, const char *placeholder, int value, const uint8_t length = 10)
        : WiFiManagerParameter("") {
        init(id, placeholder, ("#" + String(Utils::rgb565ToRgb888(value), 16)).c_str(), length, colorHtml, WFM_LABEL_BEFORE);
    }

    int getValue() {
        String value = String(WiFiManagerParameter::getValue()).substring(1);
        int rgb888 = strtol(value.c_str(), 0, 16);
        int rgb565 = Utils::rgb888ToRgb565(rgb888);
        return rgb565;
    }
};

class ComboBoxParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
public:
    ComboBoxParameter(const char *id, const char *placeholder, String options[], int numOptions, int value, const uint8_t length = 10)
        : WiFiManagerParameter("") {
        String html = "<label for='" + String(id) + "'>" + String(placeholder) + "</label><br/><select id='" + String(id) + "' name='" + String(id) + "'>";
        for (int i = 0; i < numOptions; i++) {
            html += "<option value='" + String(i) + "'" + (value == i ? " selected>" : ">") + options[i] + "</option>";
        }
        html += "</select>";
        // Set id as label here, otherwise WifiMgr will show an input
        init(NULL, id, nullptr, 0, Utils::copyString(html.c_str()), WFM_LABEL_AFTER);
    }

    int getValue(WiFiManager &wm) {
        Serial.println(_label);
        // The combobox will return its value via "/paramsave" argument
        if (wm.server->hasArg(_label)) {
            String arg = wm.server->arg(_label);
            Serial.println(arg);
            return arg.toInt();
        }
        // Fallback to 0
        return 0;
    }
};

class FloatParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
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