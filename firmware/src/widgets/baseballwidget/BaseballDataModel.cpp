#include "BaseballDataModel.h"

BaseballDataModel::BaseballDataModel() {
}

BaseballDataModel &BaseballDataModel::setTeamId(int teamId) {
    if (m_teamId != teamId) {
        m_teamId = teamId;
        m_changed = true;
    }
    return *this;
}
int BaseballDataModel::getTeamId() {
    return m_teamId;
}
int BaseballDataModel::getTeamId() const {
    return m_teamId; // Same implementation as non-const version
}
BaseballDataModel &BaseballDataModel::setSeason(String season) {
    if (m_season != season) {
        m_season = season;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getSeason() {
    return m_season;
}

BaseballDataModel &BaseballDataModel::setFullName(String fullName) {
    if (m_fullName != fullName) {
        m_fullName = fullName;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getFullName() {
    return m_fullName;
}

BaseballDataModel &BaseballDataModel::setShortName(String shortName) {
    if (m_shortName != shortName) {
        m_shortName = shortName;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getShortName() {
    return m_shortName;
}

BaseballDataModel &BaseballDataModel::setColors(std::vector<TeamColor> colors) {
    m_colors = colors;
    m_changed = true;
    return *this;
}
std::vector<BaseballDataModel::TeamColor> BaseballDataModel::getColors() {
    return m_colors;
}

BaseballDataModel &BaseballDataModel::setLogoUrl(String logoUrl) {
    if (m_logoUrl != logoUrl) {
        m_logoUrl = logoUrl;
        m_changed = true;
    }
    return *this;
}

String BaseballDataModel::getLogoUrl() {
    return m_logoUrl;
}

BaseballDataModel &BaseballDataModel::setLogoImageFileName(String logoImageFileName) {
    if (m_logoImageFileName != logoImageFileName) {
        m_logoImageFileName = logoImageFileName;
        m_changed = true;
    }
    return *this;
}

String BaseballDataModel::getLogoImageFileName() {
    return m_logoImageFileName;
}

BaseballDataModel &BaseballDataModel::setLogoBackgroundColor(String logoBackgroundColor) {
    if (m_logoBackgroundColor != logoBackgroundColor) {
        m_logoBackgroundColor = logoBackgroundColor;
        m_changed = true;
    }
    return *this;
}

String BaseballDataModel::getLogoBackgroundColor() {
    return m_logoBackgroundColor;
}

BaseballDataModel &BaseballDataModel::setRecord(String record) {
    if (m_record != record) {
        m_record = record;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getRecord() {
    return m_record;
}

BaseballDataModel &BaseballDataModel::setDivision(String division) {
    if (m_division != division) {
        m_division = division;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getDivision() {
    return m_division;
}

BaseballDataModel &BaseballDataModel::setDivisionRank(String divisionRank) {
    if (m_divisionRank != divisionRank) {
        m_divisionRank = divisionRank;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getDivisionRank() {
    return m_divisionRank;
}

BaseballDataModel &BaseballDataModel::setWinningPercentage(String winningPercentage) {
    if (m_winningPercentage != winningPercentage) {
        m_winningPercentage = winningPercentage;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getWinningPercentage() {
    return m_winningPercentage;
}

BaseballDataModel &BaseballDataModel::setGamesBack(String gamesBack) {
    if (m_gamesBack != gamesBack) {
        m_gamesBack = gamesBack;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getGamesBack() {
    return m_gamesBack;
}

// Last game methods
BaseballDataModel &BaseballDataModel::setLastGameDate(String date) {
    if (m_lastGameDate != date) {
        m_lastGameDate = date;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastGameDate() {
    return m_lastGameDate;
}

BaseballDataModel &BaseballDataModel::setLastGameDay(String day) {
    if (m_lastGameDay != day) {
        m_lastGameDay = day;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastGameDay() {
    return m_lastGameDay;
}

BaseballDataModel &BaseballDataModel::setLastGameOpponent(String opponent) {
    if (m_lastGameOpponent != opponent) {
        m_lastGameOpponent = opponent;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastGameOpponent() {
    return m_lastGameOpponent;
}

BaseballDataModel &BaseballDataModel::setLastGameScore(String score) {
    if (m_lastGameScore != score) {
        m_lastGameScore = score;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastGameScore() {
    return m_lastGameScore;
}

BaseballDataModel &BaseballDataModel::setLastGameResult(String result) {
    if (m_lastGameResult != result) {
        m_lastGameResult = result;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastGameResult() {
    return m_lastGameResult;
}

BaseballDataModel &BaseballDataModel::setLastGameTime(String gameTime) {
    if (m_lastGameTime != gameTime) {
        m_lastGameTime = gameTime;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastGameTime() {
    return m_lastGameTime;
}

BaseballDataModel &BaseballDataModel::setLastTen(String lastTen) {
    if (m_lastTen != lastTen) {
        m_lastTen = lastTen;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getLastTen() {
    return m_lastTen;
}

// Next game methods
BaseballDataModel &BaseballDataModel::setNextGameDate(String date) {
    if (m_nextGameDate != date) {
        m_nextGameDate = date;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameDate() {
    return m_nextGameDate;
}

BaseballDataModel &BaseballDataModel::setNextGameDay(String day) {
    if (m_nextGameDay != day) {
        m_nextGameDay = day;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameDay() {
    return m_nextGameDay;
}

BaseballDataModel &BaseballDataModel::setNextGameOpponent(String opponent) {
    if (m_nextGameOpponent != opponent) {
        m_nextGameOpponent = opponent;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameOpponent() {
    return m_nextGameOpponent;
}

BaseballDataModel &BaseballDataModel::setNextGameLocation(String location) {
    if (m_nextGameLocation != location) {
        m_nextGameLocation = location;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameLocation() {
    return m_nextGameLocation;
}

BaseballDataModel &BaseballDataModel::setNextGameProbablePitcher(String pitcher) {
    if (m_nextGameProbablePitcher != pitcher) {
        m_nextGameProbablePitcher = pitcher;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameProbablePitcher() {
    return m_nextGameProbablePitcher;
}

BaseballDataModel &BaseballDataModel::setNextGameTime(String gameTime) {
    if (m_nextGameTime != gameTime) {
        m_nextGameTime = gameTime;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameTime() {
    return m_nextGameTime;
}

BaseballDataModel &BaseballDataModel::setNextGameTvBroadcast(String tvBroadcast) {
    if (m_nextGameTvBroadcast != tvBroadcast) {
        m_nextGameTvBroadcast = tvBroadcast;
        m_changed = true;
    }
    return *this;
}
String BaseballDataModel::getNextGameTvBroadcast() {
    return m_nextGameTvBroadcast;
}

bool BaseballDataModel::isChanged() {
    return m_changed;
}
BaseballDataModel &BaseballDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}

bool BaseballDataModel::isInitialized() {
    return m_initialized;
}

BaseballDataModel &BaseballDataModel::setInitializationStatus(bool initialized) {
    m_initialized = initialized;
    return *this;
}
