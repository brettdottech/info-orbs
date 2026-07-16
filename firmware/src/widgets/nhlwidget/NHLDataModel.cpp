#include "NHLDataModel.h"

NHLDataModel::NHLDataModel() {
}

NHLDataModel &NHLDataModel::setTeamId(int teamId) {
    if (m_teamId != teamId) {
        m_teamId = teamId;
        m_changed = true;
    }
    return *this;
}
int NHLDataModel::getTeamId() {
    return m_teamId;
}
int NHLDataModel::getTeamId() const {
    return m_teamId;
}
NHLDataModel &NHLDataModel::setSeason(String season) {
    if (m_season != season) {
        m_season = season;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getSeason() {
    return m_season;
}

NHLDataModel &NHLDataModel::setFullName(String fullName) {
    if (m_fullName != fullName) {
        m_fullName = fullName;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getFullName() {
    return m_fullName;
}

NHLDataModel &NHLDataModel::setShortName(String shortName) {
    if (m_shortName != shortName) {
        m_shortName = shortName;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getShortName() {
    return m_shortName;
}

NHLDataModel &NHLDataModel::setAbbreviation(String abbreviation) {
    if (m_abbreviation != abbreviation) {
        m_abbreviation = abbreviation;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getAbbreviation() {
    return m_abbreviation;
}

NHLDataModel &NHLDataModel::setColors(std::vector<TeamColor> colors) {
    m_colors = colors;
    m_changed = true;
    return *this;
}
std::vector<NHLDataModel::TeamColor> NHLDataModel::getColors() {
    return m_colors;
}

NHLDataModel &NHLDataModel::setLogoUrl(String logoUrl) {
    if (m_logoUrl != logoUrl) {
        m_logoUrl = logoUrl;
        m_changed = true;
    }
    return *this;
}

String NHLDataModel::getLogoUrl() {
    return m_logoUrl;
}

NHLDataModel &NHLDataModel::setLogoImageFileName(String logoImageFileName) {
    if (m_logoImageFileName != logoImageFileName) {
        m_logoImageFileName = logoImageFileName;
        m_changed = true;
    }
    return *this;
}

String NHLDataModel::getLogoImageFileName() {
    return m_logoImageFileName;
}

NHLDataModel &NHLDataModel::setLogoBackgroundColor(String logoBackgroundColor) {
    if (m_logoBackgroundColor != logoBackgroundColor) {
        m_logoBackgroundColor = logoBackgroundColor;
        m_changed = true;
    }
    return *this;
}

String NHLDataModel::getLogoBackgroundColor() {
    return m_logoBackgroundColor;
}

NHLDataModel &NHLDataModel::setRecord(String record) {
    if (m_record != record) {
        m_record = record;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getRecord() {
    return m_record;
}

NHLDataModel &NHLDataModel::setDivision(String division) {
    if (m_division != division) {
        m_division = division;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getDivision() {
    return m_division;
}

NHLDataModel &NHLDataModel::setDivisionRank(String divisionRank) {
    if (m_divisionRank != divisionRank) {
        m_divisionRank = divisionRank;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getDivisionRank() {
    return m_divisionRank;
}

NHLDataModel &NHLDataModel::setConference(String conference) {
    if (m_conference != conference) {
        m_conference = conference;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getConference() {
    return m_conference;
}

NHLDataModel &NHLDataModel::setConferenceRank(String conferenceRank) {
    if (m_conferenceRank != conferenceRank) {
        m_conferenceRank = conferenceRank;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getConferenceRank() {
    return m_conferenceRank;
}

NHLDataModel &NHLDataModel::setWildcardRank(String wildcardRank) {
    if (m_wildcardRank != wildcardRank) {
        m_wildcardRank = wildcardRank;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getWildcardRank() {
    return m_wildcardRank;
}

NHLDataModel &NHLDataModel::setWinningPercentage(String winningPercentage) {
    if (m_winningPercentage != winningPercentage) {
        m_winningPercentage = winningPercentage;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getWinningPercentage() {
    return m_winningPercentage;
}

NHLDataModel &NHLDataModel::setPoints(String points) {
    if (m_points != points) {
        m_points = points;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getPoints() {
    return m_points;
}

// Last game methods
NHLDataModel &NHLDataModel::setLastGameDate(String date) {
    if (m_lastGameDate != date) {
        m_lastGameDate = date;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastGameDate() {
    return m_lastGameDate;
}

NHLDataModel &NHLDataModel::setLastGameDay(String day) {
    if (m_lastGameDay != day) {
        m_lastGameDay = day;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastGameDay() {
    return m_lastGameDay;
}

NHLDataModel &NHLDataModel::setLastGameOpponent(String opponent) {
    if (m_lastGameOpponent != opponent) {
        m_lastGameOpponent = opponent;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastGameOpponent() {
    return m_lastGameOpponent;
}

NHLDataModel &NHLDataModel::setLastGameScore(String score) {
    if (m_lastGameScore != score) {
        m_lastGameScore = score;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastGameScore() {
    return m_lastGameScore;
}

NHLDataModel &NHLDataModel::setLastGameResult(String result) {
    if (m_lastGameResult != result) {
        m_lastGameResult = result;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastGameResult() {
    return m_lastGameResult;
}

NHLDataModel &NHLDataModel::setLastGameTime(String gameTime) {
    if (m_lastGameTime != gameTime) {
        m_lastGameTime = gameTime;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastGameTime() {
    return m_lastGameTime;
}

NHLDataModel &NHLDataModel::setLastTen(String lastTen) {
    if (m_lastTen != lastTen) {
        m_lastTen = lastTen;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getLastTen() {
    return m_lastTen;
}

// Next game methods
NHLDataModel &NHLDataModel::setNextGameDate(String date) {
    if (m_nextGameDate != date) {
        m_nextGameDate = date;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getNextGameDate() {
    return m_nextGameDate;
}

NHLDataModel &NHLDataModel::setNextGameDay(String day) {
    if (m_nextGameDay != day) {
        m_nextGameDay = day;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getNextGameDay() {
    return m_nextGameDay;
}

NHLDataModel &NHLDataModel::setNextGameOpponent(String opponent) {
    if (m_nextGameOpponent != opponent) {
        m_nextGameOpponent = opponent;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getNextGameOpponent() {
    return m_nextGameOpponent;
}

NHLDataModel &NHLDataModel::setNextGameLocation(String location) {
    if (m_nextGameLocation != location) {
        m_nextGameLocation = location;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getNextGameLocation() {
    return m_nextGameLocation;
}

NHLDataModel &NHLDataModel::setNextGameTime(String gameTime) {
    if (m_nextGameTime != gameTime) {
        m_nextGameTime = gameTime;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getNextGameTime() {
    return m_nextGameTime;
}

NHLDataModel &NHLDataModel::setNextGameTvBroadcast(String tvBroadcast) {
    if (m_nextGameTvBroadcast != tvBroadcast) {
        m_nextGameTvBroadcast = tvBroadcast;
        m_changed = true;
    }
    return *this;
}
String NHLDataModel::getNextGameTvBroadcast() {
    return m_nextGameTvBroadcast;
}

bool NHLDataModel::isChanged() {
    return m_changed;
}
NHLDataModel &NHLDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}

bool NHLDataModel::isInitialized() {
    return m_initialized;
}

NHLDataModel &NHLDataModel::setInitializationStatus(bool initialized) {
    m_initialized = initialized;
    return *this;
}
