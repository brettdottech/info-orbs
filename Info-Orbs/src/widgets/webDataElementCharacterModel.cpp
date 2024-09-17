#include "widgets/webDataElementCharacterModel.h"

int32_t WebDataElementCharacterModel::getX() {
    return m_x;
}

void WebDataElementCharacterModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementCharacterModel::getY() {
    return m_y;
}

void WebDataElementCharacterModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

String WebDataElementCharacterModel::getCharacter() {
    return m_character;
}

void WebDataElementCharacterModel::setCharacter(String character) {
    if (m_character != character) {
        m_character = character[0];
        m_changed = true;
    }
}

int32_t WebDataElementCharacterModel::getFont() {
    return m_font;
}

void WebDataElementCharacterModel::setFont(int32_t font) {
    if (m_font != font) {
        m_font = font;
        m_changed = true;
    }
}

int32_t WebDataElementCharacterModel::getBackgroundColor() {
    return m_background;
}

void WebDataElementCharacterModel::setBackgroundColor(int32_t background) {
    if (m_background != background) {
        m_background = background;
        m_changed = true;
    }
}

void WebDataElementCharacterModel::setBackgroundColor(String background) {
    setBackgroundColor(Utils::stringToColor(background));
}

void WebDataElementCharacterModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementCharacterModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementCharacterModel::getColor() {
    return m_color;
}

void WebDataElementCharacterModel::parseData(JsonObject doc) {
    Serial.println("Parsing Data");

    if (doc.containsKey("x")) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc.containsKey("y")) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc.containsKey("character")) {
        setCharacter(doc["character"].as<String>());
    }
    if (doc.containsKey("font")) {
        setFont(doc["font"].as<int32_t>());
    }
    if (doc.containsKey("color")) {
        setColor(doc["color"].as<String>());
    }
    if (doc.containsKey("background")) {
        setBackgroundColor(doc["background"].as<String>());
    }
}

void WebDataElementCharacterModel::draw(TFT_eSPI& display, int32_t defaultColor, int32_t defaultBackground) {
    int32_t color = getColor();
    if (color < 0) {
        color = defaultColor;
    }
    int32_t background = getBackgroundColor();
    if (background < 0) {
        background = defaultBackground;
    }
    display.setTextColor(color, background);
    display.drawChar(getCharacter()[0], getX(), getY(), getFont());
}
