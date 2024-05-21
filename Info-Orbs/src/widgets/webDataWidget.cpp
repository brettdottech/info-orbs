
#include "widgets/webDataWidget.h"

#include <config.h>
#include <globalTime.h>

WebDataWidget::WebDataWidget(ScreenManager &manager) : Widget(manager) {
    m_lastUpdate = 0;
}

WebDataWidget::~WebDataWidget() {
}

void WebDataWidget::setup() {
}

void WebDataWidget::draw(bool force) {
    for (int i = 0; i < 5; i++) {
        String fingerprint = getFingerprint(i);
        if (fingerprint != m_fingerprint[i]) {
            m_manager.selectScreen(i);

            TFT_eSPI &display = m_manager.getDisplay();
            display.fillScreen(m_background[i]);
            display.setTextColor(m_labelColor[i]);
            display.setTextSize(2);
            display.setTextDatum(MC_DATUM);
            display.drawString(m_label[i], 120, 75, 2);
            display.setTextColor(m_color[i]);
            display.setTextDatum(MC_DATUM);
            if (m_data[i].length() > 10) {
                display.setTextSize(2);
            } else {
                display.setTextSize(3);
            }
            display.drawString(m_data[i], 120, 120, 2);
            m_fingerprint[i] = fingerprint;
        }
    }
}

void WebDataWidget::update(bool force) {
    if (force || m_lastUpdate == 0 || (millis() - m_lastUpdate) >= m_updateDelay) {
        HTTPClient http;
        http.begin(httpRequestAddress);
        int httpCode = http.GET();

        if (httpCode > 0) {  // Check for the returning code
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, http.getString());
            if (!error) {
                for (int8_t i = 0; i < 5; i++) {
                    m_label[i] = doc[i]["label"].as<String>();
                    m_data[i] = doc[i]["data"].as<String>();
                    if (doc[i].containsKey("color")) {
                        m_color[i] = stringToColor(doc[i]["color"].as<String>());
                    } else {
                        m_color[i] = m_defaultColor;
                    }
                    if (doc[i].containsKey("labelColor")) {
                        m_labelColor[i] = stringToColor(doc[i]["labelColor"].as<String>());
                    } else {
                        m_labelColor[i] = m_color[i];
                    }
                    if (doc[i].containsKey("background")) {
                        m_background[i] = stringToColor(doc[i]["background"].as<String>());
                    } else {
                        m_background[i] = m_defaultBackground;
                    }
                }

                m_lastUpdate = millis();
            } else {
                // Handle JSON deserialization error
                Serial.println("deserializeJson() failed");
            }
        } else {
            // Handle HTTP request error
            Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
}

int32_t WebDataWidget::stringToColor(String color) {
    color.toLowerCase();
    color.replace(" ", "");
    if (color == "black") {
        return TFT_BLACK;
    } else if (color == "navy") {
        return TFT_NAVY;
    } else if (color == "darkgreen") {
        return TFT_DARKGREEN;
    } else if (color == "darkcyan") {
        return TFT_DARKCYAN;
    } else if (color == "maroon") {
        return TFT_MAROON;
    } else if (color == "purple") {
        return TFT_PURPLE;
    } else if (color == "olive") {
        return TFT_OLIVE;
    } else if (color == "lightgrey") {
        return TFT_LIGHTGREY;
    } else if (color == "darkgrey") {
        return TFT_DARKGREY;
    } else if (color == "blue") {
        return TFT_BLUE;
    } else if (color == "green") {
        return TFT_GREEN;
    } else if (color == "cyan") {
        return TFT_CYAN;
    } else if (color == "red") {
        return TFT_RED;
    } else if (color == "magenta") {
        return TFT_MAGENTA;
    } else if (color == "yellow") {
        return TFT_YELLOW;
    } else if (color == "white") {
        return TFT_WHITE;
    } else if (color == "orange") {
        return TFT_ORANGE;
    } else if (color == "greenyellow") {
        return TFT_GREENYELLOW;
    } else if (color == "pink") {
        return TFT_PINK;
    } else if (color == "brown") {
        return TFT_BROWN;
    } else if (color == "gold") {
        return TFT_GOLD;
    } else if (color == "silver") {
        return TFT_SILVER;
    } else if (color == "skyblue") {
        return TFT_SKYBLUE;
    } else if (color == "vilolet") {
        return TFT_VIOLET;
    } else {
        Serial.print("Invalid color: ");
        Serial.println(color);
        return TFT_BLACK;
    }
}

String WebDataWidget::getFingerprint(int8_t display) {
    return m_label[display] + "~" + m_data[display] + "~" + m_color[display] + "~" + m_background[display];
}