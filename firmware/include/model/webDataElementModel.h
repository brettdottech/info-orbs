#ifndef WEB_DATA_ELEMENT_MODEL_H
#define WEB_DATA_ELEMENT_MODEL_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#include "utils.h"
#include "webDataElement.h"

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
    void setType(const String& type);
    WebDataElementModelTypes getType();

    bool isChanged();
    void setChangedStatus(bool changed);

    void parseData(JsonObject doc, int32_t defaultColor, int32_t defaultBackground);
    void draw(ScreenManager& manager);

   private:
    WebDataElementModelTypes m_type = OTHER;
    WebDataElement* m_element = nullptr;

    bool m_changed = false;
};
#endif
