#ifndef WEB_DATA_ELEMENT_H
#define WEB_DATA_ELEMENT_H

#include <ArduinoJson.h>
#include <screenManager.h>

class WebDataElement {
   public:
    virtual ~WebDataElement() = default;

    bool isChanged();
    void setChangedStatus(bool changed);

    virtual void parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground);

    virtual void draw(ScreenManager& manager);

   protected:
    bool m_changed = false;
};
#endif
