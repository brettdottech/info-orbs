#include "model/webDataElementImageModel.h"

int32_t WebDataElementImageModel::getX() {
    return m_x;
}

void WebDataElementImageModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementImageModel::getY() {
    return m_y;
}

void WebDataElementImageModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

String WebDataElementImageModel::getImage() {
    return m_image;
}

void WebDataElementImageModel::setImage(String image) {
    if (m_image != image) {
        m_image = image;
        m_changed = true;
    }
}

void WebDataElementImageModel::parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) {
    Serial.println("Parsing Data");

    if (doc.containsKey("x")) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc.containsKey("y")) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc.containsKey("image")) {
        setImage(doc["image"].as<String>());
    }
    // if (doc.containsKey("imageUrl")) {
    //     setImageUrl(doc["imageUrl"].as<String>());
    // }
    // if (doc.containsKey("imageBytes")) {
    //     setImageBytes(doc["imageBytes"].as<String>());
    // }
}

void WebDataElementImageModel::draw(TFT_eSPI& display) {
    // TODO implement displaying an image
    //  display.drawImage(getImage(), getX(), getY());
}
