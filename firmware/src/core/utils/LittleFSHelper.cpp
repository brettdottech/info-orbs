#include "LittleFSHelper.h"

bool LittleFSHelper::begin() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed. Formatting...");
        LittleFS.format();
        if (!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS after formatting.");
            return false;
        }
    }
    Serial.println("LittleFS mounted successfully.");
    return true;
}

void LittleFSHelper::writeFile(const char *path, const char *message) {
    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(message);
    file.close();
    Serial.println("File written successfully.");
}

void LittleFSHelper::readFile(const char *path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    Serial.print("Reading file: ");
    Serial.println(path);
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void LittleFSHelper::deleteFile(const char *path) {
    if (LittleFS.remove(path)) {
        Serial.println("File deleted successfully.");
    } else {
        Serial.println("Failed to delete file.");
    }
}

void LittleFSHelper::listFilesRecursively(const char *dirname) {
    // Ensure the path starts with a `/`
    if (dirname[0] != '/') {
        Serial.println("Path must start with '/'");
        return;
    }

    File root = LittleFS.open(dirname);
    if (!root || !root.isDirectory()) {
        Serial.printf("Failed to open directory: %s\n", dirname);
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("Directory: ");
            // Recursive call for subdirectories
            char fullPath[128];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirname, file.name());
            Serial.print(fullPath);
            listFilesRecursively(fullPath);
        } else {
            Serial.print("File: ");
            Serial.printf("%s/%s", dirname, file.name());
            Serial.print(", Size: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
