#include "model/webDataElementTextModel.h"

int32_t WebDataElementTextModel::getX() {
    return m_x;
}

void WebDataElementTextModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementTextModel::getY() {
    return m_y;
}

void WebDataElementTextModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

String WebDataElementTextModel::getText() {
    return m_text;
}

void WebDataElementTextModel::setText(String text) {
    if (m_text != text) {
        m_text = text;
        m_changed = true;
    }
}

int32_t WebDataElementTextModel::getFont() {
    return m_font;
}

void WebDataElementTextModel::setFont(int32_t font) {
    if (m_font != font) {
        m_font = font;
        m_changed = true;
    }
}

int32_t WebDataElementTextModel::getSize() {
    return m_size;
}
void WebDataElementTextModel::setSize(int32_t size) {
    if (m_size != size) {
        m_size = size;
        m_changed = true;
    }
}
int32_t WebDataElementTextModel::getAlignment() {
    return m_alignment;
}
void WebDataElementTextModel::setAlignment(int32_t alignment) {
    if (m_alignment != alignment) {
        m_alignment = alignment;
        m_changed = true;
    }
}

void WebDataElementTextModel::setAlignment(String alignment) {
    int32_t newAlignment = Utils::stringToAlignment(alignment);
    if (m_alignment != newAlignment) {
        m_alignment = newAlignment;
        m_changed = true;
    }
}

int32_t WebDataElementTextModel::getBackgroundColor() {
    return m_background;
}

void WebDataElementTextModel::setBackgroundColor(int32_t background) {
    if (m_background != background) {
        m_background = background;
        m_changed = true;
    }
}

void WebDataElementTextModel::setBackgroundColor(String background) {
    setBackgroundColor(Utils::stringToColor(background));
}

void WebDataElementTextModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementTextModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementTextModel::getColor() {
    return m_color;
}

void WebDataElementTextModel::parseData(const JsonObject &doc, int32_t defaultColor, int32_t defaultBackground) {
    if (doc["x"].is<int32_t>()) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc["y"].is<int32_t>()) {
        setY(doc["y"].as<int32_t>());
    }
    if (const char *text = doc["text"]) {
        setText(text);
    }
    if (doc["font"].is<int32_t>()) {
        setFont(doc["font"].as<int32_t>());
    }
    if (doc["size"].is<int32_t>()) {
        setSize(doc["size"].as<int32_t>());
    }
    if (const char *alignment = doc["alignment"]) {
        setAlignment(alignment);
    }
    if (const char *color = doc["color"]) {
        setColor(color);
    } else {
        setColor(defaultColor);
    }
    if (const char *background = doc["background"]) {
        setBackgroundColor(background);
    } else {
        setBackgroundColor(defaultBackground);
    }
}

void WebDataElementTextModel::draw(ScreenManager& manager) {
    manager.setLegacyTextFont(getFont());
    manager.setLegacyTextDatum(getAlignment());
    manager.setLegacyTextSize(getSize());
    manager.setLegacyTextColor(getColor(), getBackgroundColor());
    manager.drawLegacyString(getText(), getX(), getY(), getFont());
}
