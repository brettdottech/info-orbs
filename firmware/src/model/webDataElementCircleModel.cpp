#include "model/webDataElementCircleModel.h"

int32_t WebDataElementCircleModel::getX() {
    return m_x;
}

void WebDataElementCircleModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementCircleModel::getY() {
    return m_y;
}

void WebDataElementCircleModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

int32_t WebDataElementCircleModel::getRadius() {
    return m_radius;
}

void WebDataElementCircleModel::setRadius(int32_t radius) {
    if (m_radius != radius) {
        m_radius = radius;
        m_changed = true;
    }
}

bool WebDataElementCircleModel::getFilled() {
    return m_filled;
}

void WebDataElementCircleModel::setFilled(bool filled) {
    if (m_filled != filled) {
        m_filled = filled;
        m_changed = true;
    }
}
void WebDataElementCircleModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementCircleModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementCircleModel::getColor() {
    return m_color;
}

void WebDataElementCircleModel::parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) {
    if (doc["x"].is<int32_t>()) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc["y"].is<int32_t>()) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc["radius"].is<int32_t>()) {
        setRadius(doc["radius"].as<int32_t>());
    }
    if (doc["filled"].is<int32_t>()) {
        setFilled(doc["filled"].as<bool>());
    }
    if (const char* color = doc["color"]) {
        setColor(color);
    } else {
        setColor(defaultColor);
    }
}

void WebDataElementCircleModel::draw(ScreenManager& manager) {
    if (getFilled()) {
        manager.fillCircle(getX(), getY(), getRadius(), getColor());
    } else {
        manager.drawCircle(getX(), getY(), getRadius(), getColor());
    }
}
