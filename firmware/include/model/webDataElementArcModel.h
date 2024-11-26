#ifndef WEB_DATA_ELEMENT_Arc_MODEL_H
#define WEB_DATA_ELEMENT_Arc_MODEL_H

#include "webDataElement.h"
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "utils.h"

class WebDataElementArcModel: public WebDataElement {
   public:
    int32_t getX();
    void setX(int32_t x);
    int32_t getY();
    void setY(int32_t y);

    int32_t getRadius();
    void setRadius(int32_t radius);
    int32_t getInnerRadius();
    void setInnerRadius(int32_t radius);

    uint32_t getAngleStart();
    void setAngleStart(uint32_t angle);

    uint32_t getAngleEnd();
    void setAngleEnd(uint32_t angle);

    void setColor(int32_t color);
    void setColor(String color);
    int32_t getColor();

    void setBackgroundColor(int32_t background);
    void setBackgroundColor(String background);
    int32_t getBackgroundColor();

    void parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) override;
    void draw(ScreenManager& manager) override;

   private:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_radius = 0;
    int32_t m_innerRadius = 0;
    uint32_t m_angleStart = 0;
    uint32_t m_angleEnd = 0;
    int32_t m_color = -1;
    int32_t m_background = -1;
};
#endif
