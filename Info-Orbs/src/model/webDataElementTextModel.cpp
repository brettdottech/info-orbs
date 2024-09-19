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

void WebDataElementTextModel::parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) {
    // Serial.print("Parsing Data Text");

    if (doc.containsKey("x")) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc.containsKey("y")) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc.containsKey("text")) {
        setText(doc["text"].as<String>());
    }
    if (doc.containsKey("font")) {
        setFont(doc["font"].as<int32_t>());
    }
    if (doc.containsKey("color")) {
        setColor(doc["color"].as<String>());
    } else {
        setColor(defaultColor);
    }
    if (doc.containsKey("background")) {
        setBackgroundColor(doc["background"].as<String>());
    } else {
        setBackgroundColor(defaultBackground);
    }
}

void WebDataElementTextModel::draw(TFT_eSPI& display) {
    display.setTextColor(getColor(), getBackgroundColor());
    display.drawString(getText(), getX(), getY(), getFont());
}
