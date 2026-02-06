#!/usr/bin/env python3
"""
NHL API Proxy Server for ESP32 InfoOrbs
This lightweight proxy fetches NHL data and serves it over HTTP with reduced payload size.
"""

from flask import Flask, jsonify, request
import requests
from datetime import datetime, timedelta
import logging

app = Flask(__name__)
logging.basicConfig(level=logging.INFO)

# Cache configuration
CACHE_DURATION = timedelta(minutes=15)
cache = {}

NHL_API_BASE = "https://api-web.nhle.com/v1"

def get_cached_or_fetch(url, cache_key):
    """Get data from cache or fetch from NHL API"""
    now = datetime.now()
    
    if cache_key in cache:
        data, timestamp = cache[cache_key]
        if now - timestamp < CACHE_DURATION:
            logging.info(f"Cache hit for {cache_key}")
            return data
    
    logging.info(f"Fetching from NHL API: {url}")
    try:
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        data = response.json()
        cache[cache_key] = (data, now)
        return data
    except Exception as e:
        logging.error(f"Error fetching from NHL API: {e}")
        # Return cached data even if expired, if available
        if cache_key in cache:
            logging.info(f"Returning expired cache for {cache_key}")
            return cache[cache_key][0]
        raise

def filter_team_data(standings_data, team_abbrev):
    """Extract only the data for the specified team"""
    standings = standings_data.get("standings", [])
    
    for team in standings:
        if team.get("teamAbbrev", {}).get("default") == team_abbrev:
            return {
                "teamId": team.get("teamId"),
                "teamAbbrev": team.get("teamAbbrev", {}).get("default"),
                "teamName": team.get("teamName", {}).get("default"),
                "teamCommonName": team.get("teamCommonName", {}).get("default"),
                "teamLogo": team.get("teamLogo"),
                "divisionName": team.get("divisionName"),
                "divisionSequence": team.get("divisionSequence"),
                "conferenceName": team.get("conferenceName"),
                "conferenceSequence": team.get("conferenceSequence"),
                "wildcardSequence": team.get("wildcardSequence"),
                "points": team.get("points"),
                "winPctg": team.get("winPctg"),
                "wins": team.get("wins"),
                "losses": team.get("losses"),
                "otLosses": team.get("otLosses"),
                "l10Wins": team.get("l10Wins"),
                "l10Losses": team.get("l10Losses"),
                "l10OtLosses": team.get("l10OtLosses"),
                "seasonId": team.get("seasonId")
            }
    
    return None

def filter_schedule_data(schedule_data, team_abbrev):
    """Extract last and next game data"""
    games = schedule_data.get("games", [])
    
    last_game = None
    next_game = None
    
    for game in games:
        game_state = game.get("gameState")
        
        # Keep updating last_game to get the most recent completed game
        if game_state in ["FINAL", "OFF"]:
            last_game = game
        
        # Get the first upcoming game
        if not next_game and game_state in ["FUT", "LIVE", "CRIT", "PRE"]:
            next_game = game
    
    result = {}
    
    if last_game:
        home_team = last_game.get("homeTeam", {})
        away_team = last_game.get("awayTeam", {})
        is_home = home_team.get("abbrev") == team_abbrev
        
        result["lastGame"] = {
            "gameDate": last_game.get("gameDate"),
            "startTimeUTC": last_game.get("startTimeUTC"),
            "opponent": away_team.get("commonName", {}).get("default") if is_home else home_team.get("commonName", {}).get("default"),
            "isHome": is_home,
            "ourScore": home_team.get("score") if is_home else away_team.get("score"),
            "theirScore": away_team.get("score") if is_home else home_team.get("score")
        }
    
    if next_game:
        home_team = next_game.get("homeTeam", {})
        away_team = next_game.get("awayTeam", {})
        is_home = home_team.get("abbrev") == team_abbrev
        tv_broadcasts = next_game.get("tvBroadcasts", [])
        
        # Log all available broadcasts for debugging
        logging.info(f"TV Broadcasts for {team_abbrev} (isHome={is_home}):")
        for i, broadcast in enumerate(tv_broadcasts):
            logging.info(f"  [{i}] network={broadcast.get('network')}, market={broadcast.get('market')}, countryCode={broadcast.get('countryCode')}")
        
        # Filter TV broadcasts to prioritize Utah's local station, then national broadcasts
        # Avoid showing opponent's local broadcasts
        selected_broadcast = "TBD"
        if tv_broadcasts:
            # Priority 1: Look for Utah's local station (Utah16, UT 16, etc.)
            for broadcast in tv_broadcasts:
                network = broadcast.get("network", "")
                market = broadcast.get("market", "")
                # Match Utah16, UT 16, or similar variations
                # For Utah team, look for their broadcast regardless of home/away market designation
                if "utah" in network.lower() or network == "UT 16":
                    selected_broadcast = network
                    logging.info(f"✓ Selected {network} (Priority 1 - Utah local)")
                    break
            
            # Priority 2: If Utah local not found, look for national broadcasts
            if selected_broadcast == "TBD":
                for broadcast in tv_broadcasts:
                    network = broadcast.get("network", "")
                    market = broadcast.get("market", "")
                    
                    # National broadcasts (ESPN+, Hulu, TNT, Max, NHLN, etc.)
                    if market == "N":
                        selected_broadcast = network
                        logging.info(f"✓ Selected {network} (Priority 2 - National)")
                        break
            
            # Priority 3: If no national broadcast, use our team's broadcast (H if home, A if away)
            if selected_broadcast == "TBD":
                our_market = "H" if is_home else "A"
                for broadcast in tv_broadcasts:
                    network = broadcast.get("network", "")
                    market = broadcast.get("market", "")
                    
                    if market == our_market:
                        selected_broadcast = network
                        logging.info(f"✓ Selected {network} (Priority 3 - Our team {our_market})")
                        break
            
            # Last resort: if still nothing, just show first available
            if selected_broadcast == "TBD" and tv_broadcasts:
                selected_broadcast = tv_broadcasts[0].get("network", "TBD")
                logging.info(f"✓ Selected {selected_broadcast} (Last resort - first available)")
        
        logging.info(f"Final selected broadcast: {selected_broadcast}")
        
        result["nextGame"] = {
            "gameDate": next_game.get("gameDate"),
            "startTimeUTC": next_game.get("startTimeUTC"),
            "opponent": away_team.get("commonName", {}).get("default") if is_home else home_team.get("commonName", {}).get("default"),
            "isHome": is_home,
            "tvBroadcast": selected_broadcast
        }
    
    return result

@app.route('/nhl/team/<team_abbrev>', methods=['GET'])
def get_team_data(team_abbrev):
    """Get combined team data (standings + schedule)"""
    try:
        team_abbrev = team_abbrev.upper()
        
        # Fetch standings
        standings_url = f"{NHL_API_BASE}/standings/now"
        standings_data = get_cached_or_fetch(standings_url, "standings")
        team_info = filter_team_data(standings_data, team_abbrev)
        
        if not team_info:
            return jsonify({"error": f"Team {team_abbrev} not found"}), 404
        
        # Fetch schedule
        schedule_url = f"{NHL_API_BASE}/club-schedule-season/{team_abbrev}/now"
        schedule_data = get_cached_or_fetch(schedule_url, f"schedule_{team_abbrev}")
        
        # Parse last and next game from schedule
        game_info = filter_schedule_data(schedule_data, team_abbrev)
        
        # Merge team info and game info into flat structure for ESP32
        result = {**team_info, **game_info}
        
        return jsonify(result)
    
    except Exception as e:
        logging.error(f"Error processing request: {e}")
        return jsonify({"error": str(e)}), 500

@app.route('/nhl/logo/<team_abbrev>', methods=['GET'])
def get_team_logo(team_abbrev):
    """Proxy team logo image
    
    TODO: SVG to JPG/PNG Conversion Enhancement
    The NHL API provides SVG logos, but the ESP32 drawJpg() function only supports JPG format.
    This endpoint currently returns SVG content as-is, which cannot be rendered on the ESP32.
    
    Future enhancement should:
    1. Convert SVG to JPG or PNG format before returning to ESP32
    2. Add dependencies: cairosvg or PIL (Pillow) for image conversion
    3. Consider caching converted images to reduce processing overhead
    4. Add optional format parameter (e.g., ?format=jpg) for flexibility
    
    Example implementation approach:
    - Use cairosvg to convert SVG to PNG: cairosvg.svg2png(url=logo_url, write_to=output)
    - Use PIL to convert PNG to JPG if needed: Image.open(png_file).convert('RGB').save(jpg_file)
    - Return converted image with appropriate Content-Type header
    
    Dependencies to add to requirements.txt:
    - cairosvg>=2.7.0
    - Pillow>=10.0.0
    """
    try:
        team_abbrev = team_abbrev.upper()
        
        # Get logo URL from standings
        standings_url = f"{NHL_API_BASE}/standings/now"
        standings_data = get_cached_or_fetch(standings_url, "standings")
        team_info = filter_team_data(standings_data, team_abbrev)
        
        if not team_info or not team_info.get("teamLogo"):
            return jsonify({"error": "Logo not found"}), 404
        
        logo_url = team_info["teamLogo"]
        
        # Fetch and return logo
        response = requests.get(logo_url, timeout=10)
        response.raise_for_status()
        
        return response.content, 200, {'Content-Type': 'image/svg+xml'}
    
    except Exception as e:
        logging.error(f"Error fetching logo: {e}")
        return jsonify({"error": str(e)}), 500

@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        "status": "healthy",
        "cache_size": len(cache),
        "timestamp": datetime.now().isoformat()
    })

@app.route('/clear-cache', methods=['POST', 'GET'])
def clear_cache():
    """Clear the cache to force fresh data fetch"""
    global cache
    cache_size = len(cache)
    cache = {}
    logging.info(f"Cache cleared - removed {cache_size} entries")
    return jsonify({
        "status": "success",
        "message": f"Cache cleared - removed {cache_size} entries",
        "timestamp": datetime.now().isoformat()
    })

if __name__ == '__main__':
    print("=" * 60)
    print("NHL API Proxy Server for ESP32 InfoOrbs")
    print("=" * 60)
    print("\nEndpoints:")
    print("  GET /nhl/team/<TEAM_ABBREV>  - Get team data (e.g., /nhl/team/UTA)")
    print("  GET /nhl/logo/<TEAM_ABBREV>  - Get team logo (e.g., /nhl/logo/UTA)")
    print("  GET /health                  - Health check")
    print("\nStarting server on http://0.0.0.0:5000")
    print("=" * 60)
    
    app.run(host='0.0.0.0', port=5000, debug=False)
