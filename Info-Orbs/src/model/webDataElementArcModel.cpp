#include "model/webDataElementArcModel.h"

int32_t WebDataElementArcModel::getX() {
    return m_x;
}

void WebDataElementArcModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementArcModel::getY() {
    return m_y;
}

void WebDataElementArcModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

int32_t WebDataElementArcModel::getRadius() {
    return m_radius;
}

void WebDataElementArcModel::setRadius(int32_t radius) {
    if (m_radius != radius) {
        m_radius = radius;
        m_changed = true;
    }
}

int32_t WebDataElementArcModel::getInnerRadius() {
    return m_innerRadius;
}

void WebDataElementArcModel::setInnerRadius(int32_t radius) {
    if (m_innerRadius != radius) {
        m_innerRadius = radius;
        m_changed = true;
    }
}

uint32_t WebDataElementArcModel::getAngleStart() {
    return m_angleStart;
}

void WebDataElementArcModel::setAngleStart(uint32_t angle) {
    if (m_angleStart != angle) {
        m_angleStart = angle;
        m_changed = true;
    }
}

uint32_t WebDataElementArcModel::getAngleEnd() {
    return m_angleEnd;
}

void WebDataElementArcModel::setAngleEnd(uint32_t angle) {
    if (m_angleEnd != angle) {
        m_angleEnd = angle;
        m_changed = true;
    }
}

int32_t WebDataElementArcModel::getBackgroundColor() {
    return m_background;
}

void WebDataElementArcModel::setBackgroundColor(int32_t background) {
    if (m_background != background) {
        m_background = background;
        m_changed = true;
    }
}

void WebDataElementArcModel::setBackgroundColor(String background) {
    setBackgroundColor(Utils::stringToColor(background));
}

void WebDataElementArcModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementArcModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementArcModel::getColor() {
    return m_color;
}

void WebDataElementArcModel::parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) {
    // Serial.println("Parsing Data");

    if (doc.containsKey("x")) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc.containsKey("y")) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc.containsKey("radius")) {
        setRadius(doc["radius"].as<int32_t>());
    }
    if (doc.containsKey("innerRadius")) {
        setInnerRadius(doc["innerRadius"].as<int32_t>());
    }
    if (doc.containsKey("angleStart")) {
        setAngleStart(doc["angleStart"].as<int32_t>());
    }
    if (doc.containsKey("angleEnd")) {
        setAngleEnd(doc["angleEnd"].as<int32_t>());
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

void WebDataElementArcModel::draw(TFT_eSPI& display) {
    display.drawArc(getX(), getY(), getRadius(), getInnerRadius(), getAngleStart(), getAngleEnd(), getColor(), getBackgroundColor(), true);
}
