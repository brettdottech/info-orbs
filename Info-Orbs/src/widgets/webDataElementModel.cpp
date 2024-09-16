#include "widgets/webDataElementModel.h"

String WebDataElementModel::getType() {
    return m_type;
}

void WebDataElementModel::setType(String type) {
    if (m_type != type) {
        m_type = type;

        if (m_type == "text") {
            m_typeEnum = TEXT;
        } else if (m_type == "rectangle") {
            m_typeEnum = RECTANGLE;
        } else if (m_type == "triangle") {
            m_typeEnum = TRIANGLE;
        } else if (m_type == "circle") {
            m_typeEnum = CIRCLE;
        } else if (m_type == "arc") {
            m_typeEnum = ARC;
        } else if (m_type == "image") {
            m_typeEnum = IMAGE;
        } else {
            m_typeEnum = OTHER;
        }
        m_changed = true;
    }
}

WebDataElementModelTypes WebDataElementModel::getTypeEnum() {
    return m_typeEnum;
}

int32_t WebDataElementModel::getX() {
    return m_x;
}

void WebDataElementModel::setX(int32_t x) {
    if (m_x != x) {
        m_x = x;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getY() {
    return m_y;
}

void WebDataElementModel::setY(int32_t y) {
    if (m_y != y) {
        m_y = y;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getX2() {
    return m_x2;
}
void WebDataElementModel::setX2(int32_t x2) {
    if (m_x2 != x2) {
        m_x2 = x2;
        m_changed = true;
    }
}
int32_t WebDataElementModel::getY2() {
    return m_y2;
}
void WebDataElementModel::setY2(int32_t y2) {
    if (m_y2 != y2) {
        m_y2 = y2;
        m_changed = true;
    }
}
int32_t WebDataElementModel::getX3() {
    return m_x3;
}
void WebDataElementModel::setX3(int32_t x3) {
    if (m_x3 != x3) {
        m_x3 = x3;
        m_changed = true;
    }
}
int32_t WebDataElementModel::getY3() {
    return m_y3;
}
void WebDataElementModel::setY3(int32_t y3) {
    if (m_y3 != y3) {
        m_y3 = y3;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getWidth() {
    return m_width;
}

void WebDataElementModel::setWidth(int32_t width) {
    if (m_width != width) {
        m_width = width;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getHeight() {
    return m_height;
}

void WebDataElementModel::setHeight(int32_t height) {
    if (m_height != height) {
        m_height = height;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getRadius() {
    return m_radius;
}

void WebDataElementModel::setRadius(int32_t radius) {
    if (m_radius != radius) {
        m_radius = radius;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getBorderWidth() {
    return m_borderWidth;
}

void WebDataElementModel::setBorderWidth(int32_t borderWidth) {
    if (m_borderWidth != borderWidth) {
        m_borderWidth = borderWidth;
        m_changed = true;
    }
}

uint32_t WebDataElementModel::getArcAngle1() {
    return m_arcAngle1;
}

void WebDataElementModel::setArcAngle1(uint32_t arcAngle1) {
    if (m_arcAngle1 != arcAngle1) {
        m_arcAngle1 = arcAngle1;
        m_changed = true;
    }
}

uint32_t WebDataElementModel::getArcAngle2() {
    return m_arcAngle2;
}

void WebDataElementModel::setArcAngle2(uint32_t arcAngle2) {
    if (m_arcAngle2 != arcAngle2) {
        m_arcAngle2 = arcAngle2;
        m_changed = true;
    }
}

String WebDataElementModel::getText() {
    return m_text;
}

void WebDataElementModel::setText(String text) {
    if (m_text != text) {
        m_text = text;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getFont() {
    return m_font;
}

void WebDataElementModel::setFont(int32_t font) {
    if (m_font != font) {
        m_font = font;
        m_changed = true;
    }
}

bool WebDataElementModel::getFilled() {
    return m_filled;
}

void WebDataElementModel::setFilled(bool filled) {
    if (m_filled != filled) {
        m_filled = filled;
        m_changed = true;
    }
}

int32_t WebDataElementModel::getBackgroundColor() {
    return m_background;
}

void WebDataElementModel::setBackgroundColor(int32_t background) {
    if (m_background != background) {
        m_background = background;
        m_changed = true;
    }
}

void WebDataElementModel::setBackgroundColor(String background) {
    setBackgroundColor(Utils::stringToColor(background));
}

void WebDataElementModel::setColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataElementModel::setColor(String color) {
    setColor(Utils::stringToColor(color));
}

int32_t WebDataElementModel::getColor() {
    return m_color;
}

String WebDataElementModel::getImageUrl() {
    return m_imageUrl;
}
void WebDataElementModel::setImageUrl(String imageUrl) {
    if (m_imageUrl != imageUrl) {
        m_imageUrl = imageUrl;
        m_changed = true;
    }
}

String WebDataElementModel::getImageBytes() {
    return m_imageBytes;
}

void WebDataElementModel::setImageBytes(String imageBytes) {
    if (m_imageBytes != imageBytes) {
        m_imageBytes = imageBytes;
        m_changed = true;
    }
}

bool WebDataElementModel::isChanged() {
    return m_changed;
}
void WebDataElementModel::setChangedStatus(bool changed) {
    m_changed = changed;
}

void WebDataElementModel::parseData(JsonObject doc) {
    Serial.println("Parsing Data");

    if (doc.containsKey("type")) {
        setType(doc["type"].as<String>());
    }
    if (doc.containsKey("x")) {
        setX(doc["x"].as<int32_t>());
    }
    if (doc.containsKey("y")) {
        setY(doc["y"].as<int32_t>());
    }
    if (doc.containsKey("x2")) {
        setX2(doc["x2"].as<int32_t>());
    }
    if (doc.containsKey("y2")) {
        setY2(doc["y2"].as<int32_t>());
    }
    if (doc.containsKey("x3")) {
        setX3(doc["x3"].as<int32_t>());
    }
    if (doc.containsKey("y3")) {
        setY3(doc["y3"].as<int32_t>());
    }
    if (doc.containsKey("width")) {
        setWidth(doc["width"].as<int32_t>());
    }
    if (doc.containsKey("height")) {
        setHeight(doc["height"].as<int32_t>());
    }
    if (doc.containsKey("radius")) {
        setRadius(doc["radius"].as<int32_t>());
    }
    if (doc.containsKey("borderWidth")) {
        setBorderWidth(doc["borderWidth"].as<int32_t>());
    }
    if (doc.containsKey("arcAngle1")) {
        setArcAngle1(doc["arcAngle1"].as<int32_t>());
    }
    if (doc.containsKey("arcAngle2")) {
        setArcAngle2(doc["arcAngle2"].as<int32_t>());
    }
    if (doc.containsKey("text")) {
        setText(doc["text"].as<String>());
    }
    if (doc.containsKey("font")) {
        setFont(doc["font"].as<int32_t>());
    }
    if (doc.containsKey("filled")) {
        setFilled(doc["filled"].as<bool>());
    }
    if (doc.containsKey("background")) {
        setBackgroundColor(doc["background"].as<String>());
    }
    if (doc.containsKey("color")) {
        setColor(doc["color"].as<String>());
    }
    if (doc.containsKey("imageUrl")) {
        setImageUrl(doc["imageUrl"].as<String>());
    }
    if (doc.containsKey("imageBytes")) {
        setImageBytes(doc["imageBytes"].as<String>());
    }
}

void WebDataElementModel::draw(TFT_eSPI& display) {
    switch (getTypeEnum()) {
        case TEXT:
            display.setTextColor(getColor(), getBackgroundColor());
            display.drawString(getText(), getX(), getY(), getFont());
            break;
        case CHARACTER:
            display.setTextColor(getColor(), getBackgroundColor());
            display.drawChar(getX(), getY(), getText().c_str()[0], getColor(), getBackgroundColor(), getFont());
            break;
        case LINE:
            display.drawLine(getX(), getY(), getX2(), getY2(), getColor());
            break;
        case RECTANGLE:
            if (getFilled()) {
                display.fillRect(getX(), getY(), getWidth(), getHeight(), getColor());
            } else {
                display.drawRect(getX(), getY(), getWidth(), getHeight(), getColor());
            }
            break;
        case TRIANGLE:
            if (getFilled()) {
                display.fillTriangle(getX(), getY(), getX2(), getY2(), getX3(), getY3(), getColor());
            } else {
                display.drawTriangle(getX(), getY(), getX2(), getY2(), getX3(), getY3(), getColor());
            }
        case CIRCLE:
            if (getFilled()) {
                display.fillCircle(getX(), getY(), getRadius(), getColor());
            } else {
                display.drawCircle(getX(), getY(), getRadius(), getColor());
            }
            break;
        case ARC:
            display.drawArc(getX(), getY(), getRadius(), getWidth(), getArcAngle1(), getArcAngle2(), getColor(), getBackgroundColor(), true);
            break;
        case IMAGE:
            Serial.println("Image Drawing is TBD");
            // TBD
            // display.drawXBitmap(getX(), getY(), getImage(), getWidth(), getHeight(), getColor());
            break;
        case OTHER:
            Serial.println("WebDataWidget::draw: Other type not supported");
            break;
    }
}
