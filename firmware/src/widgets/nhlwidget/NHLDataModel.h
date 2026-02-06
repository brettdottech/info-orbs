#ifndef NHL_DATA_MODEL_H
#define NHL_DATA_MODEL_H

#include <Arduino.h>
#include <vector>

class NHLDataModel {
public:
    struct TeamColor {
        String name;
        String code;
    };

    NHLDataModel();
    NHLDataModel &setTeamId(int teamId);
    int getTeamId();
    int getTeamId() const;
    NHLDataModel &setSeason(String season);
    String getSeason();
    NHLDataModel &setFullName(String fullName);
    String getFullName();
    NHLDataModel &setShortName(String shortName);
    String getShortName();
    NHLDataModel &setAbbreviation(String abbreviation);
    String getAbbreviation();
    NHLDataModel &setColors(std::vector<TeamColor> colors);
    std::vector<TeamColor> getColors();
    NHLDataModel &setLogoUrl(String logoUrl);
    String getLogoUrl();
    NHLDataModel &setLogoImageFileName(String logoImageFileName);
    String getLogoImageFileName();
    NHLDataModel &setLogoBackgroundColor(String logoBackgroundColor);
    String getLogoBackgroundColor();
    NHLDataModel &setRecord(String record);
    String getRecord();
    NHLDataModel &setDivision(String division);
    String getDivision();
    NHLDataModel &setDivisionRank(String divisionRank);
    String getDivisionRank();
    NHLDataModel &setConference(String conference);
    String getConference();
    NHLDataModel &setConferenceRank(String conferenceRank);
    String getConferenceRank();
    NHLDataModel &setWildcardRank(String wildcardRank);
    String getWildcardRank();
    NHLDataModel &setWinningPercentage(String winningPercentage);
    String getWinningPercentage();
    NHLDataModel &setPoints(String points);
    String getPoints();
    NHLDataModel &setLastGameDate(String date);
    String getLastGameDate();
    NHLDataModel &setLastGameDay(String day);
    String getLastGameDay();
    NHLDataModel &setLastGameOpponent(String opponent);
    String getLastGameOpponent();
    NHLDataModel &setLastGameScore(String score);
    String getLastGameScore();
    NHLDataModel &setLastGameResult(String result);
    String getLastGameResult();
    NHLDataModel &setLastGameTime(String gameTime);
    String getLastGameTime();
    NHLDataModel &setLastTen(String lastTen);
    String getLastTen();
    NHLDataModel &setNextGameDate(String date);
    String getNextGameDate();
    NHLDataModel &setNextGameDay(String day);
    String getNextGameDay();
    NHLDataModel &setNextGameOpponent(String opponent);
    String getNextGameOpponent();
    NHLDataModel &setNextGameLocation(String location);
    String getNextGameLocation();
    NHLDataModel &setNextGameTime(String gameTime);
    String getNextGameTime();
    NHLDataModel &setNextGameTvBroadcast(String tvBroadcast);
    String getNextGameTvBroadcast();
    bool isChanged();
    NHLDataModel &setChangedStatus(bool changed);
    bool isInitialized();
    NHLDataModel &setInitializationStatus(bool initialized);

private:
    int m_teamId = 0;
    String m_season = "";
    String m_fullName = "";
    String m_shortName = "";
    String m_abbreviation = "";
    std::vector<TeamColor> m_colors;
    String m_logoUrl = "";
    String m_logoImageFileName = "";
    String m_logoBackgroundColor = "";
    String m_record = "";
    String m_division = "";
    String m_divisionRank = "";
    String m_conference = "";
    String m_conferenceRank = "";
    String m_wildcardRank = "";
    String m_winningPercentage = "";
    String m_points = "";
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
    String m_nextGameTime = "";
    String m_nextGameTvBroadcast = "";
    bool m_changed = false;
    bool m_initialized = false;
};

#endif
