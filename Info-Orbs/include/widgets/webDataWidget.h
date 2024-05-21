#ifndef WEB_DATA_WIDGET_H
#define WEB_DATA_WIDGET_H

#include <widget.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <config.h>
#include <widget.h>

class WebDataWidget : public Widget {
public:
    WebDataWidget(ScreenManager &manager);
    ~WebDataWidget() override;
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;

private:
    String getFingerprint(int8_t display);
    int32_t stringToColor(String color);

    int m_lastUpdate = 0;
    int m_updateDelay = 1000;
    String httpRequestAddress = "http://192.168.10.242/temp.php";
    String m_label[5];
    String m_data[5];
    int32_t m_labelColor[5];
    int32_t m_color[5];
    int32_t m_background[5];
    String m_fingerprint[5];
    int32_t m_defaultColor = TFT_WHITE;
    int32_t m_defaultBackground = TFT_BLACK;
};
#endif // WEB_DATA_WIDGET_H