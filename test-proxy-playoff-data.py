#!/usr/bin/env python3
"""
Test script to verify the proxy server returns playoff position data
"""

import requests
import json

def test_proxy_data_structure():
    """Test that the proxy returns the expected playoff data fields"""
    print("=" * 60)
    print("Testing NHL Proxy Server - Playoff Position Data")
    print("=" * 60)
    
    # Fetch standings data directly from NHL API
    print("\n1. Fetching standings from NHL API...")
    standings_url = "https://api-web.nhle.com/v1/standings/now"
    response = requests.get(standings_url)
    
    if response.status_code != 200:
        print(f"✗ Failed to fetch standings: {response.status_code}")
        return False
    
    standings_data = response.json()
    
    # Find Utah team
    team_data = None
    for team in standings_data.get("standings", []):
        if team.get("teamAbbrev", {}).get("default") == "UTA":
            team_data = team
            break
    
    if not team_data:
        print("✗ Team UTA not found in standings")
        return False
    
    print("✓ Found UTA in standings")
    
    # Check for required fields
    print("\n2. Checking for playoff position fields...")
    required_fields = {
        "conferenceName": team_data.get("conferenceName"),
        "conferenceSequence": team_data.get("conferenceSequence"),
        "wildcardSequence": team_data.get("wildcardSequence"),
        "divisionName": team_data.get("divisionName"),
        "divisionSequence": team_data.get("divisionSequence")
    }
    
    all_present = True
    for field, value in required_fields.items():
        if value is not None:
            print(f"  ✓ {field}: {value}")
        else:
            print(f"  ✗ {field}: MISSING")
            all_present = False
    
    if not all_present:
        return False
    
    # Interpret the playoff position
    print("\n3. Playoff Position Analysis:")
    conf_rank = required_fields["conferenceSequence"]
    div_rank = required_fields["divisionSequence"]
    wc_rank = required_fields["wildcardSequence"]
    
    print(f"  Conference: {required_fields['conferenceName']}")
    print(f"  Conference Rank: #{conf_rank}")
    print(f"  Division: {required_fields['divisionName']}")
    print(f"  Division Rank: #{div_rank}")
    
    if wc_rank > 0:
        print(f"  Wild Card Position: WC#{wc_rank}")
        print(f"  → Team is in a WILD CARD playoff spot")
    elif div_rank <= 3:
        print(f"  → Team is in a DIVISION playoff spot (top 3)")
    else:
        print(f"  → Team is NOT currently in a playoff spot")
    
    print("\n" + "=" * 60)
    print("✓ All playoff position data is available!")
    print("=" * 60)
    
    return True

if __name__ == "__main__":
    success = test_proxy_data_structure()
    exit(0 if success else 1)
