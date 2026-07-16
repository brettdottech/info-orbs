#ifndef NHLWIDGET_H
#define NHLWIDGET_H

#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <TaskManager.h>
#include <map>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "NHLDataModel.h"
#include "Widget.h"
#include "config_helper.h"

#ifndef NHL_API_BASE_URL
    #define NHL_API_BASE_URL "https://nhl.taylorshome.cc/nhl"
#endif

#ifndef NHL_TEAM_ABBREV
    #define NHL_TEAM_ABBREV "UTA"
#endif

class NHLWidget : public Widget {
public:
    NHLWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    // HTTP response handlers
    void onTeamDataResponse(int httpCode, const String &response);
    void onLogoResponse(int httpCode, const String &response);
    
    // Load logo from filesystem
    void loadLogoFromFilesystem();
    
    // Process combined team data (standings + schedule)
    void processCombinedTeamData(NHLDataModel &team, int httpCode, const String &response);
    void nextPage();
    void switchToNextTeam();
    
    // Helper functions for date/time formatting
    String formatDate(const String &isoDate);
    String getDayOfWeek(const String &isoDate);
    String formatTime(const String &isoTime);

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
        return const_cast<NHLDataModel &>(m_teamData).getLogoUrl();
    }

    std::string m_teamAbbrev = NHL_TEAM_ABBREV;
    
    // Team switching support - configurable list of teams to cycle through
    std::string m_teamListConfig = "UTA,COL,VGK";  // Default: Utah, Colorado, Vegas
    std::vector<std::string> m_teamList;
    int m_currentTeamIndex = 0;
    
    void parseTeamList();
    void preloadAllTeams();
    void preloadTeam(const std::string& teamAbbrev);
    void preloadTeamSequential(size_t teamIndex);

    // Cache for team data - stores data for all teams in the list
    std::map<std::string, NHLDataModel> m_teamDataCache;
    std::map<std::string, std::unique_ptr<uint8_t[]>> m_logoDataCache;
    std::map<std::string, size_t> m_logoSizeCache;
    
    NHLDataModel m_teamData;

    int m_switchinterval = 10;
    unsigned long m_prevMillisSwitch = 0;

    WidgetTimer &m_drawTimer;
    WidgetTimer &m_updateTimer;

#ifndef NHL_UPDATE_DELAY
    #define NHL_UPDATE_DELAY TimeFrequency::FifteenMinutes
#endif

#ifndef NHL_DRAW_DELAY
    #define NHL_DRAW_DELAY TimeFrequency::FiveMinutes
#endif
};
#endif // NHLWIDGET_H
