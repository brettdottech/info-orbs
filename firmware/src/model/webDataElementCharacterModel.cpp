#include "model/webDataElementCharacterModel.h"

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

int32_t WebDataElementCharacterModel::getSize() {
    return m_size;
}
void WebDataElementCharacterModel::setSize(int32_t size) {
    if (m_size != size) {
        m_size = size;
        m_changed = true;
    }
}
int32_t WebDataElementCharacterModel::getAlignment() {
    return m_alignment;
}
void WebDataElementCharacterModel::setAlignment(int32_t alignment) {
    if (m_alignment != alignment) {
        m_alignment = alignment;
        m_changed = true;
    }
}

void WebDataElementCharacterModel::setAlignment(String alignment) {
    int32_t newAlignment = Utils::stringToAlignment(alignment);
    if (m_alignment != newAlignment) {
        m_alignment = newAlignment;
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

void WebDataElementCharacterModel::parseData(const JsonObject &doc, int32_t defaultColor, int32_t defaultBackground) {
    if (doc["x"].is<int32_t>()) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc["y"].is<int32_t>()) {
        setY(doc["y"].as<int32_t>());
    }
    if (const char *character = doc["character"]) {
        setCharacter(character);
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

void WebDataElementCharacterModel::draw(ScreenManager& manager) {
    manager.setLegacyTextDatum(getAlignment());
    manager.setLegacyTextSize(getSize());
    manager.setLegacyTextColor(getColor(), getBackgroundColor());
    manager.drawLegacyChar(getCharacter()[0], getX(), getY(), getFont());
}
