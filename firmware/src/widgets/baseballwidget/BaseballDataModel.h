#ifndef BASEBALL_DATA_MODEL_H
#define BASEBALL_DATA_MODEL_H

#include <Arduino.h>
#include <vector>

class BaseballDataModel {
public:
    struct TeamColor {
        String name;
        String code;
    };

    BaseballDataModel();
    BaseballDataModel &setTeamId(int teamId);
    int getTeamId();
    int getTeamId() const;
    BaseballDataModel &setSeason(String season);
    String getSeason();
    BaseballDataModel &setFullName(String fullName);
    String getFullName();
    BaseballDataModel &setShortName(String shortName);
    String getShortName();
    BaseballDataModel &setColors(std::vector<TeamColor> colors);
    std::vector<TeamColor> getColors();
    BaseballDataModel &setLogoUrl(String logoUrl);
    String getLogoUrl();
    BaseballDataModel &setLogoImageFileName(String logoImageFileName);
    String getLogoImageFileName();
    BaseballDataModel &setLogoBackgroundColor(String logoBackgroundColor);
    String getLogoBackgroundColor();
    BaseballDataModel &setRecord(String record);
    String getRecord();
    BaseballDataModel &setDivision(String division);
    String getDivision();
    BaseballDataModel &setDivisionRank(String divisionRank);
    String getDivisionRank();
    BaseballDataModel &setWinningPercentage(String winningPercentage);
    String getWinningPercentage();
    BaseballDataModel &setGamesBack(String gamesBack);
    String getGamesBack();
    BaseballDataModel &setLastGameDate(String date);
    String getLastGameDate();
    BaseballDataModel &setLastGameDay(String day);
    String getLastGameDay();
    BaseballDataModel &setLastGameOpponent(String opponent);
    String getLastGameOpponent();
    BaseballDataModel &setLastGameScore(String score);
    String getLastGameScore();
    BaseballDataModel &setLastGameResult(String result);
    String getLastGameResult();
    BaseballDataModel &setLastGameTime(String gameTime);
    String getLastGameTime();
    BaseballDataModel &setLastTen(String lastTen);
    String getLastTen();
    BaseballDataModel &setNextGameDate(String date);
    String getNextGameDate();
    BaseballDataModel &setNextGameDay(String day);
    String getNextGameDay();
    BaseballDataModel &setNextGameOpponent(String opponent);
    String getNextGameOpponent();
    BaseballDataModel &setNextGameLocation(String location);
    String getNextGameLocation();
    BaseballDataModel &setNextGameProbablePitcher(String pitcher);
    String getNextGameProbablePitcher();
    BaseballDataModel &setNextGameTime(String gameTime);
    String getNextGameTime();
    BaseballDataModel &setNextGameTvBroadcast(String tvBroadcast);
    String getNextGameTvBroadcast();
    bool isChanged();
    BaseballDataModel &setChangedStatus(bool changed);
    bool isInitialized();
    BaseballDataModel &setInitializationStatus(bool initialized);

private:
    int m_teamId = 0;
    String m_season = "";
    String m_fullName = "";
    String m_shortName = "";
    std::vector<TeamColor> m_colors;
    String m_logoUrl = "";
    String m_logoImageFileName = "";
    String m_logoBackgroundColor = "";
    String m_record = "";
    String m_division = "";
    String m_divisionRank = "";
    String m_winningPercentage = "";
    String m_gamesBack = "";
    String m_lastGameDate = "";
    String m_lastGameDay = "";
    String m_lastGameOpponent = "";
    String m_lastGameScore = "";
    String m_lastGameResult = "";
    String m_lastGameTime = "";
    String m_lastTen = "";
    String m_nextGameDate = "";
    String m_nextGameDay = "";
    String m_nextGameOpponent = "";
    String m_nextGameLocation = "";
    String m_nextGameProbablePitcher = "";
    String m_nextGameTime = "";
    String m_nextGameTvBroadcast = "";
    bool m_changed = false;
    bool m_initialized = false;
};

#endif
