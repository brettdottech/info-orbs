#include "Utils.h"
#include "config_helper.h"
#include <TFT_eSPI.h>
#include <cstring>

int Utils::getWrappedLines(String (&lines)[MAX_WRAPPED_LINES], String str, int limit) {
    char buf[str.length() + 1];
    char lineBuf[limit + 1];
    str.toCharArray(buf, str.length() + 1);

    char *p = buf;
    char *eol;
    int lineCount = 0;
    for (int i = 0; i < MAX_WRAPPED_LINES; i++) {
        if (p - buf > strlen(buf)) {
            lines[i] = "";
            continue;
        }
        eol = strchr(p, '\n');
        if (eol == NULL) {
            eol = p + strlen(p);
        }

        if (eol - p > limit) {
            eol = p + limit;
            while (*eol != ' ' && *eol != '\n' && eol > p) {
                eol--;
            }
        }
        strncpy(lineBuf, p, eol - p);
        lineBuf[eol - p] = '\0';

        lines[i] = String(lineBuf);
        lineCount++;
        p = eol + 1;
    }
    return lineCount;
}

String Utils::getWrappedLine(String str, int limit, int lineNum, int maxLines) {
    if (lineNum > maxLines) {
        return "";
    }
    char buf[str.length() + 1];
    char lineBuf[limit + 1];
    str.toCharArray(buf, str.length() + 1);

    String lines[maxLines];

    char *p = buf;
    char *eol;

    for (int i = 0; i < maxLines && i <= lineNum; i++) {
        if (p - buf > strlen(buf)) {
            lines[i] = "";
            continue;
        }
        eol = strchr(p, '\n');
        if (eol == NULL) {
            eol = p + strlen(p);
        }

        if (eol - p > limit) {
            eol = p + limit;
            while (*eol != ' ' && *eol != '\n' && eol > p) {
                eol--;
            }
        }
        strncpy(lineBuf, p, eol - p);
        lineBuf[eol - p] = '\0';

        lines[i] = String(lineBuf);
        p = eol + 1;
    }
    return lines[lineNum];
}

int32_t Utils::stringToColor(String color) {
    color.toLowerCase();
    color.replace(" ", "");
    if (color == "black") {
        return TFT_BLACK;
    } else if (color == "navy") {
        return TFT_NAVY;
    } else if (color == "darkgreen") {
        return TFT_DARKGREEN;
    } else if (color == "darkcyan") {
        return TFT_DARKCYAN;
    } else if (color == "maroon") {
        return TFT_MAROON;
    } else if (color == "purple") {
        return TFT_PURPLE;
    } else if (color == "olive") {
        return TFT_OLIVE;
    } else if (color == "lightgrey" || color == "grey") {
        return TFT_LIGHTGREY;
    } else if (color == "darkgrey") {
        return TFT_DARKGREY;
    } else if (color == "blue") {
        return TFT_BLUE;
    } else if (color == "green") {
        return TFT_GREEN;
    } else if (color == "cyan") {
        return TFT_CYAN;
    } else if (color == "red") {
        return TFT_RED;
    } else if (color == "magenta") {
        return TFT_MAGENTA;
    } else if (color == "yellow") {
        return TFT_YELLOW;
    } else if (color == "white") {
        return TFT_WHITE;
    } else if (color == "orange") {
        return TFT_ORANGE;
    } else if (color == "greenyellow") {
        return TFT_GREENYELLOW;
    } else if (color == "pink") {
        return TFT_PINK;
    } else if (color == "brown") {
        return TFT_BROWN;
    } else if (color == "gold") {
        return TFT_GOLD;
    } else if (color == "silver") {
        return TFT_SILVER;
    } else if (color == "skyblue") {
        return TFT_SKYBLUE;
    } else if (color == "vilolet") {
        return TFT_VIOLET;
    } else {
        Serial.print("Invalid color: ");
        Serial.println(color);
        return TFT_BLACK;
    }
}

String Utils::formatFloat(float value, int8_t digits) {
    char tmp[30] = {};
    dtostrf(value, 1, digits, tmp);
    return tmp;
}

int32_t Utils::stringToAlignment(String alignment) {
    alignment.toLowerCase();
    if (alignment.indexOf(" ") != -1) {
        alignment = alignment[0] + alignment.substring(alignment.indexOf(" ") + 1, 1);
    }
    alignment.replace(" ", "");
    if (alignment == "tl") {
        return TL_DATUM;
    } else if (alignment == "tc") {
        return TC_DATUM;
    } else if (alignment == "tr") {
        return TR_DATUM;
    } else if (alignment == "ml") {
        return ML_DATUM;
    } else if (alignment == "mc") {
        return MC_DATUM;
    } else if (alignment == "mr") {
        return MR_DATUM;
    } else if (alignment == "bl") {
        return BL_DATUM;
    } else if (alignment == "bc") {
        return BC_DATUM;
    } else if (alignment == "br") {
        return BR_DATUM;
    } else if (alignment == "cl") {
        return CL_DATUM;
    } else if (alignment == "cc") {
        return CC_DATUM;
    } else if (alignment == "cr") {
        return CR_DATUM;
    } else if (alignment == "bl") {
        return BL_DATUM;
    } else if (alignment == "bc") {
        return BC_DATUM;
    } else if (alignment == "br") {
        return BR_DATUM;
    } else if (alignment == "l") {
        return L_BASELINE;
    } else if (alignment == "c") {
        return C_BASELINE;
    } else if (alignment == "r") {
        return R_BASELINE;
    } else {
        return TL_DATUM;
    }
}

uint16_t Utils::rgb565dim(uint16_t rgb565, uint8_t brightness, bool swapBytes) {
    if (rgb565 == TFT_BLACK or brightness == 0) {
        return 0;
    }
    if (swapBytes) {
        // swap bytes
        rgb565 = (rgb565 >> 8) | (rgb565 << 8);
    }

    // Extract 5-bit Red, 6-bit Green, and 5-bit Blue components
    uint8_t r5 = (rgb565 >> 11) & 0x1F;
    uint8_t g6 = (rgb565 >> 5) & 0x3F;
    uint8_t b5 = rgb565 & 0x1F;

    // Scale brightness (0-255) to (0-32) and (0-64) ranges
    uint16_t r5_dim = (r5 * brightness + 127) / 255; // Round by adding 127
    uint16_t g6_dim = (g6 * brightness + 127) / 255;
    uint16_t b5_dim = (b5 * brightness + 127) / 255;

    // Recombine into RGB565 format
    uint16_t result = (r5_dim << 11) | (g6_dim << 5) | b5_dim;

    if (swapBytes) {
        // swap bytes
        result = (result >> 8) | (result << 8);
    }
    return result;
}

void Utils::rgb565dimBitmap(uint16_t *pixel565, size_t length, uint8_t brightness, bool swapBytes) {
    for (int i = 0; i < length; i++) {
        pixel565[i] = rgb565dim(pixel565[i], brightness, swapBytes);
    }
}

// Function to extract R, G, B from a 16-bit RGB565 pixel
uint32_t Utils::rgb565ToRgb888(uint16_t rgb565, bool swapBytes) {
    uint8_t r, g, b;

    if (swapBytes) {
        // Swap bytes
        rgb565 = (rgb565 >> 8) | (rgb565 << 8);
    }
    r = (rgb565 >> 11) & 0x1F; // Extract red (5 bits)
    g = (rgb565 >> 5) & 0x3F; // Extract green (6 bits)
    b = rgb565 & 0x1F; // Extract blue (5 bits)

    // Scale to 8 bits
    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    // Pack R, G, B into a single 32-bit color
    return (r << 16) | (g << 8) | b;
}

// Function to scale RGB888 color to RGB565 format
uint16_t Utils::rgb888ToRgb565(uint32_t rgb888, bool swapBytes) {
    uint8_t r = (rgb888 >> 16) & 0xFF;
    uint8_t g = (rgb888 >> 8) & 0xFF;
    uint8_t b = rgb888 & 0xFF;

    uint16_t pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

    if (swapBytes) {
        // Swap bytes
        pixel = (pixel >> 8) | (pixel << 8);
    }
    return pixel;
}

String Utils::rgb565ToRgb888html(int color565) {
    String hexColor = String(Utils::rgb565ToRgb888(color565), HEX); // Convert to hex and mask for 6 bytes
    while (hexColor.length() < 6) {
        hexColor = "0" + hexColor; // Add leading zeroes if needed
    }
    hexColor = "#" + hexColor; // Add the HTML color prefix
    return hexColor;
}

int Utils::rgb888htmlToRgb565(String hexColor) {
    if (hexColor.startsWith("#")) {
        hexColor = hexColor.substring(1); // Remove leading '#'
    }
    int rgb888 = strtol(hexColor.c_str(), 0, HEX); // Convert to RGB888 int
    int rgb565 = Utils::rgb888ToRgb565(rgb888); // Convert to RGB565
    return rgb565;
}

// Function to apply grayscale and map to target color
uint16_t Utils::grayscaleToTargetColor(uint8_t grayscale, uint8_t targetR8, uint8_t targetG8, uint8_t targetB8, float brightness, bool swapBytes) {
    // Apply brightness enhancement
    int scaledGrayscale = grayscale * brightness;
    if (scaledGrayscale > 255)
        scaledGrayscale = 255; // Clamp to 255

    // Map grayscale to target color
    uint8_t r = (targetR8 * scaledGrayscale) / 255;
    uint8_t g = (targetG8 * scaledGrayscale) / 255;
    uint8_t b = (targetB8 * scaledGrayscale) / 255;

    return rgb888ToRgb565((r << 16) | (g << 8) | b, swapBytes);
}

// Function to colorize image data
void Utils::colorizeImageData(uint16_t *pixels565, size_t length, uint32_t targetColor565, float brightness, bool swapBytes) {
    uint32_t targetColor888 = rgb565ToRgb888(targetColor565, false);

    // Extract target R, G, B
    uint8_t targetR8 = (targetColor888 >> 16) & 0xFF;
    uint8_t targetG8 = (targetColor888 >> 8) & 0xFF;
    uint8_t targetB8 = targetColor888 & 0xFF;

    for (size_t i = 0; i < length; i++) {
        // Convert RGB565 to RGB888
        uint32_t color888 = rgb565ToRgb888(pixels565[i], true);

        // Extract grayscale (approximate)
        uint8_t r = (color888 >> 16) & 0xFF;
        uint8_t g = (color888 >> 8) & 0xFF;
        uint8_t b = color888 & 0xFF;
        uint8_t grayscale = (r * 30 + g * 59 + b * 11) / 100;

        // Map grayscale to the target color
        pixels565[i] = grayscaleToTargetColor(grayscale, targetR8, targetG8, targetB8, brightness, swapBytes);
    }
}

const char *Utils::createConstCharBuffer(const std::string &originalString) {
    // Allocate enough memory for the string and the null-terminator
    char *buffer = new char[originalString.size() + 1];
    strcpy(buffer, originalString.c_str()); // Copy the string contents
    return buffer; // Return the pointer to this new string
}

const char *Utils::createConstCharBufferAndConcat(const char *prefix, const char *original, const char *postfix) {
    // Calculate lengths
    size_t prefixLen = strlen(prefix);
    size_t originalLen = strlen(original);
    size_t postfixLen = strlen(postfix);

    // Allocate memory for new string (including null terminator)
    size_t totalLen = prefixLen + originalLen + postfixLen + 1; // +1 for '\0'
    char *result = new char[totalLen];

    // Construct the new string
    strcpy(result, prefix); // Copy prefix
    strcat(result, original); // Append original string
    strcat(result, postfix); // Append postfix

    return result;
}

uint8_t Utils::stringToButtonId(const String &buttonName) {
    if (buttonName.equalsIgnoreCase("left")) {
        return BUTTON_LEFT;
    } else if (buttonName.equalsIgnoreCase("middle")) {
        return BUTTON_OK;
    } else if (buttonName.equalsIgnoreCase("right")) {
        return BUTTON_RIGHT;
    } else {
        return 0;
    }
}

ButtonState Utils::stringToButtonState(const String &buttonState) {
    if (buttonState.equalsIgnoreCase("short")) {
        return BTN_SHORT;
    } else if (buttonState.equalsIgnoreCase("medium")) {
        return BTN_MEDIUM;
    } else if (buttonState.equalsIgnoreCase("long")) {
        return BTN_LONG;
    } else {
        return BTN_NOTHING;
    }
}