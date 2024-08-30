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
    setLabelColor(Utils::stringToColor(color));
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
    setDataColor(Utils::stringToColor(color));
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
    setBackgroundColor(Utils::stringToColor(background));
}

bool WebDataModel::isChanged() {
    return m_changed;
}
void WebDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
}
