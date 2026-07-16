#!/usr/bin/env python3
"""
Simple NHL logo downloader - downloads PNG logos and converts to JPG.
This version doesn't require cairosvg and uses direct PNG downloads.
"""

import requests
import os
from PIL import Image
from io import BytesIO

# NHL team abbreviations (all 32 teams)
NHL_TEAMS = [
    "ANA", "BOS", "BUF", "CAR", "CBJ", "CGY", "CHI", "COL", "DAL", "DET",
    "EDM", "FLA", "LAK", "MIN", "MTL", "NJD", "NSH", "NYI", "NYR", "OTT",
    "PHI", "PIT", "SEA", "SJS", "STL", "TBL", "TOR", "UTA", "VAN", "VGK",
    "WPG", "WSH"
]

def download_and_convert_logo(team_abbrev, output_dir="images/nhl", size=(200, 200)):
    """
    Download NHL team logo as PNG and convert to JPG format.
    
    Args:
        team_abbrev: Team abbreviation (e.g., "COL", "BOS")
        output_dir: Directory to save the logo
        size: Tuple of (width, height) to resize logo
    """
    # ESPN uses different abbreviations for some teams
    espn_abbrev_map = {
        "LAK": "LA",
        "SJS": "SJ",
        "TBL": "TB"
    }
    espn_abbrev = espn_abbrev_map.get(team_abbrev, team_abbrev)
    
    # Try multiple logo sources
    logo_urls = [
        # NHL assets - try different formats
        f"https://assets.nhle.com/logos/nhl/svg/{team_abbrev}_light.svg",
        f"https://a.espncdn.com/i/teamlogos/nhl/500/{espn_abbrev}.png",
        f"https://a.espncdn.com/combiner/i?img=/i/teamlogos/nhl/500/{espn_abbrev}.png&h=200&w=200",
    ]
    
    output_path = os.path.join(output_dir, f"{team_abbrev}.jpg")
    
    print(f"Downloading logo for {team_abbrev}...")
    
    for logo_url in logo_urls:
        try:
            response = requests.get(logo_url, timeout=10)
            
            if response.status_code == 200:
                # Open the image
                img = Image.open(BytesIO(response.content))
                
                # Resize to target size
                img = img.resize(size, Image.Resampling.LANCZOS)
                
                # Convert to RGB (JPG doesn't support transparency)
                if img.mode in ('RGBA', 'LA', 'P'):
                    # Create black background for dark theme
                    background = Image.new('RGB', img.size, (0, 0, 0))
                    if img.mode == 'P':
                        img = img.convert('RGBA')
                    # Paste image on black background
                    if img.mode == 'RGBA':
                        background.paste(img, mask=img.split()[-1])
                    else:
                        background.paste(img)
                    img = background
                else:
                    img = img.convert('RGB')
                
                # Save as JPG
                img.save(output_path, 'JPEG', quality=85, optimize=True)
                print(f"  ✓ Saved {team_abbrev}.jpg ({os.path.getsize(output_path)} bytes)")
                return True
                
        except Exception as e:
            # Try next URL
            continue
    
    print(f"  ✗ Failed to download logo from any source")
    return False

def main():
    """Download all NHL team logos."""
    output_dir = "images/nhl"
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    print("=" * 60)
    print("NHL Logo Downloader (Simple Version)")
    print("=" * 60)
    print(f"Output directory: {output_dir}")
    print(f"Total teams: {len(NHL_TEAMS)}")
    print("=" * 60)
    print()
    
    # Check if PIL is available
    try:
        from PIL import Image
        print("✓ Pillow (PIL) is installed")
    except ImportError:
        print("✗ Pillow (PIL) is NOT installed")
        print("  Install it with: pip install Pillow")
        return
    
    print()
    
    # Download logos
    success_count = 0
    failed_teams = []
    total_size = 0
    
    for team in NHL_TEAMS:
        if download_and_convert_logo(team, output_dir):
            success_count += 1
            logo_path = os.path.join(output_dir, f"{team}.jpg")
            if os.path.exists(logo_path):
                total_size += os.path.getsize(logo_path)
        else:
            failed_teams.append(team)
    
    # Summary
    print()
    print("=" * 60)
    print(f"Download complete!")
    print(f"  Successful: {success_count}/{len(NHL_TEAMS)}")
    print(f"  Total size: {total_size / 1024:.1f} KB")
    if failed_teams:
        print(f"  Failed: {', '.join(failed_teams)}")
    print("=" * 60)
    
    # Instructions
    print()
    print("Next steps:")
    print("1. Verify the logos in the images/nhl/ directory")
    print("2. Build and upload your firmware - logos will be automatically")
    print("   included in the ESP32 filesystem when INCLUDE_NHL is enabled")
    print("3. Configure your team abbreviation in the NHL widget settings")
    print()

if __name__ == "__main__":
    main()
