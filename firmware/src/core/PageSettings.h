#ifndef PAGESETTINGS_H
#define PAGESETTINGS_H

#include <Arduino.h>

struct PageSettings {
    String Serialize();
    static PageSettings Deserialize(const String& settings);
};

#endif