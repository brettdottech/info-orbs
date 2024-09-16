
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

            if (data->getElementsCount() > 0) {
                for (int i = 0; i < data->getElementsCount(); i++) {
                    WebDataElementModel element = data->getElement(i);
                    switch (element.getTypeEnum()) {
                        case TEXT:
                            display.setTextColor(element.getColor(), element.getBackgroundColor());
                            display.drawString(element.getText(), element.getX(), element.getY(), element.getFont());
                            break;
                        case CHARACTER:
                            display.setTextColor(element.getColor(), element.getBackgroundColor());
                            display.drawChar(element.getX(), element.getY(), element.getText().c_str()[0], element.getColor(), element.getBackgroundColor(), element.getFont());
                            break;
                        case LINE:
                            display.drawLine(element.getX(), element.getY(), element.getX2(), element.getY2(), element.getColor());
                            break;
                        case RECTANGLE:
                            if (element.getFilled()) {
                                display.fillRect(element.getX(), element.getY(), element.getWidth(), element.getHeight(), element.getColor());
                            } else {
                                display.drawRect(element.getX(), element.getY(), element.getWidth(), element.getHeight(), element.getColor());
                            }
                            break;
                        case TRIANGLE:
                            if (element.getFilled()) {
                                display.fillTriangle(
                                    element.getX(),
                                    element.getY(),
                                    element.getX2(),
                                    element.getY2(),
                                    element.getX3(),
                                    element.getY3(),
                                    element.getColor());
                            } else {
                                display.drawTriangle(
                                    element.getX(),
                                    element.getY(),
                                    element.getX2(),
                                    element.getY2(),
                                    element.getX3(),
                                    element.getY3(),
                                    element.getColor());
                            }
                        case CIRCLE:
                            if (element.getFilled()) {
                                display.fillCircle(element.getX(), element.getY(), element.getRadius(), element.getColor());
                            } else {
                                display.drawCircle(element.getX(), element.getY(), element.getRadius(), element.getColor());
                            }
                            break;
                        case ARC:
                            display.drawArc(
                                element.getX(),
                                element.getY(),
                                element.getRadius(),
                                element.getWidth(),
                                element.getArcAngle1(),
                                element.getArcAngle2(),
                                element.getColor(),
                                element.getBackgroundColor(),
                                true);
                            break;
                        case IMAGE:
                            // TBD
                            // display.drawXBitmap(element.getX(), element.getY(), element.getImage(), element.getWidth(), element.getHeight(), element.getColor());
                            break;
                        case OTHER:
                            Serial.println("WebDataWidget::draw: Other type not supported");
                            break;
                    }
                }
            } else {
                String wrappedLines[MAX_WRAPPED_LINES];
                String dataValues = data->getData();
                int lineCount = Utils::getWrappedLines(wrappedLines, dataValues, 10);
                int height = display.fontHeight() + 10;
                for (int i = 0; i < lineCount; i++) {
                    display.drawString(wrappedLines[i], 120, yOffset + (height * i), 2);
                }
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
                JsonVariant array = doc.as<JsonArray>();
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
