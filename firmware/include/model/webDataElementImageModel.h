#ifndef WEB_DATA_ELEMENT_IMAGE_MODEL_H
#define WEB_DATA_ELEMENT_IMAGE_MODEL_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#include "utils.h"
#include "webDataElement.h"

class WebDataElementImageModel : public WebDataElement {
   public:
    int32_t getX();
    void setX(int32_t x);
    int32_t getY();
    void setY(int32_t y);

    void setImage(String image);
    String getImage();

    void parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground) override;
    void draw(ScreenManager& manager) override;

   private:
    int32_t m_x = 0;
    int32_t m_y = 0;
    String m_image = "";
};
#endif
