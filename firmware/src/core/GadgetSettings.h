#ifndef GADGETSETTINGS_H
#define GADGETSETTINGS_H

#include <Arduino.h>

struct GadgetSettings {
public:
    String Serialize();
    static GadgetSettings Deserialize(const String& settings);
};

#endif