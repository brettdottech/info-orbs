# NHL Team Logos

This directory contains NHL team logos in JPG format for use with the NHL widget.

## Downloading Logos

To download all NHL team logos, run the provided Python script:

```bash
python3 scripts/download_nhl_logos.py
```

### Prerequisites

The script requires the following Python packages:
- `requests` - for downloading files
- `Pillow` (PIL) - for image processing
- `cairosvg` - for converting SVG to PNG/JPG

Install them with:
```bash
pip install requests Pillow cairosvg
```

**Note:** On macOS, you may also need to install cairo:
```bash
brew install cairo
```

## Manual Download Alternative

If you prefer to download logos manually or the script doesn't work, you can:

1. Visit the NHL team pages or use a logo repository
2. Download team logos as PNG or JPG files
3. Resize them to approximately 200x200 pixels
4. Save them in this directory with the naming convention: `{TEAM_ABBREV}.jpg`

For example:
- `COL.jpg` - Colorado Avalanche
- `BOS.jpg` - Boston Bruins
- `TOR.jpg` - Toronto Maple Leafs

## Team Abbreviations

The following team abbreviations are used:

| Abbreviation | Team Name |
|--------------|-----------|
| ANA | Anaheim Ducks |
| BOS | Boston Bruins |
| BUF | Buffalo Sabres |
| CAR | Carolina Hurricanes |
| CBJ | Columbus Blue Jackets |
| CGY | Calgary Flames |
| CHI | Chicago Blackhawks |
| COL | Colorado Avalanche |
| DAL | Dallas Stars |
| DET | Detroit Red Wings |
| EDM | Edmonton Oilers |
| FLA | Florida Panthers |
| LAK | Los Angeles Kings |
| MIN | Minnesota Wild |
| MTL | Montreal Canadiens |
| NJD | New Jersey Devils |
| NSH | Nashville Predators |
| NYI | New York Islanders |
| NYR | New York Rangers |
| OTT | Ottawa Senators |
| PHI | Philadelphia Flyers |
| PIT | Pittsburgh Penguins |
| SEA | Seattle Kraken |
| SJS | San Jose Sharks |
| STL | St. Louis Blues |
| TBL | Tampa Bay Lightning |
| TOR | Toronto Maple Leafs |
| UTA | Utah Hockey Club |
| VAN | Vancouver Canucks |
| VGK | Vegas Golden Knights |
| WPG | Winnipeg Jets |
| WSH | Washington Capitals |

## How It Works

1. When you build the firmware with `INCLUDE_NHL == WIDGET_ON`, the build system automatically copies all `.jpg` files from this directory to the ESP32's LittleFS filesystem under `/nhl/`

2. The NHL widget loads the logo from the filesystem at `/nhl/{TEAM_ABBREV}.jpg` based on your configured team abbreviation

3. This eliminates the need to fetch logos over the network and convert SVG files on the fly

## Benefits

- **Faster loading** - No network requests needed for logos
- **More reliable** - Works even if the logo API is down
- **Better quality** - Pre-processed images optimized for the display
- **Lower memory usage** - No need to convert SVG to JPG in memory
- **Offline operation** - Logos available immediately without internet

## File Size Considerations

The ESP32 has limited flash storage. Each logo should be:
- Format: JPG (better compression than PNG)
- Size: ~200x200 pixels
- Quality: 85% (good balance of quality and file size)
- Expected file size: 5-15 KB per logo

All 32 team logos should take approximately 200-400 KB of storage.
