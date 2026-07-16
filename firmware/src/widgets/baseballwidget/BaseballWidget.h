#ifndef BASEBALLWIDGET_H
#define BASEBALLWIDGET_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <TaskManager.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "BaseballDataModel.h"
#include "Widget.h"
#include "config_helper.h"

#ifndef BASEBALL_API_URL
    #define BASEBALL_API_URL "https://info-orbs-proxy.onrender.com/mlbdata/proxy"
#endif

#ifndef BASEBALL_TEAM_NAME
    #define BASEBALL_TEAM_NAME "Mets"
#endif

class BaseballWidget : public Widget {
public:
    BaseballWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    void processResponse(BaseballDataModel &team, int httpCode, const String &response);
    void nextPage();

    // Add these between existing private members
    void drawTeamInfoScreen(uint16_t primaryColor, uint16_t secondaryColor);
    void drawLastGameScreen(uint16_t primaryColor, uint16_t secondaryColor);
    void drawTeamLogoScreen(uint16_t primaryColor);
    void drawNextGameScreen(uint16_t primaryColor, uint16_t secondaryColor);
    void drawStandingsScreen(uint16_t primaryColor, uint16_t secondaryColor);

    // Common drawing constants
    static constexpr uint8_t TOP_BAR_HEIGHT = 35;
    static constexpr uint8_t BOTTOM_BAR_HEIGHT = 35;
    static constexpr uint8_t HEADER_TEXT_SIZE = 18;
    static constexpr uint8_t FOOTER_TEXT_SIZE = 18;

    static constexpr uint8_t SCREEN_COUNT = 5;
    int8_t m_page = 0;
    int8_t m_pageCount = 5;

    std::unique_ptr<uint8_t[]> m_logoData;
    size_t m_logoSize = 0;
    bool m_hasLogo = false;

    String getLogoUrl() const {
        String baseUrl(BASEBALL_API_URL);

        // Remove "/proxy" from the end (if present)
        if (baseUrl.endsWith("/proxy")) {
            baseUrl = baseUrl.substring(0, baseUrl.length() - 6); // Remove last 6 chars ("/proxy")
        }
        return baseUrl + "/logo/" + const_cast<BaseballDataModel &>(m_teamData).getLogoImageFileName();
    }

    std::string m_teamName = BASEBALL_TEAM_NAME;

    BaseballDataModel m_teamData;

    int m_switchinterval = 10;
    unsigned long m_prevMillisSwitch = 0;

    WidgetTimer &m_drawTimer;
    WidgetTimer &m_updateTimer;

#ifndef BASEBALL_UPDATE_DELAY
    #define BASEBALL_UPDATE_DELAY TimeFrequency::FifteenMinutes
#endif

#ifndef BASEBALL_DRAW_DELAY
    #define BASEBALL_DRAW_DELAY TimeFrequency::FiveMinutes
#endif
};
#endif // BASEBALLWIDGET_H
