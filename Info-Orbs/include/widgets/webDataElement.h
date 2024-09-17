#ifndef WEB_DATA_ELEMENT_H
#define WEB_DATA_ELEMENT_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "utils.h"

class WebDataElement {
   public:
    virtual bool isChanged();
    virtual void setChangedStatus(bool changed);

    virtual void parseData(JsonObject doc);
    virtual void draw(TFT_eSPI& display, int32_t defaultColor, int32_t defaultBackground);

   protected:
    bool m_changed = false;
};
#endif