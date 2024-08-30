#ifndef WEB_DATA_MODEL_H
#define WEB_DATA_MODEL_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "utils.h"

class WebDataModel {
   public:
    String getLabel();
    void setLabel(String label);
    String getData();
    void setData(String data);
    int32_t getLabelColor();
    void setLabelColor(int32_t color);
    void setLabelColor(String color);
    int32_t getDataColor();
    void setDataColor(int32_t color);
    void setDataColor(String color);
    int32_t getBackgroundColor();
    void setBackgroundColor(int32_t background);
    void setBackgroundColor(String background);
    bool isChanged();
    void setChangedStatus(bool changed);
    
   private:
    String m_label;
    String m_data;
    int32_t m_labelColor;
    int32_t m_color;
    int32_t m_background;
    String m_fingerprint;
    bool m_changed = false;
};
#endif