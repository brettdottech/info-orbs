
#include "widgets/webDataWidget.h"

WebDataWidget::WebDataWidget(ScreenManager &manager, String url) : Widget(manager) {
    httpRequestAddress = url;

    m_lastUpdate = 0;
    for (int i = 0; i < 5; i++) {
        m_obj[i] = WebDataModel();
    }
}

WebDataWidget::~WebDataWidget() {
}

void WebDataWidget::setup() {
}

void WebDataWidget::changeMode() {
}

void WebDataWidget::draw(bool force) {
    for (int i = 0; i < 5; i++) {
        WebDataModel *data = &m_obj[i];
        if (data->isChanged() || force) {
            m_manager.selectScreen(i);
            TFT_eSPI &display = m_manager.getDisplay();
            display.fillScreen(data->getBackgroundColor());
            display.setTextColor(data->getLabelColor());
            display.setTextSize(2);
            display.setTextDatum(MC_DATUM);
            display.drawString(data->getLabel(), 120, 70, 2);
            display.setTextColor(data->getDataColor());
            display.setTextDatum(MC_DATUM);
            int yOffset = 110;

            String wrappedLines[MAX_WRAPPED_LINES];
            String dataValues = data->getData();
            int lineCount = Utils::getWrappedLines(wrappedLines, dataValues, 10);
            int height = display.fontHeight() + 10;
            for (int i = 0; i < lineCount; i++) {
                display.drawString(wrappedLines[i], 120, yOffset + (height*i), 2);
            }
            data->setChangedStatus(false);
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
                    WebDataModel *data = &m_obj[i];
                    data->setLabel(doc[i]["label"].as<String>());
                    data->setData(doc[i]["data"].as<String>());
                    if (doc[i].containsKey("color")) {
                        data->setDataColor(doc[i]["color"].as<String>());
                    } else {
                        data->setDataColor(m_defaultColor);
                    }
                    if (doc[i].containsKey("labelColor")) {
                        data->setLabelColor(doc[i]["labelColor"].as<String>());
                    } else {
                        data->setLabelColor(data->getDataColor());
                    }
                    if (doc[i].containsKey("background")) {
                        data->setBackgroundColor(doc[i]["background"].as<String>());
                    } else {
                        data->setBackgroundColor(m_defaultBackground);
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
