#!/bin/bash

echo "================================================================================"
echo "NHL API Testing - Comparing Expected vs Actual Response"
echo "================================================================================"
echo ""

TEAM="UTA"

echo "1. Testing Schedule API (what the code currently calls)"
echo "   URL: https://api-web.nhle.com/v1/club-schedule-season/$TEAM/now"
echo "--------------------------------------------------------------------------------"

# Get schedule data and save to temp file
curl -L -s "https://api-web.nhle.com/v1/club-schedule-season/$TEAM/now" > /tmp/nhl-schedule.json

# Check what fields exist
echo "Top-level fields in response:"
python3 -c "import json; data=json.load(open('/tmp/nhl-schedule.json')); print('\n'.join(['  ✓ ' + k for k in data.keys()]))"

echo ""
echo "Fields the CODE EXPECTS (but are NOT in response):"
python3 -c "
import json
data = json.load(open('/tmp/nhl-schedule.json'))
expected = ['teamId', 'team', 'record', 'standings', 'lastGame', 'nextGame', 'lastTen']
for field in expected:
    if field not in data:
        print(f'  ✗ {field}')
"

echo ""
echo "Sample of what IS in the response:"
python3 -c "
import json
data = json.load(open('/tmp/nhl-schedule.json'))
print(f'  previousSeason: {data.get(\"previousSeason\")}')
print(f'  currentSeason: {data.get(\"currentSeason\")}')
print(f'  clubTimezone: {data.get(\"clubTimezone\")}')
print(f'  Number of games: {len(data.get(\"games\", []))}')
if data.get('games'):
    game = data['games'][0]
    print(f'  First game date: {game.get(\"gameDate\")}')
    print(f'  First game state: {game.get(\"gameState\")}')
"

echo ""
echo "================================================================================"
echo "2. Testing Standings API (provides team info)"
echo "   URL: https://api-web.nhle.com/v1/standings/now"
echo "--------------------------------------------------------------------------------"

curl -L -s "https://api-web.nhle.com/v1/standings/now" > /tmp/nhl-standings.json

python3 -c "
import json
data = json.load(open('/tmp/nhl-standings.json'))
team = next((t for t in data.get('standings', []) if t.get('teamAbbrev', {}).get('default') == 'UTA'), None)
if team:
    print('Team data found:')
    print(f'  Team Name: {team.get(\"teamName\", {}).get(\"default\")}')
    print(f'  Division: {team.get(\"divisionName\")}')
    print(f'  Division Rank: {team.get(\"divisionSequence\")}')
    print(f'  Points: {team.get(\"points\")}')
    print(f'  Record: {team.get(\"wins\")}-{team.get(\"losses\")}-{team.get(\"otLosses\")}')
    print(f'  Last 10: {team.get(\"l10Wins\")}-{team.get(\"l10Losses\")}-{team.get(\"l10OtLosses\")}')
    print(f'  Win %: {team.get(\"winPctg\")}')
    print(f'  Logo: {team.get(\"teamLogo\")}')
"

echo ""
echo "================================================================================"
echo "CONCLUSION"
echo "================================================================================"
echo ""
echo "❌ PROBLEM IDENTIFIED:"
echo "   The code expects a combined response with team info, standings, and games"
echo "   But the API endpoint only returns a games array!"
echo ""
echo "✅ THE API IS WORKING - it returns valid data"
echo "   The issue is that the code's parsing logic doesn't match the API structure"
echo ""
echo "📝 NEXT STEPS:"
echo "   The code needs to be updated to:"
echo "   1. Call both /standings/now AND /club-schedule-season/{team}/now"
echo "   2. Parse the games array to extract last/next game info"
echo "   3. Combine the data from both endpoints"
echo ""
echo "================================================================================"
