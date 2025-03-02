#include "I18n.h"

String I18n::s_allLanguages[] = ALL_LANGUAGES;
int I18n::s_languageId = DEFAULT_LANGUAGE;

void I18n::setLanguageId(const int langId) {
    if (langId >= 0 && langId < LANG_NUM) {
        s_languageId = langId;
    } else {
        s_languageId = DEFAULT_LANGUAGE;
    }
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
