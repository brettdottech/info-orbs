#!/usr/bin/env python3
"""
Test script to examine NHL API responses and compare with what the code expects.
"""

import requests
import json
from datetime import datetime

def test_schedule_api(team_abbrev="UTA"):
    """Test the schedule API endpoint that the code currently uses."""
    print("=" * 80)
    print(f"Testing Schedule API for {team_abbrev}")
    print("=" * 80)
    
    url = f"https://api-web.nhle.com/v1/club-schedule-season/{team_abbrev}/now"
    print(f"\nURL: {url}\n")
    
    response = requests.get(url)
    print(f"Status Code: {response.status_code}")
    
    if response.status_code == 200:
        data = response.json()
        print(f"\nTop-level keys in response:")
        for key in data.keys():
            print(f"  - {key}")
        
        print(f"\nSample data structure:")
        print(f"  previousSeason: {data.get('previousSeason')}")
        print(f"  currentSeason: {data.get('currentSeason')}")
        print(f"  clubTimezone: {data.get('clubTimezone')}")
        print(f"  Number of games: {len(data.get('games', []))}")
        
        # Check what the code expects but isn't in the response
        expected_fields = ['teamId', 'team', 'record', 'standings', 'lastGame', 'nextGame', 'lastTen']
        print(f"\nFields the code expects (but are NOT in this response):")
        for field in expected_fields:
            if field not in data:
                print(f"  ❌ {field}")
        
        # Show first game structure
        if data.get('games'):
            print(f"\nFirst game structure (truncated):")
            first_game = data['games'][0]
            print(f"  id: {first_game.get('id')}")
            print(f"  gameDate: {first_game.get('gameDate')}")
            print(f"  gameState: {first_game.get('gameState')}")
            print(f"  homeTeam: {first_game.get('homeTeam', {}).get('abbrev')}")
            print(f"  awayTeam: {first_game.get('awayTeam', {}).get('abbrev')}")
            print(f"  homeScore: {first_game.get('homeTeam', {}).get('score')}")
            print(f"  awayScore: {first_game.get('awayTeam', {}).get('score')}")
    
    return response.json() if response.status_code == 200 else None


def test_standings_api(team_abbrev="UTA"):
    """Test the standings API endpoint."""
    print("\n" + "=" * 80)
    print(f"Testing Standings API for {team_abbrev}")
    print("=" * 80)
    
    url = "https://api-web.nhle.com/v1/standings/now"
    print(f"\nURL: {url}\n")
    
    response = requests.get(url)
    print(f"Status Code: {response.status_code}")
    
    if response.status_code == 200:
        data = response.json()
        
        # Find the team
        team_data = None
        for team in data.get('standings', []):
            if team.get('teamAbbrev', {}).get('default') == team_abbrev:
                team_data = team
                break
        
        if team_data:
            print(f"\nTeam data found!")
            print(f"  Team Name: {team_data.get('teamName', {}).get('default')}")
            print(f"  Common Name: {team_data.get('teamCommonName', {}).get('default')}")
            print(f"  Division: {team_data.get('divisionName')}")
            print(f"  Division Rank: {team_data.get('divisionSequence')}")
            print(f"  Conference: {team_data.get('conferenceName')}")
            print(f"  Points: {team_data.get('points')}")
            print(f"  Wins: {team_data.get('wins')}")
            print(f"  Losses: {team_data.get('losses')}")
            print(f"  OT Losses: {team_data.get('otLosses')}")
            print(f"  Win %: {team_data.get('winPctg')}")
            print(f"  Last 10 Record: {team_data.get('l10Wins')}-{team_data.get('l10Losses')}-{team_data.get('l10OtLosses')}")
            print(f"  Team Logo: {team_data.get('teamLogo')}")
            
            print(f"\nAll available fields:")
            for key in sorted(team_data.keys()):
                print(f"  - {key}")
        else:
            print(f"\n❌ Team {team_abbrev} not found in standings")
    
    return response.json() if response.status_code == 200 else None


def analyze_code_expectations():
    """Analyze what the code expects vs what the API provides."""
    print("\n" + "=" * 80)
    print("ANALYSIS: Code Expectations vs API Reality")
    print("=" * 80)
    
    print("\nThe code in NHLWidget.cpp (lines 89-149) expects this JSON structure:")
    print("""
    {
      "teamId": <int>,
      "season": <string>,
      "team": {
        "fullName": <string>,
        "shortName": <string>,
        "colors": [{"name": <string>, "code": <string>}],
        "logoUrl": <string>,
        "logoImageFileName": <string>,
        "logoBackgroundColor": <string>
      },
      "record": <string>,
      "standings": {
        "division": <string>,
        "divisionRank": <string>,
        "winningPercentage": <string>,
        "points": <string>
      },
      "lastGame": {
        "date": <string>,
        "day": <string>,
        "opponent": <string>,
        "score": <string>,
        "result": <string>,
        "gameTime": <string>
      },
      "lastTen": {
        "record": <string>
      },
      "nextGame": {
        "date": <string>,
        "day": <string>,
        "opponent": <string>,
        "location": <string>,
        "gameTime": <string>,
        "tvBroadcast": <string>
      }
    }
    """)
    
    print("\n❌ PROBLEM: The /v1/club-schedule-season/{team}/now endpoint does NOT return this structure!")
    print("   It only returns a 'games' array with individual game objects.")
    
    print("\n✅ SOLUTION: The code needs to:")
    print("   1. Call /v1/standings/now to get team info and standings")
    print("   2. Call /v1/club-schedule-season/{team}/now to get schedule")
    print("   3. Parse the games array to find last/next games")
    print("   4. Combine the data into the expected structure")


if __name__ == "__main__":
    # Test both APIs
    schedule_data = test_schedule_api("UTA")
    standings_data = test_standings_api("UTA")
    
    # Analyze the issue
    analyze_code_expectations()
    
    print("\n" + "=" * 80)
    print("CONCLUSION")
    print("=" * 80)
    print("\nThe NHL widget code is NOT processing the API correctly because:")
    print("1. It's calling the wrong endpoint (or the endpoint structure changed)")
    print("2. The expected JSON structure doesn't match the actual API response")
    print("3. The code needs to be updated to parse the actual API structure")
    print("\nThe API is working and returning data, but the parsing logic needs to be fixed.")
    print("=" * 80)
