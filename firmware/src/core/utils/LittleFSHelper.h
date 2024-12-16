#ifndef LITTLEFSHELPER_H
#define LITTLEFSHELPER_H

#include <LittleFS.h>

class LittleFSHelper {
public:
    static bool begin();
    static void writeFile(const char *path, const char *message);
    static void readFile(const char *path);
    static void deleteFile(const char *path);
    static void listFilesRecursively(const char *dirname);
};

#endif // LITTLEFSHELPER_H
