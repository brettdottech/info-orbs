#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "OrbsWiFiManager.h"
#include "WifiManagerCustomParameters.h"
#include <Preferences.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

enum class ParamType {
    String = 0,
    Int = 1,
    Bool = 2,
    Float = 3,
    Color = 4,
    ComboBox = 5
};

// Uncomment to show debug output
// #define CM_DEBUG

class ConfigManager {
public:
    ConfigManager(WiFiManager &wm);
    ~ConfigManager();

    static ConfigManager *getInstance();

    void setupWebPortal();
    void saveAllConfigs();

    // Register different types of variables with descriptions
    void addConfigString(const char *section, const char *varName, std::string *var, size_t length, const char *description, bool advanced = false);
    void addConfigInt(const char *section, const char *varName, int *var, const char *description, bool advanced = false);
    void addConfigFloat(const char *section, const char *varName, float *var, const char *description, bool advanced = false);
    void addConfigIP(const char *section, const char *varName, IPAddress *var, const char *description, bool advanced = false);
    void addConfigBool(const char *section, const char *varName, bool *var, const char *description, bool advanced = false);
    void addConfigColor(const char *section, const char *varName, int *var, const char *description, bool advanced = false);
    void addConfigComboBox(const char *section, const char *varName, int *var, String options[], int numOptions, const char *description, bool advanced = false);

    // Get stored values
    std::string getConfigString(const char *varName, std::string defaultValue);
    bool getConfigBool(const char *varName, bool defaultValue);
    int getConfigInt(const char *varName, int defaultValue);
    float getConfigFloat(const char *varName, float defaultValue);

    // Register callbacks for changes
    void addOnChangeCallback(
        const char *section,
        const char *varName,
        const std::function<void(const char *section, const char *varName)> &callback);
    void addOnChangeCallback(
        const char *section,
        const std::function<void(const char *section, const char *varName)> &callback);

    // Check if a restart is required
    bool requiresRestart() { return m_requiresRestart; }

private:
    struct Parameter {
        WiFiManagerParameter *parameter;
        const ParamType type;
        const char *section;
        const char *variableName;
        const bool advanced;
        std::function<void()> saveCallback;
    };

    static ConfigManager *s_instance;

    WiFiManager &m_wm;
    Preferences m_preferences;
    std::vector<Parameter> m_parameters;
    std::unordered_map<std::string, std::vector<std::function<void(const char *section, const char *varName)>>> m_changeCallbacks;
    bool m_requiresRestart = false;

    template <typename T, typename ParameterType, typename... Args>
    void addConfig(ParamType paramType, const char *section, const char *varName, T *var, const char *description, uint8_t length, bool advanced,
                   std::function<void(T &)> loadFromPreferences, std::function<void(ParameterType *, T &)> setParameterValue, std::function<void(T &)> saveToPreferences, Args... args);

    std::string makeKey(const char *section, const char *varName);
    void triggerChangeCallbacks(const char *section, const char *varName = "");
};

#endif // CONFIG_MANAGER_H
