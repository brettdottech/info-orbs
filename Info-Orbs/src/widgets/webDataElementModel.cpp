#include "widgets/webDataElementModel.h"

#include "widgets/webDataElementArcModel.h"
#include "widgets/webDataElementCharacterModel.h"
#include "widgets/webDataElementCircleModel.h"
#include "widgets/webDataElementImageModel.h"
#include "widgets/webDataElementLineModel.h"
#include "widgets/webDataElementRectangleModel.h"
#include "widgets/webDataElementTextModel.h"
#include "widgets/webDataElementTriangleModel.h"

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
        } else if (m_type == "character") {
            m_typeEnum = CHARACTER;
        } else if (m_type == "triangle") {
            m_typeEnum = TRIANGLE;
        } else if (m_type == "circle") {
            m_typeEnum = CIRCLE;
        } else if (m_type == "arc") {
            m_typeEnum = ARC;
        } else if (m_type == "line") {
            m_typeEnum = LINE;
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

bool WebDataElementModel::isChanged() {
    return m_changed;
}
void WebDataElementModel::setChangedStatus(bool changed) {
    m_changed = changed;
}

void WebDataElementModel::parseData(JsonObject doc) {
    Serial.println("Parsing Data");
    m_element = NULL;

    if (doc.containsKey("type")) {
        setType(doc["type"].as<String>());
        Serial.println("Type: " + getType());
        Serial.print("Type Enum: ");Serial.println(getTypeEnum());
        switch (getTypeEnum()) {
            case TEXT:
                m_element = new WebDataElementTextModel();
                break;
            case CHARACTER:
                m_element = new WebDataElementCharacterModel();
                break;
            case LINE:
                m_element = new WebDataElementLineModel();
                break;
            case RECTANGLE:
                m_element = new WebDataElementRectangleModel();
                break;
            case TRIANGLE:
                m_element = new WebDataElementTriangleModel();
                break;
            case CIRCLE:
                m_element = new WebDataElementCircleModel();
                break;
            case ARC:
                m_element = new WebDataElementArcModel();
                break;
            case IMAGE:
                m_element = new WebDataElementImageModel();
                break;
        }
    }
    if (m_element != NULL) {
        m_element->parseData(doc);
    }
}

void WebDataElementModel::draw(TFT_eSPI& display, int32_t defaultColor, int32_t defaultBackground) {
    if (m_element != NULL && getTypeEnum() != OTHER) {
        m_element->draw(display, defaultColor, defaultBackground);
    }
}