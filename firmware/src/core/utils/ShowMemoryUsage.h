#ifndef SHOW_MEMORY_USAGE_H
#define SHOW_MEMORY_USAGE_H

#define SHOW_MEMORY_USAGE(msg)                     \
    do {                                           \
        ShowMemoryUsage::printSerial(true, false); \
        if (msg[0] != '\0') {                      \
            Serial.print(" --- ");                 \
            Serial.println(msg);                   \
        } else {                                   \
            Serial.println();                      \
        }                                          \
    } while (0)

class ShowMemoryUsage {
public:
    static void printSerial(bool force = false, bool newLine = true);

private:
    static unsigned long s_lastMemoryUsageShownAt;
};

#endif // SHOW_MEMORY_USAGE_H
