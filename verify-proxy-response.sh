#!/bin/bash
# Script to verify the proxy server is returning the new playoff fields

echo "=========================================="
echo "Verifying NHL Proxy Server Response"
echo "=========================================="
echo ""

# Check if proxy server is running
echo "1. Testing proxy server connection..."
PROXY_URL="http://localhost:5000/nhl/team/UTA"

if curl -s -f "$PROXY_URL" > /dev/null 2>&1; then
    echo "   ✓ Proxy server is responding"
else
    echo "   ✗ Proxy server is NOT responding at $PROXY_URL"
    echo ""
    echo "   Please start the proxy server with:"
    echo "   python3 nhl-proxy-server.py"
    exit 1
fi

echo ""
echo "2. Checking for playoff position fields..."
RESPONSE=$(curl -s "$PROXY_URL")

# Check for each required field
if echo "$RESPONSE" | grep -q '"conferenceName"'; then
    CONF_NAME=$(echo "$RESPONSE" | grep -o '"conferenceName"[^,]*' | cut -d'"' -f4)
    echo "   ✓ conferenceName: $CONF_NAME"
else
    echo "   ✗ conferenceName: MISSING"
fi

if echo "$RESPONSE" | grep -q '"conferenceSequence"'; then
    CONF_RANK=$(echo "$RESPONSE" | grep -o '"conferenceSequence"[^,]*' | cut -d':' -f2 | tr -d ' ,')
    echo "   ✓ conferenceSequence: $CONF_RANK"
else
    echo "   ✗ conferenceSequence: MISSING - RESTART PROXY SERVER!"
fi

if echo "$RESPONSE" | grep -q '"wildcardSequence"'; then
    WC_RANK=$(echo "$RESPONSE" | grep -o '"wildcardSequence"[^,]*' | cut -d':' -f2 | tr -d ' ,')
    echo "   ✓ wildcardSequence: $WC_RANK"
else
    echo "   ✗ wildcardSequence: MISSING - RESTART PROXY SERVER!"
fi

echo ""
echo "=========================================="
echo "If any fields are MISSING, restart the proxy server:"
echo "  1. Stop the current proxy server (Ctrl+C)"
echo "  2. Run: python3 nhl-proxy-server.py"
echo "  3. Re-upload firmware to ESP32"
echo "=========================================="
