#include "DrawScript.h"
#include <map>
#include "Utils.h"

// Static command map definition
const std::map<std::string, DrawScript::CommandType> DrawScript::commandMap = {
    {"fill", COMMAND_FILL},
    {"f", COMMAND_FILL},
    {"line", COMMAND_LINE},
    {"l", COMMAND_LINE},
    {"circle", COMMAND_CIRCLE},
    {"circ", COMMAND_CIRCLE},
    {"c", COMMAND_CIRCLE},
    {"fcircle", COMMAND_FILL_CIRCLE},
    {"fcirc", COMMAND_FILL_CIRCLE},
    {"fc", COMMAND_FILL_CIRCLE},
    {"rectangle", COMMAND_RECTANGLE},
    {"rect", COMMAND_RECTANGLE},
    {"r", COMMAND_RECTANGLE},
    {"frectangle", COMMAND_FILL_RECTANGLE},
    {"frect", COMMAND_FILL_RECTANGLE},
    {"fr", COMMAND_FILL_RECTANGLE},
    {"triangle", COMMAND_TRIANGLE},
    {"tri", COMMAND_TRIANGLE},
    {"ftriangle", COMMAND_FILL_TRIANGLE},
    {"ftri", COMMAND_FILL_TRIANGLE},
    {"arc", COMMAND_ARC},
    {"sarc", COMMAND_SMOOTH_ARC},
    {"image", COMMAND_IMAGE},
    {"i", COMMAND_IMAGE},
    {"text", COMMAND_TEXT},
    {"t", COMMAND_TEXT},
    {"textc", COMMAND_TEXT_CENTERED},
    {"tc", COMMAND_TEXT_CENTERED},
    {"textf", COMMAND_TEXT_FITTED},
    {"tf", COMMAND_TEXT_FITTED},
    {"font", COMMAND_FONT},
    {"fsize", COMMAND_FONT_SIZE},
    {"fs", COMMAND_FONT_SIZE},
    {"falign", COMMAND_FONT_ALIGN},
    {"fa", COMMAND_FONT_ALIGN},
    {"fcolor", COMMAND_FONT_COLOR},
    {"fcol", COMMAND_FONT_COLOR},
    {"fbcolor", COMMAND_FONT_BACK_COLOR},
    {"fbcol", COMMAND_FONT_BACK_COLOR},
};

DrawScript::DrawScript(ScreenManager & manager) : m_manager(manager) {
}

// Function to parse and execute a command
void DrawScript::processDrawCommand(CommandType command, const std::vector<String>& p) {
    int pc = p.size() - 1;
    //printf(">>processing %s(%d) %d params\n", p[0], command, pc);
    switch (command) {
    case COMMAND_FILL:
        if (pc == 1) {
            m_manager.fillScreen(parseColor(p[1]));
        }
        break;
    case COMMAND_LINE:
        if (pc == 5) {
            m_manager.drawLine(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_CIRCLE:
        if (pc == 4) {
            m_manager.drawCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]));
        }
        break;
    case COMMAND_FILL_CIRCLE:
        if (pc == 4) {
            m_manager.fillCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]));
        }
        break;
    case COMMAND_RECTANGLE:
        if (pc == 5) {
            m_manager.drawRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_FILL_RECTANGLE:
        if (pc == 5) {
            m_manager.fillRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_TRIANGLE:
        if (pc == 7) {
            m_manager.drawTriangle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]));
        }
        break;
    case COMMAND_FILL_TRIANGLE:
        if (pc == 7) {
            m_manager.fillTriangle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]));
        }
        break;
    case COMMAND_ARC:
        if(pc == 7) {
            m_manager.drawArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), 0, false);
        } else if (pc == 8) {
            m_manager.drawArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), parseColor(p[8]),true);
        }
        break;
    case COMMAND_SMOOTH_ARC:
        if (pc == 9) {
            m_manager.drawSmoothArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), parseColor(p[8]), parseInt(p[9]));
        }
        break;
    case COMMAND_IMAGE:
        if (pc == 3) {
            m_manager.drawFsJpg(parseInt(p[1]), parseInt(p[2]), p[3].c_str());
        } else if (pc == 4) {
            m_manager.drawFsJpg(parseInt(p[1]), parseInt(p[2]), p[3].c_str(), parseInt(p[4]));
        } else if (pc == 5) {
            m_manager.drawFsJpg(parseInt(p[1]), parseInt(p[2]), p[3].c_str(), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_TEXT:
        switch (pc) {
        case 3:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]));
            break;
        case 5:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]));
            break;
        case 6:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]), parseColor(p[6]));
            break;
        case 7:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]), parseColor(p[6]), parseColor(p[7]));
            break;
        case 8:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]), parseColor(p[6]), parseColor(p[7]), parseInt(p[8]));
            break;
        }
        break;
    case COMMAND_TEXT_CENTERED:
        if (pc == 3) {
            m_manager.drawCentreString(p[1], parseInt(p[2]), parseInt(p[3]));
        } else if (pc == 4) {
            m_manager.drawCentreString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]));
        }
        break;
    case COMMAND_TEXT_FITTED:
        if (pc == 5) {
            m_manager.drawFittedString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]));
        } else if (pc == 6) {
            m_manager.drawFittedString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseAlign(p[6]));
        }
        break;
    case COMMAND_FONT:
        if (pc == 1) {
            m_manager.setFont(parseFont(p[1]));
        }
        break;
    case COMMAND_FONT_SIZE:
        if (pc == 1) {
            m_manager.setFontSize(parseInt(p[1]));
        }
        break;
    case COMMAND_FONT_ALIGN:
        if (pc == 1) {
            m_manager.setAlignment(parseAlign(p[1]));
        }
        break;
    case COMMAND_FONT_COLOR:
        if (pc == 1) {
            m_manager.setFontColor(parseColor(p[1]));
        }
        break;
    case COMMAND_FONT_BACK_COLOR:
        if (pc == 1) {
            m_manager.setBackgroundColor(parseColor(p[1]));
        }
        break;
    }
}
//    void drawString(const String &text, int x, int y, unsigned int fontSize, Align align, int32_t fgColor = -1, int32_t bgColor = -1, bool applyScale = true);

// Function to process an entire script
void DrawScript::processScript(const String& script) {
    int lineStart = 0;
    int lineEnd;

    while (lineStart < script.length()) {
        lineEnd = script.indexOf('\n', lineStart);
        if (lineEnd == -1) lineEnd = script.length();

        String line = script.substring(lineStart, lineEnd);
        line.replace("\r", ""); // Handle Windows-style text
        line.trim();

        std::vector<String> parameters = parseLine(line);

        if (!parameters.empty()) {
            auto it = commandMap.find(parameters[0].c_str());
            CommandType command = (it != commandMap.end()) ? it->second : COMMAND_UNKNOWN;

            processDrawCommand(command, parameters);
        }

        lineStart = lineEnd + 1;
    }
}
// Helper functions for parameter extraction
std::vector<String> DrawScript::parseLine(const String& line) {
    std::vector<String> tokens;
    String currentToken;
    bool escape = false;

    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];

        if (escape) {
            currentToken += c;
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == ',') {
            tokens.push_back(currentToken);
            currentToken = "";
        } else {
            currentToken += c;
        }
    }

    if (!currentToken.isEmpty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

int DrawScript::parseInt(const String& p) {
    int result;
    if(p.startsWith("y") || p.startsWith("t")) {
        result = 1;
    } else if(p.startsWith("n") || p.startsWith("f")) {
        result = 0;
    } else {
        result = strtol(p.c_str(), nullptr, 0);
    }
    return result;
}

TTF_Font DrawScript::parseFont(const String& name) {
    TTF_Font fval = DEFAULT_FONT;
    if(name.startsWith("r")) {
        fval = ROBOTO_REGULAR;
    }else if(name.startsWith("f")) {
        fval = FINAL_FRONTIER;
    } else if(name == "dseg7") {
        fval = DSEG7;
    } else if(name == "dseg14") {
        fval = DSEG14;
    }
 //   Serial.printf("font name '%s' = %d\n",name.c_str(), fval);
    return fval;
}

int DrawScript::parseColor(const String &p) {
    const char *start = p.c_str();
    int value;
    // Check if the parameter begins with a letter
    if (isalpha(*start)) {
        value = Utils::stringToColor(p);
    } else {
        // Treat as integer (including base 16)
        value = strtol(start, nullptr, 0); // Automatically detects base
        if (value != -1) {
            uint16_t r = (value >> 8) & 0xF800;
            uint16_t g = (value >> 5) & 0x07E0;
            uint16_t b = (value >> 3) & 0x001F;
            value = r | g | b;
        }
    }
   // Serial.printf("color value=%x\n", value);
    return value;
}

static const std::map<const std::string, const Align> alignStringsMap = {
    {"l", Align::Left},
    {"c", Align::Center},
    {"r", Align::Right},
    {"tl", Align::TopLeft},
    {"tc", Align::TopCenter},
    {"tr", Align::TopRight},
    {"ml", Align::MiddleLeft},
    {"mc", Align::MiddleCenter},
    {"mr", Align::MiddleRight},
    {"bl", Align::BottomLeft},
    {"bc", Align::BottomCenter},
    {"br", Align::BottomRight}
};
Align DrawScript::parseAlign(const String& p) {
    Align val = Align::MiddleCenter;
    auto it = alignStringsMap.find(p.c_str());
    if ( it != alignStringsMap.end()) {
        val = it->second;
    }
   // Serial.printf("align value=%d\n", val);
    return val;
}
