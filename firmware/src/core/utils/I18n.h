#ifndef I18N_H
#define I18N_H

#include "config_helper.h"
#include <Arduino.h>
#include <map>

#ifndef I18N_DIR
    #define I18N_DIR "/i18n/"
#endif

#define ALL_LANGUAGES {"en", "de", "fr"}

enum Language {
    LANG_EN = 0,
    LANG_DE,
    LANG_FR,
    LANG_NUM
};

#define DEFAULT_LANGUAGE LANG_EN

class I18n {
public:
    static void setLanguageId(int langId);
    static String getLanguageString(int langId);
    static String *getAllLanguages();

    static void loadExtraTranslations(const String &extraName);

    // Returning const char* here allows us to use it in ConfigManager.addConfig*()
    // because it will never go out of scope (as long as the translation is not removed)
    static const char *get(const String &key);

    // Returning String here instead of const char* as it ensures the memory
    // is managed properly and avoids the risk of accessing invalid memory
    // from dynamic translations. However, this approach does not allow us to
    // use it in ConfigManager.addConfig*(), because the String and its
    // c_str might go out of scope
    template <typename... Args>
    static String get(const String &key, Args... args) {
        String result = get(key);
        replacePlaceholders(result, args...);
        return result;
    }

private:
    static String s_allLanguages[];
    static int s_languageId;
    static std::map<String, const char *> s_translations;

    static void clearTranslations();
    static bool loadFile(const String &filename);
    static bool loadFile(int langId);

    static void replacePlaceholder(String &str, int index, const String &value);

    template <typename T, typename... Args>
    static void replacePlaceholders(String &str, T value, Args... args) {
        static int index = 1;
        replacePlaceholder(str, index++, String(value));
        replacePlaceholders(str, args...);
    }

    static void replacePlaceholders(String &) {}
};

#endif // I18N_H
