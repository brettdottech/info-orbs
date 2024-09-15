#ifndef WEB_DATA_ELEMENT_MODEL_H
#define WEB_DATA_ELEMENT_MODEL_H

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

    int32_t getX();
    void setX(int32_t x);
    int32_t getY();
    void setY(int32_t y);
    int32_t getX2();
    void setX2(int32_t x);
    int32_t getY2();
    void setY2(int32_t y);
    int32_t getX3();
    void setX3(int32_t x);
    int32_t getY3();
    void setY3(int32_t y);

    int32_t getWidth();
    void setWidth(int32_t width);
    int32_t getHeight();
    void setHeight(int32_t height);

    int32_t getRadius();
    void setRadius(int32_t radius);

    int32_t getBorderWidth();
    void setBorderWidth(int32_t borderWidth);

    uint32_t getArcAngle1();
    void setArcAngle1(uint32_t arcAngle1);
    uint32_t getArcAngle2();
    void setArcAngle2(uint32_t arcAngle2);


    String getText();
    void setText(String text);

    int32_t getFont();
    void setFont(int32_t font);

    bool getFilled();
    void setFilled(bool filled);

    int32_t getBackgroundColor();
    void setBackgroundColor(int32_t background);
    void setBackgroundColor(String background);

    void setColor(int32_t color);
    void setColor(String color);
    int32_t getColor();

    String getImageUrl();
    void setImageUrl(String imageUrl);
    String getImageBytes();
    void setImageBytes(String imageBytes);

    bool isChanged();
    void setChangedStatus(bool changed);

    void parseData(JsonObject doc);

   private:
    String m_type = "other";
    WebDataElementModelTypes m_typeEnum = OTHER;
    int32_t m_x = 0;
    int32_t m_y = 0;
    int32_t m_x2 = 0;
    int32_t m_y2 = 0;
    int32_t m_x3 = 0;
    int32_t m_y3 = 0;
    int32_t m_width = 0;
    int32_t m_height = 0;
    int32_t m_radius = 0;
    int32_t m_borderWidth;
    uint32_t m_arcAngle1;
    uint32_t m_arcAngle2;
    String m_text;
    int32_t m_font;
    bool m_filled;
    int32_t m_color;
    int32_t m_background;
    String m_imageUrl;
    String m_imageBytes;

    String m_fingerprint;
    bool m_changed = false;
};
#endif