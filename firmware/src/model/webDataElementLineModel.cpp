#include "model/webDataElementLineModel.h"

int32_t WebDataElementLineModel::getX() {
    return m_x;
}

void WebDataElementLineModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementLineModel::getY() {
    return m_y;
}

void WebDataElementLineModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

int32_t WebDataElementLineModel::getX2() {
    return m_x2;
}

void WebDataElementLineModel::setX2(int32_t x) {
    if (m_x2 != x) {
        m_x2 = x;
        m_changed = true;
    }
}

int32_t WebDataElementLineModel::getY2() {
    return m_y2;
}

void WebDataElementLineModel::setY2(int32_t y) {
    if (m_y2 != y) {
        m_y2 = y;
        m_changed = true;
    }
}

void WebDataElementLineModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementLineModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementLineModel::getColor() {
    return m_color;
}

void WebDataElementLineModel::parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) {
    if (doc["x"].is<int32_t>()) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc["y"].is<int32_t>()) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc["x2"].is<int32_t>()) {
        setX2(doc["x2"].as<int32_t>());
    }
    if (doc["y2"].is<int32_t>()) {
        setY2(doc["y2"].as<int32_t>());
    }
    if (const char* color = doc["color"]) {
        setColor(color);
    } else {
        setColor(defaultColor);
    }
}

void WebDataElementLineModel::draw(ScreenManager& manager) {
    manager.drawLine(getX(), getY(), getX2(), getY2(), getColor());
}
