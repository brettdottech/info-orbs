#include "widgets/webDataModel.h"

String WebDataModel::getLabel() {
    return m_label;
}

void WebDataModel::setLabel(String label) {
    if (m_label != label) {
        m_label = label;
        m_changed = true;
    }
}

String WebDataModel::getData() {
    return m_data;
}

void WebDataModel::setData(String data) {
    if (m_data != data) {
        m_data = data;
        Serial.print(m_label);
        Serial.print(": ");
        Serial.print(m_data[0]);
        Serial.print(" - ");
        Serial.println(m_data);
        if (data[0] == '[') {
            Serial.println("JSON");
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data);
            setElementsCount(doc.size());
            Serial.print("Count: " + String(m_elementsCount));
            for (int i = 0; i < doc.size(); i++) {
            // Serial.print(i);
            Serial.print(" i: " + String(i));
                WebDataElementModel element = WebDataElementModel();
            Serial.print("  ");
            Serial.print(i);
                element.parseData(doc[i].as<JsonObject>());
            Serial.print(" X: ");
            Serial.print(element.getX());
            Serial.print(" Y: ");
            Serial.print(element.getY());
            Serial.print(" Text: ");
            Serial.print(element.getText());

            Serial.print(" Type: ");
            Serial.print(element.getType());
            Serial.println("");
            m_elements[i] = element;
            }
        }
        m_changed = true;
    }
}

WebDataElementModel& WebDataModel::getElements() {
    return *m_elements;
}

WebDataElementModel WebDataModel::getElement(int index) {
    return m_elements[index];
}

int32_t WebDataModel::getElementsCount() {
    return m_elementsCount;
}

void WebDataModel::setElementsCount(int32_t count) {
    if (m_elementsCount != count) {
        m_elements = (WebDataElementModel *)malloc(sizeof(WebDataElementModel) * count);
        m_elementsCount = count;
        m_changed = true;
    }
}

// void WebDataModel::setElements(WebDataElementModel *element) {
//     if (m_element != element) {
//         m_element = element;
//         m_changed = true;
//     }
// }

int32_t WebDataModel::getLabelColor() {
    return m_labelColor;
}

void WebDataModel::setLabelColor(int32_t color) {
    if (m_labelColor != color) {
        m_labelColor = color;
        m_changed = true;
    }
}

void WebDataModel::setLabelColor(String color) {
    setLabelColor(Utils::stringToColor(color));
}

int32_t WebDataModel::getDataColor() {
    return m_color;
}

void WebDataModel::setDataColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataModel::setDataColor(String color) {
    setDataColor(Utils::stringToColor(color));
}

int32_t WebDataModel::getBackgroundColor() {
    return m_background;
}

void WebDataModel::setBackgroundColor(int32_t background) {
    if (m_background != background) {
        m_background = background;
        m_changed = true;
    }
}

void WebDataModel::setBackgroundColor(String background) {
    setBackgroundColor(Utils::stringToColor(background));
}

bool WebDataModel::isChanged() {
    return m_changed;
}

void WebDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
}

void WebDataModel::parseData(JsonObject doc, int32_t defaultColor, int32_t defaultBackground) {
    if (doc.containsKey("label")) {
        setLabel(doc["label"].as<String>());
    }
    if (doc.containsKey("data")) {
        setData(doc["data"].as<String>());
    }
    if (doc.containsKey("color")) {
        setDataColor(doc["color"].as<String>());
    } else {
        setDataColor(defaultColor);
    }
    if (doc.containsKey("labelColor")) {
        setLabelColor(doc["labelColor"].as<String>());
    } else {
        setLabelColor(getDataColor());
    }
    if (doc.containsKey("background")) {
        setBackgroundColor(doc["background"].as<String>());
    } else {
        setBackgroundColor(defaultBackground);
    }
}