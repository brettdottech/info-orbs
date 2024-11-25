#ifndef WEB_DATA_MODEL_H
#define WEB_DATA_MODEL_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#include "webDataElementModel.h"

class WebDataModel {
   public:
    virtual ~WebDataModel() = default;
    String getLabel();
    void setLabel(String label);
    String getData();
    void setData(String data, int32_t defaultColor, int32_t defaultBackground);
    void setData(JsonArray data, int32_t defaultColor, int32_t defaultBackground);
    const WebDataElementModel& getElement(int index);
    int32_t getElementsCount();
    void setElementsCount(int32_t elementsCount);
    void initElements(int32_t count);
    // void setElements(WebDataElementModel *element);
    int32_t getLabelColor();
    void setLabelColor(int32_t color);
    void setLabelColor(String color);
    int32_t getDataColor();
    void setDataColor(int32_t color);
    void setDataColor(String color);
    int32_t getBackgroundColor();
    void setBackgroundColor(int32_t background);
    void setBackgroundColor(String background);

    bool isFullDraw();
    void setFullDrawStatus(bool fullDraw);

    bool isChanged();
    void setChangedStatus(bool changed);

    bool isInitialized();
    void setInitializedStatus(bool initialized);

    void parseData(const JsonObject& doc, int32_t defaultColor, int32_t defaultBackground);
    void draw(ScreenManager& manager);

   private:
    bool m_isInitialized = false;
    String m_label = "";
    String m_data = "";
    WebDataElementModel* m_elements = nullptr;
    int m_elementsCount = 0;
    int32_t m_labelColor = -1;
    int32_t m_color = -1;
    int32_t m_background = -1;
    bool m_fullDraw = false;
    bool m_changed = false;
};
#endif
