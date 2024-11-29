#ifndef WEB_DATA_WIDGET_H
#define WEB_DATA_WIDGET_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "Widget.h"

#include "WebDataModel.h"
#include "Utils.h"

class WebDataWidget : public Widget {
   public:
    WebDataWidget(ScreenManager &manager, String url);
    ~WebDataWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

   private:
    unsigned long m_lastUpdate = 0;
    unsigned long m_updateDelay = 1000;
    String httpRequestAddress;
    WebDataModel m_obj[5];
    int32_t m_defaultColor = TFT_WHITE;
    int32_t m_defaultBackground = TFT_BLACK;
};
#endif  // WEB_DATA_WIDGET_H
