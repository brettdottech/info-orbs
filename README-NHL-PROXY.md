# NHL API Proxy Server Setup Guide

This proxy server solves the ESP32 memory limitations by fetching NHL data via HTTPS, filtering it, and serving it over plain HTTP with a much smaller payload.

## Prerequisites

- Python 3.7 or higher
- pip (Python package manager)

## Installation

1. Install required Python packages:
```bash
pip install flask requests
```

## Running the Proxy Server

### Option 1: Run Locally on Your Computer

```bash
python3 nhl-proxy-server.py
```

The server will start on `http://0.0.0.0:5000`

### Option 2: Run on Raspberry Pi or Always-On Server

1. Copy `nhl-proxy-server.py` to your server
2. Install dependencies: `pip3 install flask requests`
3. Run the server: `python3 nhl-proxy-server.py`
4. (Optional) Set up as a systemd service for auto-start

### Option 3: Run as a Docker Container

Create a `Dockerfile`:
```dockerfile
FROM python:3.11-slim
WORKDIR /app
COPY nhl-proxy-server.py .
RUN pip install flask requests
EXPOSE 5000
CMD ["python", "nhl-proxy-server.py"]
```

Build and run:
```bash
docker build -t nhl-proxy .
docker run -d -p 5000:5000 --name nhl-proxy nhl-proxy
```

## API Endpoints

### Get Team Data
```
GET http://YOUR_SERVER_IP:5000/nhl/team/<TEAM_ABBREV>
```

Example: `http://192.168.1.100:5000/nhl/team/UTA`

Returns combined standings and schedule data (~1-2KB instead of 15KB):
```json
{
  "teamId": 59,
  "teamAbbrev": "UTA",
  "teamName": "Utah Hockey Club",
  "teamCommonName": "Utah",
  "teamLogo": "https://assets.nhle.com/logos/nhl/svg/UTA_light.svg",
  "divisionName": "Central",
  "divisionSequence": 4,
  "conferenceName": "Western",
  "conferenceSequence": 6,
  "wildcardSequence": 1,
  "points": 52,
  "winPctg": 0.542,
  "wins": 21,
  "losses": 17,
  "otLosses": 10,
  "l10Wins": 5,
  "l10Losses": 3,
  "l10OtLosses": 2,
  "seasonId": 20242025,
  "lastGame": {
    "gameDate": "2026-01-27",
    "startTimeUTC": "2026-01-28T02:00:00Z",
    "opponent": "Avalanche",
    "isHome": false,
    "ourScore": 2,
    "theirScore": 4
  },
  "nextGame": {
    "gameDate": "2026-01-29",
    "startTimeUTC": "2026-01-30T02:00:00Z",
    "opponent": "Blackhawks",
    "isHome": true,
    "tvBroadcast": "Utah16"
  }
}
```

### Get Team Logo
```
GET http://YOUR_SERVER_IP:5000/nhl/logo/<TEAM_ABBREV>
```

Returns the team logo as SVG image.

### Health Check
```
GET http://YOUR_SERVER_IP:5000/health
```

Returns server status and cache information.

## Configuring the ESP32

Once the proxy server is running, you need to update the ESP32 firmware to use it:

1. Find your proxy server's IP address (e.g., `192.168.1.100`)
2. Update `firmware/src/widgets/nhlwidget/NHLWidget.h`:
   ```cpp
   #define NHL_API_BASE_URL "http://192.168.1.100:5000/nhl"
   ```
3. Update `firmware/src/widgets/nhlwidget/NHLWidget.cpp` to use the new endpoints:
   - Change `/standings/now` to `/team/UTA`
   - Remove the separate schedule fetch (it's now combined)
   - Update logo URL to `/logo/UTA`

4. Re-enable the NHL widget in `firmware/config/config.system.h`:
   ```cpp
   #define INCLUDE_NHL WIDGET_ON
   ```

5. Rebuild and upload the firmware

## Features

- **Caching**: Data is cached for 15 minutes to reduce API calls
- **Filtering**: Only sends the data needed for your team (~1-2KB vs 15KB)
- **HTTP**: No SSL overhead on the ESP32
- **Error Handling**: Returns cached data if NHL API is unavailable
- **Logging**: Detailed logs for debugging

## Troubleshooting

### Server won't start
- Check if port 5000 is already in use: `lsof -i :5000`
- Try a different port by changing the last line in the script

### ESP32 can't connect
- Verify the server IP address is correct
- Check firewall settings (allow port 5000)
- Ensure both devices are on the same network
- Test the endpoint in a browser: `http://YOUR_IP:5000/nhl/team/UTA`

### Data not updating
- Check server logs for errors
- Verify NHL API is accessible: `curl https://api-web.nhle.com/v1/standings/now`
- Clear cache by restarting the proxy server

## Team Abbreviations

Common NHL team abbreviations:
- ANA (Anaheim Ducks)
- BOS (Boston Bruins)
- BUF (Buffalo Sabres)
- CGY (Calgary Flames)
- CAR (Carolina Hurricanes)
- CHI (Chicago Blackhawks)
- COL (Colorado Avalanche)
- CBJ (Columbus Blue Jackets)
- DAL (Dallas Stars)
- DET (Detroit Red Wings)
- EDM (Edmonton Oilers)
- FLA (Florida Panthers)
- LAK (Los Angeles Kings)
- MIN (Minnesota Wild)
- MTL (Montreal Canadiens)
- NSH (Nashville Predators)
- NJD (New Jersey Devils)
- NYI (New York Islanders)
- NYR (New York Rangers)
- OTT (Ottawa Senators)
- PHI (Philadelphia Flyers)
- PIT (Pittsburgh Penguins)
- SJS (San Jose Sharks)
- SEA (Seattle Kraken)
- STL (St. Louis Blues)
- TBL (Tampa Bay Lightning)
- TOR (Toronto Maple Leafs)
- UTA (Utah Hockey Club)
- VAN (Vancouver Canucks)
- VGK (Vegas Golden Knights)
- WSH (Washington Capitals)
- WPG (Winnipeg Jets)
