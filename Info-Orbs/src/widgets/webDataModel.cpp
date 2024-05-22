#include "widgets/webDataModel.h"

String WebDataModel::getLabel() {
    return m_label;
}

void WebDataModel::setLabel(String label) {
    if (m_label != label) {
        m_label = label;
        m_changed = true;
    }
}

String WebDataModel::getData() {
    return m_data;
}

void WebDataModel::setData(String data) {
    if (m_data != data) {
        m_data = data;
        m_changed = true;
    }
}

int32_t WebDataModel::getLabelColor() {
    return m_labelColor;
}

void WebDataModel::setLabelColor(int32_t color) {
    if (m_labelColor != color) {
        m_labelColor = color;
        m_changed = true;
    }
}

void WebDataModel::setLabelColor(String color) {
    setLabelColor(stringToColor(color));
}

int32_t WebDataModel::getDataColor() {
    return m_color;
}

void WebDataModel::setDataColor(int32_t color) {
    if (m_color != color) {
        m_color = color;
        m_changed = true;
    }
}

void WebDataModel::setDataColor(String color) {
    setDataColor(stringToColor(color));
}

int32_t WebDataModel::getBackgroundColor() {
    return m_background;
}

void WebDataModel::setBackgroundColor(int32_t background) {
    if (m_background != background) {
        m_background = background;
        m_changed = true;
    }
}

void WebDataModel::setBackgroundColor(String background) {
    setBackgroundColor(stringToColor(background));
}

bool WebDataModel::isChanged() {
    return m_changed;
}
void WebDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
}

int32_t WebDataModel::stringToColor(String color) {
    color.toLowerCase();
    color.replace(" ", "");
    if (color == "black") {
        return TFT_BLACK;
    } else if (color == "navy") {
        return TFT_NAVY;
    } else if (color == "darkgreen") {
        return TFT_DARKGREEN;
    } else if (color == "darkcyan") {
        return TFT_DARKCYAN;
    } else if (color == "maroon") {
        return TFT_MAROON;
    } else if (color == "purple") {
        return TFT_PURPLE;
    } else if (color == "olive") {
        return TFT_OLIVE;
    } else if (color == "lightgrey") {
        return TFT_LIGHTGREY;
    } else if (color == "darkgrey") {
        return TFT_DARKGREY;
    } else if (color == "blue") {
        return TFT_BLUE;
    } else if (color == "green") {
        return TFT_GREEN;
    } else if (color == "cyan") {
        return TFT_CYAN;
    } else if (color == "red") {
        return TFT_RED;
    } else if (color == "magenta") {
        return TFT_MAGENTA;
    } else if (color == "yellow") {
        return TFT_YELLOW;
    } else if (color == "white") {
        return TFT_WHITE;
    } else if (color == "orange") {
        return TFT_ORANGE;
    } else if (color == "greenyellow") {
        return TFT_GREENYELLOW;
    } else if (color == "pink") {
        return TFT_PINK;
    } else if (color == "brown") {
        return TFT_BROWN;
    } else if (color == "gold") {
        return TFT_GOLD;
    } else if (color == "silver") {
        return TFT_SILVER;
    } else if (color == "skyblue") {
        return TFT_SKYBLUE;
    } else if (color == "vilolet") {
        return TFT_VIOLET;
    } else {
        Serial.print("Invalid color: ");
        Serial.println(color);
        return TFT_BLACK;
    }
}
