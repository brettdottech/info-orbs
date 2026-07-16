#!/usr/bin/env python3
"""
Download NHL team logos and convert them to JPG format for ESP32 storage.
This script fetches logos from the NHL API and saves them locally.
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
    Download NHL team logo and convert to JPG format.
    
    Args:
        team_abbrev: Team abbreviation (e.g., "COL", "BOS")
        output_dir: Directory to save the logo
        size: Tuple of (width, height) to resize logo
    """
    # NHL logo URL pattern (using assets.nhle.com)
    logo_url = f"https://assets.nhle.com/logos/nhl/svg/{team_abbrev}_light.svg"
    
    # Alternative: Try the dark version if light doesn't work
    alt_logo_url = f"https://assets.nhle.com/logos/nhl/svg/{team_abbrev}_dark.svg"
    
    output_path = os.path.join(output_dir, f"{team_abbrev}.jpg")
    
    print(f"Downloading logo for {team_abbrev}...")
    
    try:
        # Try to download the logo
        response = requests.get(logo_url, timeout=10)
        
        if response.status_code != 200:
            print(f"  Light logo not found, trying dark version...")
            response = requests.get(alt_logo_url, timeout=10)
        
        if response.status_code == 200:
            # For SVG files, we need to convert them
            # Since PIL doesn't handle SVG directly, we'll use cairosvg if available
            try:
                import cairosvg
                
                # Convert SVG to PNG in memory
                png_data = cairosvg.svg2png(bytestring=response.content, 
                                           output_width=size[0], 
                                           output_height=size[1])
                
                # Open PNG and convert to RGB (JPG doesn't support transparency)
                img = Image.open(BytesIO(png_data))
                
                # Create white background
                if img.mode in ('RGBA', 'LA', 'P'):
                    background = Image.new('RGB', img.size, (255, 255, 255))
                    if img.mode == 'P':
                        img = img.convert('RGBA')
                    background.paste(img, mask=img.split()[-1] if img.mode == 'RGBA' else None)
                    img = background
                else:
                    img = img.convert('RGB')
                
                # Save as JPG
                img.save(output_path, 'JPEG', quality=85, optimize=True)
                print(f"  ✓ Saved {team_abbrev}.jpg")
                return True
                
            except ImportError:
                print(f"  ✗ cairosvg not installed. Install with: pip install cairosvg")
                print(f"    Alternatively, manually download PNG logos instead of SVG")
                return False
        else:
            print(f"  ✗ Failed to download logo (HTTP {response.status_code})")
            return False
            
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def main():
    """Download all NHL team logos."""
    output_dir = "images/nhl"
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    print("=" * 60)
    print("NHL Logo Downloader")
    print("=" * 60)
    print(f"Output directory: {output_dir}")
    print(f"Total teams: {len(NHL_TEAMS)}")
    print("=" * 60)
    print()
    
    # Check if cairosvg is available
    try:
        import cairosvg
        print("✓ cairosvg is installed - SVG conversion available")
    except ImportError:
        print("✗ cairosvg is NOT installed")
        print("  Install it with: pip install cairosvg")
        print("  Or manually download PNG versions of logos")
        print()
        response = input("Continue anyway? (y/n): ")
        if response.lower() != 'y':
            return
    
    print()
    
    # Download logos
    success_count = 0
    failed_teams = []
    
    for team in NHL_TEAMS:
        if download_and_convert_logo(team, output_dir):
            success_count += 1
        else:
            failed_teams.append(team)
    
    # Summary
    print()
    print("=" * 60)
    print(f"Download complete!")
    print(f"  Successful: {success_count}/{len(NHL_TEAMS)}")
    if failed_teams:
        print(f"  Failed: {', '.join(failed_teams)}")
    print("=" * 60)
    
    # Instructions
    print()
    print("Next steps:")
    print("1. Verify the logos in the images/nhl/ directory")
    print("2. The logos will be automatically included in the ESP32 filesystem")
    print("3. Update your NHL widget configuration to use your team abbreviation")
    print()

if __name__ == "__main__":
    main()
