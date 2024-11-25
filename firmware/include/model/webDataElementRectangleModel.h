#ifndef WEB_DATA_ELEMENT_RECTANGLE_MODEL_H
#define WEB_DATA_ELEMENT_RECTANGLE_MODEL_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#include "utils.h"
#include "webDataElement.h"

class WebDataElementRectangleModel : public WebDataElement {
   public:
    int32_t getX();
    void setX(int32_t x);
    int32_t getY();
    void setY(int32_t y);

    int32_t getWidth();
    void setWidth(int32_t width);
    int32_t getHeight();
    void setHeight(int32_t height);

    bool getFilled();
    void setFilled(bool filled);

    void setColor(int32_t color);
    void setColor(String color);
    int32_t getColor();

    void parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) override;
    void draw(ScreenManager& manager) override;

   private:
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
    bool m_filled = false;
    int32_t m_color = -1;
};
#endif
