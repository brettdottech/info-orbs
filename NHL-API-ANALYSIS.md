# NHL API Analysis Report

## Summary

The NHL API is **working correctly** and returning valid data. However, there is a **mismatch between what the code expects and what the API actually returns**.

## Problem Identified

The code in [`NHLWidget.cpp`](firmware/src/widgets/nhlwidget/NHLWidget.cpp:60) calls:
```
https://api-web.nhle.com/v1/club-schedule-season/{teamAbbrev}/now
```

And expects this JSON structure (based on parsing logic in lines 89-149):
```json
{
  "teamId": 68,
  "season": "20252026",
  "team": {
    "fullName": "Utah Mammoth",
    "shortName": "Mammoth",
    "colors": [
      {"name": "primary", "code": "#6CACE4"},
      {"name": "secondary", "code": "#010101"}
    ],
    "logoUrl": "https://...",
    "logoImageFileName": "UTA.svg",
    "logoBackgroundColor": "white"
  },
  "record": "28-21-4",
  "standings": {
    "division": "Central",
    "divisionRank": "4",
    "winningPercentage": "0.528",
    "points": "60"
  },
  "lastGame": {
    "date": "2026-01-27",
    "day": "Monday",
    "opponent": "Avalanche",
    "score": "3-2",
    "result": "W",
    "gameTime": "7:00 PM"
  },
  "lastTen": {
    "record": "8-1-1"
  },
  "nextGame": {
    "date": "2026-01-29",
    "day": "Wednesday",
    "opponent": "Wild",
    "location": "Home",
    "gameTime": "7:00 PM",
    "tvBroadcast": "ESPN+"
  }
}
```

## What the API Actually Returns

### 1. Schedule Endpoint Response
**URL:** `https://api-web.nhle.com/v1/club-schedule-season/UTA/now`

**Actual Structure:**
```json
{
  "previousSeason": 20242025,
  "currentSeason": 20252026,
  "clubTimezone": "America/Denver",
  "clubUTCOffset": "-07:00",
  "games": [
    {
      "id": 2025010103,
      "season": 20252026,
      "gameType": 1,
      "gameDate": "2025-09-21",
      "venue": {"default": "Magness Arena"},
      "gameState": "FINAL",
      "tvBroadcasts": [...],
      "awayTeam": {
        "id": 21,
        "commonName": {"default": "Avalanche"},
        "placeName": {"default": "Colorado"},
        "abbrev": "COL",
        "logo": "https://assets.nhle.com/logos/nhl/svg/COL_light.svg",
        "score": 5
      },
      "homeTeam": {
        "id": 68,
        "commonName": {"default": "Mammoth"},
        "placeName": {"default": "Utah"},
        "abbrev": "UTA",
        "logo": "https://assets.nhle.com/logos/nhl/svg/UTA_light.svg?season=20252026",
        "score": 1
      }
    }
    // ... 88 more games
  ]
}
```

**Missing Fields:**
- ❌ `teamId` (not at top level)
- ❌ `team` object with fullName, shortName, colors, etc.
- ❌ `record` (W-L-OT record)
- ❌ `standings` object
- ❌ `lastGame` object (must be parsed from games array)
- ❌ `nextGame` object (must be parsed from games array)
- ❌ `lastTen` object

### 2. Standings Endpoint (Provides Missing Data)
**URL:** `https://api-web.nhle.com/v1/standings/now`

**Returns team data including:**
```json
{
  "teamName": {"default": "Utah Mammoth"},
  "teamCommonName": {"default": "Mammoth"},
  "teamAbbrev": {"default": "UTA"},
  "teamLogo": "https://assets.nhle.com/logos/nhl/svg/UTA_light.svg?season=20252026",
  "divisionName": "Central",
  "divisionSequence": 4,
  "conferenceName": "Western",
  "points": 60,
  "wins": 28,
  "losses": 21,
  "otLosses": 4,
  "winPctg": 0.528302,
  "l10Wins": 8,
  "l10Losses": 1,
  "l10OtLosses": 1,
  "gamesPlayed": 53,
  "goalFor": 167,
  "goalAgainst": 146
  // ... many more fields
}
```

## Test Results

### Schedule API Test
```bash
curl -L "https://api-web.nhle.com/v1/club-schedule-season/UTA/now"
```

✅ **Status:** 200 OK  
✅ **Data Returned:** 89 games  
✅ **Valid JSON:** Yes  
❌ **Matches Expected Structure:** No

### Standings API Test
```bash
curl -L "https://api-web.nhle.com/v1/standings/now"
```

✅ **Status:** 200 OK  
✅ **Data Returned:** All 32 teams  
✅ **Valid JSON:** Yes  
✅ **Contains Team Info:** Yes

## Root Cause

The code's [`processScheduleResponse()`](firmware/src/widgets/nhlwidget/NHLWidget.cpp:89) function expects a pre-formatted response that combines:
1. Team information
2. Standings data
3. Last game details
4. Next game details
5. Last 10 games record

But the actual NHL API separates this data into:
1. **Schedule endpoint** - Returns raw games array
2. **Standings endpoint** - Returns team info and standings

## Solution Required

The code needs to be updated to:

1. **Call both API endpoints:**
   - `/v1/standings/now` - for team info and standings
   - `/v1/club-schedule-season/{team}/now` - for game schedule

2. **Parse the games array** to extract:
   - Last completed game (most recent game with `gameState: "FINAL"`)
   - Next upcoming game (first game with `gameState: "FUT"` or similar)

3. **Transform the data** into the format expected by the data model

4. **Handle team colors** - The standings API doesn't provide team colors, so these may need to be:
   - Hardcoded per team
   - Fetched from another endpoint
   - Extracted from team logos

## API Endpoints Summary

| Endpoint | Purpose | Status |
|----------|---------|--------|
| `/v1/club-schedule-season/{team}/now` | Get team schedule | ✅ Working |
| `/v1/standings/now` | Get all team standings | ✅ Working |
| Team logo URLs | Get team logos | ✅ Working |

## Next Steps

1. ✅ **API is working** - No issues with the NHL API itself
2. ❌ **Code needs updating** - The parsing logic must be rewritten to match the actual API structure
3. 📝 **Consider creating a helper function** to transform the API response into the expected format
4. 🧪 **Add error handling** for cases where games array is empty or team not found

## Sample Data Files

Test data has been saved to:
- `/tmp/nhl-schedule.json` - Schedule API response
- `/tmp/nhl-standings.json` - Standings API response

You can examine these files to see the complete API structure.

## Conclusion

**The NHL API is functioning correctly and returning valid data.** The issue is that the firmware code's parsing logic doesn't match the current API structure. The API either changed since the code was written, or the code was written expecting a different (possibly custom) API endpoint that aggregates this data.

To fix the widget, the [`processScheduleResponse()`](firmware/src/widgets/nhlwidget/NHLWidget.cpp:89) function and the [`update()`](firmware/src/widgets/nhlwidget/NHLWidget.cpp:58) function need to be rewritten to:
1. Fetch data from both endpoints
2. Parse the actual JSON structure
3. Transform it into the format the data model expects
