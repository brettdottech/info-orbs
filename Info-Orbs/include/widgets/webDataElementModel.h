#ifndef WEB_DATA_ELEMENT_MODEL_H
#define WEB_DATA_ELEMENT_MODEL_H

#include "webDataElement.h"
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "utils.h"

enum WebDataElementModelTypes {
    TEXT,
    CHARACTER,
    LINE,
    RECTANGLE,
    TRIANGLE,
    CIRCLE,
    ARC,
    IMAGE,
    OTHER
};

class WebDataElementModel {
   public:
    String getType();
    void setType(String type);
    WebDataElementModelTypes getTypeEnum();

    bool isChanged();
    void setChangedStatus(bool changed);

    void parseData(JsonObject doc);
    void draw(TFT_eSPI& display, int32_t defaultColor, int32_t defaultBackground);

   private:
    String m_type = "other";
    WebDataElementModelTypes m_typeEnum = OTHER;
    WebDataElement *m_element;

    bool m_changed = false;
};
#endif