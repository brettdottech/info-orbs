#include "I18n.h"
#include <LittleFS.h>

std::map<String, String> I18n::translations;

bool I18n::load(const char *filename) {
    File file = LittleFS.open(filename, "r");
    if (!file) {
        Serial.println("Failed to open language file");
        return false;
    }

    translations.clear();
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();

        if (line.length() == 0 || line.startsWith("#"))
            continue;

        int sepIndex = line.indexOf('=');
        if (sepIndex > 0) {
            String key = line.substring(0, sepIndex);
            String value = line.substring(sepIndex + 1);
            key.trim();
            value.trim();
            translations[key] = value;
        }
    }
    file.close();
    return true;
}

String I18n::get(const String &key) {
    if (translations.find(key) != translations.end()) {
        return translations[key];
    }
    return key;
}

void I18n::replacePlaceholder(String &str, int index, const String &value) {
    String placeholder = "\\" + String(index); // e.g., \1, \2
    str.replace(placeholder, value);
}