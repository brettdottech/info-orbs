#ifndef WEB_DATA_WIDGET_H
#define WEB_DATA_WIDGET_H

#include <widget.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <widget.h>
#include "webDataModel.h"
#include "utils.h"

class WebDataWidget : public Widget {
public:
    WebDataWidget(ScreenManager &manager, String url);
    ~WebDataWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void changeMode() override;

private:
    int m_lastUpdate = 0;
    int m_updateDelay = 1000;
    String httpRequestAddress;
    WebDataModel m_obj[5];
    int32_t m_defaultColor = TFT_WHITE;
    int32_t m_defaultBackground = TFT_BLACK;
};
#endif // WEB_DATA_WIDGET_H
