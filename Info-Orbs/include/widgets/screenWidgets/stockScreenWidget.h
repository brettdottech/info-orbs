#include <screenWidget.h>
#include <screenManager.h>
#include <HTTPClient.h>

class StockScreenWidget : public ScreenWidget {
public:
    StockScreenWidget(ScreenManager &sm,const String& stock, int screenIndex);
    virtual void setup() override;
    virtual void draw() override;
    virtual void update() override;

private:
    String m_TrackingStock{};
    String httpRequestAddress{ "" };

    // HttpClient http;

};