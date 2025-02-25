#ifndef I18N_H
#define I18N_H

#include "config_helper.h"
#include <Arduino.h>

#define ALL_LANGUAGES {"en", "de", "fr"}

enum Language {
    LANG_EN = 0,
    LANG_DE,
    LANG_FR,
    LANG_NUM // keep this as last item
};

#define DEFAULT_LANGUAGE LANG_EN

// Define a custom type for translation arrays
using Translation = const char *const[LANG_NUM];
// Define a template for translation arrays with a fixed number of columns (LANG_NUM)
template <size_t Rows>
using TranslationMulti = const char *const[Rows][LANG_NUM];

class I18n {
public:
    static void setLanguageId(int langId);
    static String getLanguageString(int langId);
    static String *getAllLanguages();

    template <size_t N>
    static const char *get(const char *const (&translations)[N]) {
        const char *text = translations[s_languageId];
        if (text == nullptr) {
            text = translations[DEFAULT_LANGUAGE];
        }
        return text ? text : "@missingTranslation@";
    }

    template <size_t X, size_t Y>
    static const char *get(const char *const (&translations)[X][Y], size_t index) {
        if (index >= X) {
            return "@invalidIndex@";
        }

        const char *text = translations[index][s_languageId];
        if (text == nullptr) {
            text = translations[index][DEFAULT_LANGUAGE];
        }
        return text ? text : "@missingTranslation@";
    }

private:
    static String s_allLanguages[];
    static int s_languageId;
};

// I18n helper
template <size_t N>
static const char *i18n(const char *const (&translations)[N]) {
    return I18n::get(translations);
}

// I18n helper (with index)
template <size_t X, size_t Y>
static const char *i18n(const char *const (&translations)[X][Y], size_t index) {
    return I18n::get(translations, index);
}

#endif // I18N_H
