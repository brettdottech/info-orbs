#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Utils.h"
#include "WifiManagerCustomParameters.h"
#include "nvs_flash.h"
#include <Preferences.h>
#include <WiFiManager.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#define CM_PARAM_TYPE_STRING 0
#define CM_PARAM_TYPE_INT 1
#define CM_PARAM_TYPE_BOOL 2
#define CM_PARAM_TYPE_FLOAT 3
#define CM_PARAM_TYPE_COLOR 4
#define CM_PARAM_TYPE_IP 5

class ConfigManager {
public:
    ConfigManager(WiFiManager &wm);
    ~ConfigManager();

    static ConfigManager &getInstance();

    void setupWiFiManager();
    void loadAllConfigs();
    void saveAllConfigs();

    // Register different types of variables with descriptions
    void addConfigString(const std::string &className, const std::string &varName, char *var, size_t length, const std::string &description);
    void addConfigInt(const std::string &className, const std::string &varName, int *var, const std::string &description);
    void addConfigFloat(const std::string &className, const std::string &varName, float *var, const std::string &description);
    void addConfigIP(const std::string &className, const std::string &varName, IPAddress *var, const std::string &description);
    void addConfigBool(const std::string &className, const std::string &varName, bool *var, const std::string &description);
    void addConfigColor(const std::string &className, const std::string &varName, int *var, const std::string &description);

    // Get stored values
    bool getConfigBool(const std::string &varName, bool defaultValue);

    // Register callbacks for changes
    void addOnChangeCallback(
        const std::string &className,
        const std::string &varName,
        const std::function<void(const std::string &className, const std::string &varName)> &callback);
    void addOnChangeCallback(
        const std::string &className,
        const std::function<void(const std::string &className, const std::string &varName)> &callback);

private:
    struct Parameter {
        WiFiManagerParameter *parameter;
        const int type;
        const char *className;
        const char *variableName;
        std::function<void()> saveCallback;
    };

    static ConfigManager *m_instance;

    WiFiManager &m_wm;
    Preferences preferences;
    std::vector<Parameter> parameters;

    std::unordered_map<std::string, std::vector<std::function<void(const std::string &className, const std::string &varName)>>> changeCallbacks;

    std::string makeKey(const std::string &className, const std::string &varName);
    void triggerChangeCallbacks(const std::string &className, const std::string &varName = "");
};

#endif // CONFIG_MANAGER_H
