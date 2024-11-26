
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

void WebDataWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
}

void WebDataWidget::draw(bool force) {
    for (int i = 0; i < 5; i++) {
        WebDataModel *data = &m_obj[i];
        if (force) {
            data->setInitializedStatus(false);
        }
        if (data->isChanged() || force) {
            m_manager.selectScreen(i);
            data->draw(m_manager);

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
                if (doc["interval"].is<int>()) {
                    m_updateDelay = doc["interval"];
                }
                JsonVariant array;
                if (doc["displays"].is<JsonArray>()) {
                    array = doc["displays"].as<JsonArray>();
                } else {
                    // Handle legacy response that doesn't have response level data
                    array = doc.as<JsonArray>();
                }
                for (int i = 0; i < array.size(); i++) {
                    m_obj[i].parseData(array[i].as<JsonObject>(), m_defaultColor, m_defaultBackground);
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

String WebDataWidget::getName() {
    return "WebData";
}