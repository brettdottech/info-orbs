#include "model/webDataElementRectangleModel.h"

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

void WebDataElementRectangleModel::parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) {
    if (doc["x1"].is<int32_t>()) {
        setX(doc["x1"].as<int32_t>());
    } else if (doc["x"].is<int32_t>()) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc["y1"].is<int32_t>()) {
        setY(doc["y1"].as<int32_t>());
    } else if (doc["y"].is<int32_t>()) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc["x2"].is<int32_t>()) {
        setWidth(doc["x2"].as<int32_t>() - getX());
    }
    if (doc["y2"].is<int32_t>()) {
        setWidth(doc["y2"].as<int32_t>() - getX());
    }
    if (int32_t y2 = doc["y2"]) {
        setHeight(y2 - getY());
    }
    if (doc["height"].is<int32_t>()) {
        setHeight(doc["height"].as<int32_t>());
    }
    if (doc["width"].is<int32_t>()) {
        setWidth(doc["width"].as<int32_t>());
    }
    if (doc["filled"].is<bool>()) {
        setFilled(doc["filled"].as<bool>());
    }
    if (const char* color = doc["color"]) {
        setColor(color);
    } else {
        setColor(defaultColor);
    }
}

void WebDataElementRectangleModel::draw(ScreenManager& manager) {
    if (getFilled()) {
        manager.fillRect(getX(), getY(), getWidth(), getHeight(), getColor());
    } else {
        manager.drawRect(getX(), getY(), getWidth(), getHeight(), getColor());
    }
}
