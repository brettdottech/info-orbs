#include "I18n.h"

#include <ArduinoLog.h>
#include <LittleFS.h>

String I18n::s_allLanguages[] = ALL_LANGUAGES;
std::map<String, String> I18n::s_translations;
int I18n::s_languageId = DEFAULT_LANGUAGE;

void I18n::setLanguageId(const int langId) {
    s_languageId = langId;
    if (s_languageId != DEFAULT_LANGUAGE) {
        // Load default translations (fallback)
        loadFile(DEFAULT_LANGUAGE);
    }
    // Load actual language file
    loadFile(langId);
}

String I18n::getLanguageString(const int langId) {
    if (langId >= 0 && langId < LANG_NUM) {
        return s_allLanguages[langId];
    }
    return "invalid";
}

String *I18n::getAllLanguages() {
    return s_allLanguages;
}

void I18n::loadExtraTranslations(const String &extraName) {
    if (extraName.isEmpty()) {
        return;
    }
    // Convert Widget name to lowercase for file name
    auto lowercaseName = extraName;
    lowercaseName.toLowerCase();
    if (s_languageId != DEFAULT_LANGUAGE) {
        // Load default translations (fallback)
        loadFile(getLanguageString(DEFAULT_LANGUAGE) + "." + lowercaseName);
    }
    loadFile(getLanguageString(s_languageId) + "." + lowercaseName);
}

bool I18n::loadFile(const int langId) {
    return loadFile(getLanguageString(langId));
}

bool I18n::loadFile(const String &filename) {
    if (filename.isEmpty()) {
        return false;
    }
    const String fullName = I18N_DIR + filename + ".txt";
    File file = LittleFS.open(fullName, "r");
    if (!file) {
        Log.warningln("Failed to open language file %s", fullName.c_str());
        return false;
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