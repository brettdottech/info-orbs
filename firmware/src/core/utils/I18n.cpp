#include "I18n.h"

#include <ArduinoLog.h>
#include <LittleFS.h>

std::map<String, String> I18n::s_translations;
String I18n::s_language = DEFAULT_LANGUAGE;

void I18n::setLanguage(const String &lang) {
    s_language = lang;
    if (s_language != DEFAULT_LANGUAGE) {
        // Load default translations (fallback)
        loadFile(DEFAULT_LANGUAGE);
    }
    // Load actual language file
    loadFile(lang);
}

String I18n::getLanguage() {
    return s_language;
}

void I18n::loadExtraTranslations(const String &extraName) {
    if (extraName.isEmpty() == 0) {
        return;
    }
    // Convert Widget name to lowercase for file name
    auto lowercaseName = extraName;
    lowercaseName.toLowerCase();
    if (s_language != DEFAULT_LANGUAGE) {
        // Load default translations (fallback)
        loadFile(String(DEFAULT_LANGUAGE) + "." + lowercaseName);
    }
    loadFile(s_language + "." + lowercaseName);
}

bool I18n::loadFile(const String &filename, const bool clear) {
    if (filename.isEmpty()) {
        return false;
    }
    const String fullName = I18N_DIR + filename + ".txt";
    File file = LittleFS.open(fullName, "r");
    if (!file) {
        Log.warningln("Failed to open language file %s", fullName.c_str());
        return false;
    }
    if (clear) {
        s_translations.clear();
    }
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
            s_translations[key] = value;
        }
    }
    file.close();
    Log.noticeln("Loaded language file %s", fullName.c_str());
    return true;
}

String I18n::get(const String &key) {
    if (s_translations.find(key) != s_translations.end()) {
        return s_translations[key];
    }
    return key;
}

void I18n::replacePlaceholder(String &str, int index, const String &value) {
    String placeholder = "\\" + String(index); // e.g., \1, \2
    str.replace(placeholder, value);
}