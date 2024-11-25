#include "model/webDataModel.h"

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

void WebDataModel::setData(String data, int32_t defaultColor, int32_t defaultBackground) {
    if (m_data != data) {
        m_data = data;
        setElementsCount(0);
        m_changed = true;
    }
}
void WebDataModel::setData(JsonArray data, int32_t defaultColor, int32_t defaultBackground) {
    setElementsCount(data.size());
    for (int i = 0; i < data.size(); i++) {
        m_elements[i].parseData(data[i], defaultColor, defaultBackground);
    }
    m_changed = true;
}

const WebDataElementModel &WebDataModel::getElement(int index) {
    return m_elements[index];
}

int32_t WebDataModel::getElementsCount() {
    return m_elementsCount;
}

void WebDataModel::initElements(int32_t count) {
    if (m_elementsCount > 0) {
        delete[] m_elements;
    }
    m_elements = new WebDataElementModel[count];
}
void WebDataModel::setElementsCount(int32_t count) {
    if (m_elementsCount != count) {
        initElements(count);
        m_elementsCount = count;
    }
}

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

bool WebDataModel::isFullDraw() {
    return m_fullDraw;
}

void WebDataModel::setFullDrawStatus(bool fullDraw) {
    m_fullDraw = fullDraw;
}

bool WebDataModel::isChanged() {
    return m_changed;
}

void WebDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
}

void WebDataModel::parseData(const JsonObject &doc, int32_t defaultColor, int32_t defaultBackground) {
    if (const char *label = doc["label"]) {
        setLabel(label);
    }
    if (doc["data"].is<JsonArray>()) {
        setData(doc["data"].as<JsonArray>(), defaultColor, defaultBackground);
    } else if (const char *data = doc["data"]) {
        setData(data, defaultColor, defaultBackground);
    } else if (String data = doc["data"].as<String>()) {
        setData(data, defaultColor, defaultBackground);
    }
    if (const char *color = doc["color"]) {
        setDataColor(color);
    } else {
        setDataColor(defaultColor);
    }
    if (const char *labelColor = doc["labelColor"]) {
        setLabelColor(labelColor);
    } else {
        setLabelColor(defaultColor);
    }
    if (const char *background = doc["background"]) {
        setBackgroundColor(background);
    } else {
        setBackgroundColor(defaultBackground);
    }
    if (doc["fullDraw"].is<bool>()) {
        setFullDrawStatus(doc["fullDraw"].as<bool>());
    } else {
        setFullDrawStatus(false);
    }
}

bool WebDataModel::isInitialized() {
    return m_isInitialized;
}

void WebDataModel::setInitializedStatus(bool initialized) {
    m_isInitialized = initialized;
}

void WebDataModel::draw(ScreenManager& manager) {
    if (!m_isInitialized || isFullDraw()) {
        manager.fillScreen(getBackgroundColor());
        m_isInitialized = true;
    }
    if (getLabel()) {
        manager.setLegacyTextColor(getLabelColor());
        manager.setLegacyTextSize(2);
        manager.setLegacyTextDatum(MC_DATUM);
        manager.drawLegacyString(getLabel(), 120, 70, 2);
    }
    manager.setLegacyTextDatum(MC_DATUM);

    if (getElementsCount() > 0) {
        for (int i = 0; i < getElementsCount(); i++) {
            WebDataElementModel element = getElement(i);
            element.draw(manager);
        }
    } else {
        manager.setLegacyTextColor(getDataColor(), getBackgroundColor());

        String wrappedLines[MAX_WRAPPED_LINES];
        String dataValues = getData();
        int yOffset = 110;
        int lineCount = Utils::getWrappedLines(wrappedLines, dataValues, 10);
        int height = manager.getLegacyFontHeight() + 10;
        for (int i = 0; i < lineCount; i++) {
            manager.drawLegacyString(wrappedLines[i], 120, yOffset + (height * i), 2);
        }
    }
}
