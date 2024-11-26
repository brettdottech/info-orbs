#include "model/webDataElementModel.h"

#include "model/webDataElementArcModel.h"
#include "model/webDataElementCharacterModel.h"
#include "model/webDataElementCircleModel.h"
#include "model/webDataElementImageModel.h"
#include "model/webDataElementLineModel.h"
#include "model/webDataElementRectangleModel.h"
#include "model/webDataElementTextModel.h"
#include "model/webDataElementTriangleModel.h"

void WebDataElementModel::setType(const String& type) {
    if (type == "text") {
        m_type = TEXT;
    } else if (type == "rectangle") {
        m_type = RECTANGLE;
    } else if (type == "character") {
        m_type = CHARACTER;
    } else if (type == "triangle") {
        m_type = TRIANGLE;
    } else if (type == "circle") {
        m_type = CIRCLE;
    } else if (type == "arc") {
        m_type = ARC;
    } else if (type == "line") {
        m_type = LINE;
    } else if (type == "image") {
        m_type = IMAGE;
    } else {
        m_type = OTHER;
    }
}

WebDataElementModelTypes WebDataElementModel::getType() {
    return m_type;
}

bool WebDataElementModel::isChanged() {
    return m_changed;
}
void WebDataElementModel::setChangedStatus(bool changed) {
    m_changed = changed;
}

void WebDataElementModel::parseData(JsonObject doc, int32_t defaultColor, int32_t defaultBackground) {
    delete m_element;
    m_element = nullptr;

    if (const char* type = doc["type"]) {
        setType(type);
        switch (getType()) {
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
    if (m_element != nullptr) {
        m_element->parseData(doc, defaultColor, defaultBackground);
    }
}

void WebDataElementModel::draw(ScreenManager& manager) {
    if (m_element != nullptr && getType() != OTHER) {
        m_element->draw(manager);
    }
}
