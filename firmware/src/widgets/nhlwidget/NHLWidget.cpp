#include "NHLWidget.h"
#include "NHLTranslations.h"
#include "TaskFactory.h"
#include <ArduinoLog.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h>

NHLWidget::NHLWidget(ScreenManager &manager, ConfigManager &config)
    : Widget(manager, config),
      m_drawTimer(addDrawRefreshFrequency(NHL_DRAW_DELAY)),
      m_updateTimer(addUpdateRefreshFrequency(NHL_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_NHL == WIDGET_ON);

    m_config.addConfigBool("NHLWidget", "nhlEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigString("NHLWidget", "teamAbbrev", &m_teamAbbrev, 10, t_nhlTeamAbbrev);
    m_config.addConfigString("NHLWidget", "teamList", &m_teamListConfig, 100, "Team List (comma-separated)");

    // Parse the team list from config
    parseTeamList();

    // Find the current team in the team list
    for (size_t i = 0; i < m_teamList.size(); i++) {
        if (m_teamList[i] == m_teamAbbrev) {
            m_currentTeamIndex = i;
            break;
        }
    }

    Log.infoln("NHLWidget initialized for team: %s (index %d of %d teams)",
               m_teamAbbrev.c_str(), m_currentTeamIndex, m_teamList.size());
}

void NHLWidget::parseTeamList() {
    m_teamList.clear();
    
    String teamListStr = String(m_teamListConfig.c_str());
    teamListStr.trim();
    
    // If empty, use just the current team
    if (teamListStr.length() == 0) {
        m_teamList.push_back(m_teamAbbrev);
        Log.infoln("Team list empty, using current team only: %s", m_teamAbbrev.c_str());
        return;
    }
    
    // Parse comma-separated list
    int startIndex = 0;
    int commaIndex = teamListStr.indexOf(',');
    
    while (commaIndex >= 0) {
        String team = teamListStr.substring(startIndex, commaIndex);
        team.trim();
        team.toUpperCase();
        if (team.length() > 0) {
            m_teamList.push_back(team.c_str());
        }
        startIndex = commaIndex + 1;
        commaIndex = teamListStr.indexOf(',', startIndex);
    }
    
    // Add the last team
    String lastTeam = teamListStr.substring(startIndex);
    lastTeam.trim();
    lastTeam.toUpperCase();
    if (lastTeam.length() > 0) {
        m_teamList.push_back(lastTeam.c_str());
    }
    
    Log.infoln("Parsed %d teams from config: %s", m_teamList.size(), m_teamListConfig.c_str());
}

void NHLWidget::preloadAllTeams() {
    Log.infoln("Preloading data for %d teams sequentially...", m_teamList.size());
    
    // Start preloading the first team (if any)
    if (!m_teamList.empty()) {
        preloadTeamSequential(0);
    }
}

void NHLWidget::preloadTeamSequential(size_t teamIndex) {
    // Check if we've processed all teams
    if (teamIndex >= m_teamList.size()) {
        Log.infoln("✓ Preload complete for all %d teams", m_teamList.size());
        return;
    }
    
    const auto& teamAbbrev = m_teamList[teamIndex];
    
    // Skip if already cached
    if (m_teamDataCache.find(teamAbbrev) != m_teamDataCache.end()) {
        Log.traceln("Team %s already cached, skipping", teamAbbrev.c_str());
        // Move to next team immediately
        preloadTeamSequential(teamIndex + 1);
        return;
    }
    
    Log.infoln("Preloading team %d/%d: %s", teamIndex + 1, m_teamList.size(), teamAbbrev.c_str());
    
    String teamUrl = String(NHL_API_BASE_URL) + "/team/" + String(teamAbbrev.c_str());
    
    // Capture teamIndex by value to chain to next team
    auto teamTask = TaskFactory::createHttpGetTask(
        teamUrl,
        [this, teamAbbrev, teamIndex](int httpCode, const String &response) {
            if (httpCode == HTTP_CODE_OK) {
                NHLDataModel teamData;
                processCombinedTeamData(teamData, httpCode, response);
                m_teamDataCache[teamAbbrev] = teamData;
                Log.infoln("✓ Cached data for team: %s", teamAbbrev.c_str());
                
                // Also try to load logo from filesystem
                String logoPath = "/nhl/" + String(teamAbbrev.c_str()) + ".jpg";
                File logoFile = LittleFS.open(logoPath, "r");
                if (logoFile) {
                    size_t logoSize = logoFile.size();
                    if (logoSize > 1000 && logoSize < 50000) {
                        auto logoData = std::unique_ptr<uint8_t[]>(new uint8_t[logoSize]);
                        if (logoData) {
                            size_t bytesRead = logoFile.read(logoData.get(), logoSize);
                            if (bytesRead == logoSize) {
                                m_logoDataCache[teamAbbrev] = std::move(logoData);
                                m_logoSizeCache[teamAbbrev] = logoSize;
                                Log.infoln("✓ Cached logo for team: %s (%d bytes)", teamAbbrev.c_str(), logoSize);
                            }
                        }
                    }
                    logoFile.close();
                }
            } else {
                Log.warningln("Failed to preload team %s: HTTP %d", teamAbbrev.c_str(), httpCode);
            }
            
            // Chain to next team after this one completes
            preloadTeamSequential(teamIndex + 1);
        }
    );
    
    TaskManager::getInstance()->addTask(std::move(teamTask));
}

void NHLWidget::preloadTeam(const std::string& teamAbbrev) {
    // Legacy method - now just finds the team index and calls sequential version
    for (size_t i = 0; i < m_teamList.size(); i++) {
        if (m_teamList[i] == teamAbbrev) {
            preloadTeamSequential(i);
            return;
        }
    }
}

void NHLWidget::setup() {
    m_prevMillisSwitch = millis();
    
    // Check if we already have cached data for the current team
    auto cachedData = m_teamDataCache.find(m_teamAbbrev);
    if (cachedData != m_teamDataCache.end() && cachedData->second.isInitialized()) {
        // Use cached data - no need to fetch
        Log.infoln("NHLWidget setup() - using cached data for %s", m_teamAbbrev.c_str());
        m_teamData = cachedData->second;
        m_teamData.setChangedStatus(true);
        
        // Load cached logo if available
        auto cachedLogo = m_logoDataCache.find(m_teamAbbrev);
        auto cachedSize = m_logoSizeCache.find(m_teamAbbrev);
        if (cachedLogo != m_logoDataCache.end() && cachedSize != m_logoSizeCache.end()) {
            m_logoSize = cachedSize->second;
            m_logoData.reset(new uint8_t[m_logoSize]);
            if (m_logoData) {
                memcpy(m_logoData.get(), cachedLogo->second.get(), m_logoSize);
                m_hasLogo = true;
                Log.infoln("Using cached logo for %s", m_teamAbbrev.c_str());
            }
        }
    } else {
        // No cached data, trigger immediate update on setup
        Log.infoln("NHLWidget setup() - no cached data, triggering initial update");
        update(true);
    }
    
    // Start preloading all teams in the background (only if not already done)
    if (m_teamDataCache.size() < m_teamList.size()) {
        preloadAllTeams();
    }
}

void NHLWidget::draw(bool force) {
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
        // Get team colors once - with safety checks
        std::vector<NHLDataModel::TeamColor> colors = m_teamData.getColors();
        uint16_t primaryColor = (!colors.empty() && !colors[0].code.isEmpty())
                                    ? m_manager.color565FromHex(colors[0].code)
                                    : TFT_WHITE;
        uint16_t secondaryColor = (colors.size() > 1 && !colors[1].code.isEmpty())
                                      ? m_manager.color565FromHex(colors[1].code)
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

void NHLWidget::update(bool force) {
    Log.infoln("NHL Widget update() called - Team: %s", m_teamAbbrev.c_str());
    
    // Fetch combined team data (standings + schedule) from simplified proxy endpoint
    String teamUrl = String(NHL_API_BASE_URL) + "/team/" + String(m_teamAbbrev.c_str());
    Log.infoln("Fetching team data from: %s", teamUrl.c_str());
    
    auto teamTask = TaskFactory::createHttpGetTask(
        teamUrl,
        [this](int httpCode, const String &response) {
            onTeamDataResponse(httpCode, response);
        }
    );

    TaskManager::getInstance()->addTask(std::move(teamTask));
}

void NHLWidget::onTeamDataResponse(int httpCode, const String &response) {
    Log.infoln("Team data response received - HTTP Code: %d, Response length: %d", httpCode, response.length());
    
    // Check for HTTP errors before processing
    if (httpCode <= 0) {
        Log.errorln("Team data request failed with HTTP code: %d - aborting NHL widget update", httpCode);
        return;
    }
    
    if (httpCode != HTTP_CODE_OK) {
        Log.errorln("Team data HTTP request failed, error: %d", httpCode);
        return;
    }

    // Process the combined response (contains both standings and schedule)
    processCombinedTeamData(m_teamData, httpCode, response);

    // Load logo from filesystem after team data is initialized
    if (m_teamData.isInitialized()) {
        loadLogoFromFilesystem();
    }
}

void NHLWidget::loadLogoFromFilesystem() {
    // Construct the logo file path based on team abbreviation
    String logoPath = "/nhl/" + String(m_teamAbbrev.c_str()) + ".jpg";
    
    Log.traceln("Attempting to load logo from: %s", logoPath.c_str());
    
    // Try to open file - if LittleFS isn't mounted or file doesn't exist, just skip
    File logoFile = LittleFS.open(logoPath, "r");
    if (!logoFile) {
        Log.traceln("Logo file not available (LittleFS may not be mounted yet)");
        return;
    }
    
    m_logoSize = logoFile.size();
    
    // Sanity check on file size (logos should be 5-50KB)
    if (m_logoSize < 1000 || m_logoSize > 50000) {
        Log.warningln("Invalid logo file size: %d bytes", m_logoSize);
        logoFile.close();
        return;
    }
    
    // Allocate memory for logo
    m_logoData.reset(new uint8_t[m_logoSize]);
    if (!m_logoData) {
        Log.errorln("Failed to allocate %d bytes for logo", m_logoSize);
        m_logoSize = 0;
        logoFile.close();
        return;
    }
    
    // Read logo data
    size_t bytesRead = logoFile.read(m_logoData.get(), m_logoSize);
    logoFile.close();
    
    if (bytesRead == m_logoSize) {
        m_hasLogo = true;
        m_teamData.setChangedStatus(true);
        Log.infoln("✓ Logo loaded from filesystem (%d bytes)", bytesRead);
    } else {
        Log.warningln("Incomplete logo read: %d/%d bytes", bytesRead, m_logoSize);
        m_logoData.reset();
        m_logoSize = 0;
        m_hasLogo = false;
    }
}

void NHLWidget::onLogoResponse(int httpCode, const String &response) {
    Log.infoln("Logo response received - HTTP Code: %d, Response length: %d", httpCode, response.length());
    
    // Check for HTTP errors before processing
    if (httpCode <= 0) {
        Log.errorln("Logo request failed with HTTP code: %d - skipping logo", httpCode);
        return;
    }
    
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
    } else {
        Log.warningln("Logo fetch failed or empty response");
    }
}

void NHLWidget::processCombinedTeamData(NHLDataModel &team, int httpCode, const String &response) {
    Log.infoln("Processing combined team data response...");
    
    if (httpCode != HTTP_CODE_OK) {
        Log.errorln("Team data HTTP request failed, error: %d", httpCode);
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        Log.errorln("Team data deserializeJson() failed: %s", error.c_str());
        return;
    }

    String teamAbbrevStr = String(m_teamAbbrev.c_str());
    
    // Process team/standings data (flat structure from current proxy)
    Log.infoln("Processing team data for: %s", teamAbbrevStr.c_str());
    
    // Team info
    team.setTeamId(doc["teamId"] | 0);
    team.setFullName(doc["teamName"].as<String>());
    team.setShortName(doc["teamCommonName"].as<String>());
    team.setAbbreviation(doc["teamAbbrev"].as<String>());
    team.setLogoUrl(doc["teamLogo"].as<String>());
    
    // Set logo background color (most teams use white background)
    team.setLogoBackgroundColor("white");
    
    // Extract filename from logo URL
    String logoUrl = doc["teamLogo"].as<String>();
    int lastSlash = logoUrl.lastIndexOf('/');
    if (lastSlash >= 0) {
        team.setLogoImageFileName(logoUrl.substring(lastSlash + 1));
    }

    // Standings info
    team.setDivision(doc["divisionName"].as<String>());
    team.setDivisionRank(String(doc["divisionSequence"].as<int>()));
    team.setConference(doc["conferenceName"].as<String>());
    team.setConferenceRank(String(doc["conferenceSequence"].as<int>()));
    team.setWildcardRank(String(doc["wildcardSequence"].as<int>()));
    team.setPoints(String(doc["points"].as<int>()));
    
    // Win percentage
    float winPctg = doc["winPctg"].as<float>();
    team.setWinningPercentage(String(winPctg, 3));

    // Record (W-L-OT)
    int wins = doc["wins"].as<int>();
    int losses = doc["losses"].as<int>();
    int otLosses = doc["otLosses"].as<int>();
    team.setRecord(String(wins) + "-" + String(losses) + "-" + String(otLosses));

    // Last 10 games record
    int l10Wins = doc["l10Wins"].as<int>();
    int l10Losses = doc["l10Losses"].as<int>();
    int l10OtLosses = doc["l10OtLosses"].as<int>();
    team.setLastTen(String(l10Wins) + "-" + String(l10Losses) + "-" + String(l10OtLosses));

    // Season
    team.setSeason(String(doc["seasonId"].as<int>()));

    // Set default team colors (can be customized per team if needed)
    std::vector<NHLDataModel::TeamColor> colors;
    NHLDataModel::TeamColor primary = {"primary", "#6CACE4"};
    NHLDataModel::TeamColor secondary = {"secondary", "#010101"};
    colors.push_back(primary);
    colors.push_back(secondary);
    team.setColors(colors);
    
    Log.infoln("Team/standings data processed for %s", teamAbbrevStr.c_str());

    // Process last game (already parsed by proxy)
    JsonObject lastGame = doc["lastGame"];
    if (!lastGame.isNull()) {
        String gameDate = lastGame["gameDate"].as<String>();
        team.setLastGameDate(formatDate(gameDate));
        team.setLastGameDay(getDayOfWeek(gameDate));
        
        team.setLastGameOpponent(lastGame["opponent"].as<String>());
        
        int ourScore = lastGame["ourScore"] | 0;
        int theirScore = lastGame["theirScore"] | 0;
        team.setLastGameScore(String(ourScore) + "-" + String(theirScore));
        
        // Determine result
        String result = (ourScore > theirScore) ? "W" : "L";
        team.setLastGameResult(result);
        
        // Format game time
        String startTime = lastGame["startTimeUTC"].as<String>();
        team.setLastGameTime(formatTime(startTime));
        
        Log.infoln("Last game processed: %s vs %s", teamAbbrevStr.c_str(), lastGame["opponent"].as<String>().c_str());
    }

    // Process next game (already parsed by proxy)
    JsonObject nextGame = doc["nextGame"];
    if (!nextGame.isNull()) {
        String gameDate = nextGame["gameDate"].as<String>();
        team.setNextGameDate(formatDate(gameDate));
        team.setNextGameDay(getDayOfWeek(gameDate));
        
        team.setNextGameOpponent(nextGame["opponent"].as<String>());
        
        bool isHome = nextGame["isHome"] | false;
        String location = isHome ? "Home" : "Away";
        team.setNextGameLocation(location);
        
        // Format game time
        String startTime = nextGame["startTimeUTC"].as<String>();
        team.setNextGameTime(formatTime(startTime));
        
        // Get TV broadcast info
        String tvInfo = nextGame["tvBroadcast"].as<String>();
        if (tvInfo.isEmpty()) {
            tvInfo = "TBD";
        }
        team.setNextGameTvBroadcast(tvInfo);
        
        Log.infoln("Next game processed: %s vs %s", teamAbbrevStr.c_str(), nextGame["opponent"].as<String>().c_str());
    }

    team.setInitializationStatus(true);
    team.setChangedStatus(true);
    Log.infoln("Team data processed successfully - widget initialized!");
}

// Helper function to format date from ISO format (YYYY-MM-DD) to readable format (MM/DD)
String NHLWidget::formatDate(const String &isoDate) {
    if (isoDate.length() < 10) return isoDate;
    
    String month = isoDate.substring(5, 7);
    String day = isoDate.substring(8, 10);
    return month + "/" + day;
}

// Helper function to get day of week from ISO date
String NHLWidget::getDayOfWeek(const String &isoDate) {
    if (isoDate.length() < 10) return "";
    
    int year = isoDate.substring(0, 4).toInt();
    int month = isoDate.substring(5, 7).toInt();
    int day = isoDate.substring(8, 10).toInt();
    
    // Zeller's congruence algorithm
    if (month < 3) {
        month += 12;
        year--;
    }
    
    int q = day;
    int m = month;
    int k = year % 100;
    int j = year / 100;
    
    int h = (q + ((13 * (m + 1)) / 5) + k + (k / 4) + (j / 4) - (2 * j)) % 7;
    
    const char* days[] = {"Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"};
    return String(days[h]);
}

// Helper function to format time from ISO UTC format to MST (UTC-7)
String NHLWidget::formatTime(const String &isoTime) {
    if (isoTime.length() < 19) return "TBD";
    
    // Extract hour and minute from ISO format (YYYY-MM-DDTHH:MM:SSZ)
    int hour = isoTime.substring(11, 13).toInt();
    int minute = isoTime.substring(14, 16).toInt();
    
    // Convert from UTC to MST (UTC-7)
    hour -= 7;
    
    // Handle day rollover
    if (hour < 0) {
        hour += 24;
    }
    
    // Convert to 12-hour format with AM/PM
    String ampm = (hour >= 12) ? "PM" : "AM";
    if (hour > 12) hour -= 12;
    if (hour == 0) hour = 12;
    
    String minStr = (minute < 10) ? "0" + String(minute) : String(minute);
    return String(hour) + ":" + minStr + " " + ampm + " MST";
}

void NHLWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        // Short press switches to next team and fetches new data
        switchToNextTeam();
    }
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        // Medium press forces data refresh for current team
        update(true);
    }
}
void NHLWidget::nextPage() {
    // future - to implement navigation between screen sets later:
    // m_currentScreenSet = (m_currentScreenSet + 1) % TOTAL_SCREEN_SETS;
    // For now, just force redraw all screens:
    m_teamData.setChangedStatus(true);
    draw(true);
}

void NHLWidget::switchToNextTeam() {
    // Cycle to next team in the list
    m_currentTeamIndex = (m_currentTeamIndex + 1) % m_teamList.size();
    m_teamAbbrev = m_teamList[m_currentTeamIndex];
    
    Log.infoln("Switching to team: %s (index %d)", m_teamAbbrev.c_str(), m_currentTeamIndex);
    
    // Check if team data is cached
    auto cachedData = m_teamDataCache.find(m_teamAbbrev);
    if (cachedData != m_teamDataCache.end()) {
        // Use cached data for instant switch
        Log.infoln("Using cached data for %s", m_teamAbbrev.c_str());
        m_teamData = cachedData->second;
        m_teamData.setChangedStatus(true);
        
        // Load cached logo if available
        auto cachedLogo = m_logoDataCache.find(m_teamAbbrev);
        auto cachedSize = m_logoSizeCache.find(m_teamAbbrev);
        if (cachedLogo != m_logoDataCache.end() && cachedSize != m_logoSizeCache.end()) {
            m_logoSize = cachedSize->second;
            m_logoData.reset(new uint8_t[m_logoSize]);
            if (m_logoData) {
                memcpy(m_logoData.get(), cachedLogo->second.get(), m_logoSize);
                m_hasLogo = true;
                Log.infoln("Using cached logo for %s", m_teamAbbrev.c_str());
            }
        } else {
            m_logoData.reset();
            m_logoSize = 0;
            m_hasLogo = false;
        }
        
        // Force redraw with cached data
        draw(true);
    } else {
        // No cached data, fetch from API
        Log.infoln("No cached data for %s, fetching from API", m_teamAbbrev.c_str());
        
        // Clear current logo
        m_logoData.reset();
        m_logoSize = 0;
        m_hasLogo = false;
        
        // Reset team data
        m_teamData = NHLDataModel();
        
        // Trigger immediate update for new team
        update(true);
        
        // Force redraw
        draw(true);
    }
}

String NHLWidget::getName() {
    return "NHL";
}

void NHLWidget::drawTeamInfoScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Team Info Screen");
    m_manager.selectScreen(0);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "", "",
                            TFT_BLACK, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, FOOTER_TEXT_SIZE);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString(m_teamData.getShortName(), ScreenCenterX, contentCenterY - 30, 24);
    m_manager.drawCentreString("Record: " + m_teamData.getRecord(), ScreenCenterX, contentCenterY + 10, 24);
    m_manager.drawCentreString("Last 10: " + m_teamData.getLastTen(), ScreenCenterX, contentCenterY + 65, 13);
}

void NHLWidget::drawLastGameScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Last Game Screen");
    m_manager.selectScreen(1);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "Last Game", m_teamData.getLastGameTime(),
                            TFT_BLACK, TFT_BLACK,
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

void NHLWidget::drawTeamLogoScreen(uint16_t primaryColor) {
    Log.infoln("Drawing Logo Screen - hasLogo: %d, logoSize: %d", m_hasLogo, m_logoSize);
    m_manager.selectScreen(2);

    // Fill background with black
    m_manager.fillScreen(TFT_BLACK);

    // Draw logo centered on screen
    if (m_hasLogo && m_logoData && m_logoSize > 0) {
        // Calculate logo position to center it
        int logoSize = 200;  // Fixed size for now
        int logoX = (SCREEN_SIZE - logoSize) / 2;
        int logoY = (SCREEN_SIZE - logoSize) / 2;
        
        Log.infoln("Drawing JPG at (%d, %d), data size: %d bytes",
                   logoX, logoY, m_logoSize);
        
        // Draw the JPG - it will auto-scale to fit
        JRESULT result = m_manager.drawJpg(logoX, logoY, m_logoData.get(), m_logoSize);
        Log.infoln("JPG draw call completed with result: %d", result);
    } else {
        Log.warningln("Logo not drawn - hasLogo: %d, logoData: %d, logoSize: %d",
                      m_hasLogo, (m_logoData != nullptr), m_logoSize);
    }
}

void NHLWidget::drawNextGameScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Next Game Screen");
    m_manager.selectScreen(3);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "Next Game", "",
                            TFT_BLACK, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, FOOTER_TEXT_SIZE);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString(m_teamData.getNextGameDay() + ", " + m_teamData.getNextGameDate(),
                               ScreenCenterX, contentCenterY - 50, 20);
    m_manager.drawCentreString(m_teamData.getNextGameTime(),
                               ScreenCenterX, contentCenterY - 25, 22);
    m_manager.drawCentreString("vs " + m_teamData.getNextGameOpponent(),
                               ScreenCenterX, contentCenterY + 5, 24);
    m_manager.drawCentreString(m_teamData.getNextGameLocation(),
                               ScreenCenterX, contentCenterY + 35, 20);
    m_manager.drawCentreString("TV: " + m_teamData.getNextGameTvBroadcast(),
                               ScreenCenterX, contentCenterY + 60, 18);
}

void NHLWidget::drawStandingsScreen(uint16_t primaryColor, uint16_t secondaryColor) {
    Log.traceln("Drawing Standings Screen");
    m_manager.selectScreen(4);

    const int contentTop = TOP_BAR_HEIGHT;
    const int contentHeight = SCREEN_SIZE - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT;
    const int contentCenterY = contentTop + (contentHeight / 2);

    m_manager.drawTitleBars(primaryColor, secondaryColor,
                            "Standings", m_teamData.getConference(),
                            TFT_BLACK, TFT_BLACK,
                            TOP_BAR_HEIGHT, BOTTOM_BAR_HEIGHT,
                            HEADER_TEXT_SIZE, 15);

    m_manager.fillRect(0, contentTop, SCREEN_SIZE, contentHeight, TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    
    // Display division and division rank
    m_manager.drawCentreString(m_teamData.getDivision() + " Division", ScreenCenterX, contentCenterY - 65, 18);
    m_manager.setFontColor(primaryColor, TFT_BLACK);
    m_manager.drawCentreString("#" + m_teamData.getDivisionRank(), ScreenCenterX, contentCenterY - 40, 28);
    
    // Display conference rank
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    m_manager.drawCentreString("Conference Rank", ScreenCenterX, contentCenterY - 5, 16);
    m_manager.setFontColor(primaryColor, TFT_BLACK);
    m_manager.drawCentreString("#" + m_teamData.getConferenceRank(), ScreenCenterX, contentCenterY + 20, 28);
    
    // Display wild card position if applicable (0 means not in wild card)
    String wildcardRank = m_teamData.getWildcardRank();
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
    if (wildcardRank != "0" && !wildcardRank.isEmpty()) {
        m_manager.drawCentreString("Wild Card #" + wildcardRank, ScreenCenterX, contentCenterY + 50, 18);
    } else {
        m_manager.drawCentreString("Points: " + m_teamData.getPoints(), ScreenCenterX, contentCenterY + 50, 18);
    }
    
    // Display win percentage at bottom
    m_manager.drawCentreString("Win %: " + m_teamData.getWinningPercentage(), ScreenCenterX, contentCenterY + 75, 16);
}
