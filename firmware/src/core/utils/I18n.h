#ifndef I18N_H
#define I18N_H

#include "config_helper.h"
#include <Arduino.h>
#include <map>

#ifndef I18N_DIR
    #define I18N_DIR "/i18n/"
#endif

#ifndef DEFAULT_LANGUAGE
    #define DEFAULT_LANGUAGE "en"
#endif

class I18n {
public:
    static void setLanguage(const String &language);
    static String getLanguage();

    static void loadExtraTranslations(const String &extraName);
    static String get(const String &key);

    template <typename... Args>
    static String get(const String &key, Args... args) {
        String result = get(key);
        replacePlaceholders(result, args...);
        return result;
    }

private:
    static String s_language;
    static std::map<String, String> s_translations;

    static bool loadFile(const String &filename, bool clear = false);

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
