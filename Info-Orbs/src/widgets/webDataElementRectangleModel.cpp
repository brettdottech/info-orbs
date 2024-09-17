#include "widgets/webDataElementRectangleModel.h"

int32_t WebDataElementRectangleModel::getX() {
    return m_x;
}

void WebDataElementRectangleModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementRectangleModel::getY() {
    return m_y;
}

void WebDataElementRectangleModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

int32_t WebDataElementRectangleModel::getWidth() {
    return m_width;
}

void WebDataElementRectangleModel::setWidth(int32_t width) {
    if (m_width != width) {
        m_width = width;
        m_changed = true;
    }
}

int32_t WebDataElementRectangleModel::getHeight() {
    return m_height;
}

void WebDataElementRectangleModel::setHeight(int32_t height) {
    if (m_height != height) {
        m_height = height;
        m_changed = true;
    }
}

bool WebDataElementRectangleModel::getFilled() {
    return m_filled;
}

void WebDataElementRectangleModel::setFilled(bool filled) {
    if (m_filled != filled) {
        m_filled = filled;
        m_changed = true;
    }
}

void WebDataElementRectangleModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementRectangleModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementRectangleModel::getColor() {
    return m_color;
}

void WebDataElementRectangleModel::parseData(JsonObject doc) {

    if (doc.containsKey("x")) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc.containsKey("y")) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc.containsKey("height")) {
        setHeight(doc["height"].as<int32_t>());
    }
    if (doc.containsKey("width")) {
        setWidth(doc["width"].as<int32_t>());
    }
    if (doc.containsKey("filled")) {
        setFilled(doc["filled"].as<bool>());
    }
    if (doc.containsKey("color")) {
        setColor(doc["color"].as<String>());
    }
}

void WebDataElementRectangleModel::draw(TFT_eSPI& display, int32_t defaultColor, int32_t defaultBackground) {
    int32_t color = getColor();
    if (color < 0) {
        color = defaultColor;
    }
    if (getFilled()) {
        display.fillRect(getX(), getY(), getWidth(), getHeight(), color);
    } else {
        display.drawRect(getX(), getY(), getWidth(), getHeight(), color);
    }
}
