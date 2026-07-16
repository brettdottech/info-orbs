#include "BaseballWidget.h"
#include "BaseballTranslations.h"
#include "TaskFactory.h"
#include <ArduinoLog.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

BaseballWidget::BaseballWidget(ScreenManager &manager, ConfigManager &config)
    : Widget(manager, config),
      m_drawTimer(addDrawRefreshFrequency(BASEBALL_DRAW_DELAY)),
      m_updateTimer(addUpdateRefreshFrequency(BASEBALL_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_BASEBALL == WIDGET_ON);

    m_config.addConfigBool("BaseballWidget", "baseballEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigString("BaseballWidget", "teamName", &m_teamName, 20, t_baseballShortName);

    Log.infoln("BaseballWidget initialized for team: %s", m_teamName.c_str());
}

void BaseballWidget::setup() {
    m_prevMillisSwitch = millis();
}

void BaseballWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);

    if (!m_teamData.isInitialized() && force) {
        m_manager.fillAllScreens(TFT_BLACK);
        m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
        for (int8_t i = m_page * NUM_SCREENS; i < (m_page + 1) * NUM_SCREENS; i++) {
            int8_t displayIndex = i % NUM_SCREENS;
            m_manager.selectScreen(displayIndex);
            m_manager.drawCentreString(I18n::get(t_loadingData), ScreenCenterX, ScreenCenterY, 20);
        }
        return;
    }

    if ((m_teamData.isChanged() || force) && m_teamData.isInitialized()) {
        // Get team colors once
        uint16_t primaryColor = !m_teamData.getColors().empty()
                                    ? m_manager.color565FromHex(m_teamData.getColors()[0].code)
                                    : TFT_WHITE;
        uint16_t secondaryColor = m_teamData.getColors().size() > 1
                                      ? m_manager.color565FromHex(m_teamData.getColors()[1].code)
                                      : TFT_BLACK;

        // Draw ALL screens
        drawTeamInfoScreen(primaryColor, secondaryColor);
        drawLastGameScreen(primaryColor, secondaryColor);
        drawTeamLogoScreen(primaryColor);
        drawNextGameScreen(primaryColor, secondaryColor);
        drawStandingsScreen(primaryColor, secondaryColor);

        m_teamData.setChangedStatus(false);
    }
}

void BaseballWidget::update(bool force) {
    String teamNameStr = String(m_teamName.c_str());
    String url = String(BASEBALL_API_URL) + "?teamName=" + teamNameStr + "&force=" + (force ? "true" : "false");
    auto task = TaskFactory::createHttpGetTask(url, [this](int httpCode, const String &response) {
        processResponse(m_teamData, httpCode, response);

        if (httpCode == HTTP_CODE_OK) {
            String logoUrl = getLogoUrl();

            auto logoTask = TaskFactory::createHttpGetTask(logoUrl, [this](int httpCode, const String &response) {
                if (httpCode == HTTP_CODE_OK && response.length() > 0) {
                    m_logoSize = response.length();
                    m_logoData.reset(new uint8_t[m_logoSize]);
                    if (m_logoData) {
                        memcpy(m_logoData.get(), response.c_str(), m_logoSize);
                        m_hasLogo = true;
                        m_teamData.setChangedStatus(true);
                    } else {
                        Log.errorln("Failed to allocate memory for logo");
                        m_logoSize = 0;
                    }
                }
            });

            TaskManager::getInstance()->addTask(std::move(logoTask));
        }
    });

    TaskManager::getInstance()->addTask(std::move(task));
}

void BaseballWidget::processResponse(BaseballDataModel &team, int httpCode, const String &response) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            team.setTeamId(doc["teamId"].as<int>());
            team.setSeason(doc["season"].as<String>());

            JsonObject teamObj = doc["team"];
            team.setFullName(teamObj["fullName"].as<String>());
            team.setShortName(teamObj["shortName"].as<String>());

            std::vector<BaseballDataModel::TeamColor> colors;
            for (JsonObject color : teamObj["colors"].as<JsonArray>()) {
                BaseballDataModel::TeamColor teamColor;
                teamColor.name = color["name"].as<String>();
                teamColor.code = color["code"].as<String>();
                colors.push_back(teamColor);
            }
            team.setColors(colors);
            team.setLogoUrl(teamObj["logoUrl"].as<String>());
            team.setLogoImageFileName(teamObj["logoImageFileName"].as<String>());
            team.setLogoBackgroundColor(teamObj["logoBackgroundColor"].as<String>());

            team.setRecord(doc["record"].as<String>());

            JsonObject standings = doc["standings"];
            team.setDivision(standings["division"].as<String>());
            team.setDivisionRank(standings["divisionRank"].as<String>());
            team.setWinningPercentage(standings["winningPercentage"].as<String>());
            team.setGamesBack(standings["gamesBack"].as<String>());

            JsonObject lastGame = doc["lastGame"];
            team.setLastGameDate(lastGame["date"].as<String>());
            team.setLastGameDay(lastGame["day"].as<String>());
            team.setLastGameOpponent(lastGame["opponent"].as<String>());
            team.setLastGameScore(lastGame["score"].as<String>());
            team.setLastGameResult(lastGame["result"].as<String>());
            team.setLastGameTime(lastGame["gameTime"].as<String>());

            JsonObject lastTen = doc["lastTen"];
            team.setLastTen(lastTen["record"].as<String>());

            JsonObject nextGame = doc["nextGame"];
            team.setNextGameDate(nextGame["date"].as<String>());
            team.setNextGameDay(nextGame["day"].as<String>());
            team.setNextGameOpponent(nextGame["opponent"].as<String>());
            team.setNextGameLocation(nextGame["location"].as<String>());
            team.setNextGameProbablePitcher(nextGame["probablePitcher"].as<String>());
            team.setNextGameTime(nextGame["gameTime"].as<String>());
            team.setNextGameTvBroadcast(nextGame["tvBroadcast"].as<String>());

            team.setInitializationStatus(true);
            team.setChangedStatus(true);
        } else {
            Log.errorln("deserializeJson() failed");
        }
    } else {
        Log.errorln("HTTP request failed, error: %d", httpCode);
    }
}

void BaseballWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        nextPage();
    }
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        update(true);
    }
}
void BaseballWidget::nextPage() {
    // future - to implement navigation between screen sets later:
    // m_currentScreenSet = (m_currentScreenSet + 1) % TOTAL_SCREEN_SETS;
    // For now, just force redraw all screens:
    m_teamData.setChangedStatus(true);
    draw(true);
}

String BaseballWidget::getName() {
    return "Baseball";
}

void BaseballWidget::drawTeamInfoScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Team Info Screen");
    m_manager.selectScreen(0);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "", "",
                            TFT_WHITE, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, FOOTER_TEXT_SIZE);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString(m_teamData.getShortName(), ScreenCenterX, contentCenterY - 30, 24);
    m_manager.drawCentreString("Record: " + m_teamData.getRecord(), ScreenCenterX, contentCenterY + 10, 24);
    m_manager.drawCentreString("Last 10: " + m_teamData.getLastTen(), ScreenCenterX, contentCenterY + 65, 13);
}

void BaseballWidget::drawLastGameScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Last Game Screen");
    m_manager.selectScreen(1);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "Last Game", m_teamData.getLastGameTime(),
                            TFT_WHITE, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, FOOTER_TEXT_SIZE);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString(m_teamData.getLastGameDay() + ", " + m_teamData.getLastGameDate(),
                               ScreenCenterX, contentCenterY - 40, 20);
    m_manager.drawCentreString("vs " + m_teamData.getLastGameOpponent(),
                               ScreenCenterX, contentCenterY - 10, 22);
    m_manager.drawCentreString(m_teamData.getLastGameScore(),
                               ScreenCenterX, contentCenterY + 25, 28);
    m_manager.drawCentreString(m_teamData.getLastGameResult(),
                               ScreenCenterX, contentCenterY + 60, 20);
}

void BaseballWidget::drawTeamLogoScreen(uint16_t primaryColor) {
    Log.traceln("Drawing Logo Screen");
    m_manager.selectScreen(2);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int center = 120;

    if (String(m_teamData.getLogoBackgroundColor()) == "white") {
        m_manager.fillScreen(TFT_WHITE);
    } else
        m_manager.fillScreen(TFT_BLACK);

    if (m_hasLogo && m_logoData && m_logoSize > 0) {
        int logoSize = min(contentHeight, SCREEN_SIZE) - 20;
        int logoX = ((SCREEN_SIZE - logoSize) / 2) + 6;
        int logoY = (contentTop + (contentHeight - logoSize) / 2);
        m_manager.drawJpg(logoX, logoY, m_logoData.get(), m_logoSize);
    }

    m_manager.drawArc(center, center, 120, 117, 0, 360, TFT_WHITE, TFT_WHITE);
    m_manager.drawArc(center, center, 116, 106, 0, 360, primaryColor, primaryColor);
}

void BaseballWidget::drawNextGameScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Next Game Screen");
    m_manager.selectScreen(3);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "Next Game", m_teamData.getNextGameTime(),
                            TFT_WHITE, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, FOOTER_TEXT_SIZE);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString(m_teamData.getNextGameDay() + ", " + m_teamData.getNextGameDate(),
                               ScreenCenterX, contentCenterY - 40, 20);
    m_manager.drawCentreString("vs " + m_teamData.getNextGameOpponent(),
                               ScreenCenterX, contentCenterY - 10, 24);
    m_manager.drawCentreString(m_teamData.getNextGameLocation(),
                               ScreenCenterX, contentCenterY + 20, 20);
    m_manager.drawCentreString("Pitcher: " + m_teamData.getNextGameProbablePitcher(),
                               ScreenCenterX, contentCenterY + 50, 19);
}

void BaseballWidget::drawStandingsScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Standings Screen");
    m_manager.selectScreen(4);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "Standings", "Updated: Today",
                            TFT_WHITE, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, 15);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString(m_teamData.getDivision(), ScreenCenterX, contentCenterY - 40, 20);
    m_manager.drawCentreString("Rank: " + m_teamData.getDivisionRank(), ScreenCenterX, contentCenterY - 10, 24);
    m_manager.drawCentreString("Win %: " + m_teamData.getWinningPercentage(), ScreenCenterX, contentCenterY + 20, 24);
    m_manager.drawCentreString("GB: " + m_teamData.getGamesBack(), ScreenCenterX, contentCenterY + 50, 24);
}
