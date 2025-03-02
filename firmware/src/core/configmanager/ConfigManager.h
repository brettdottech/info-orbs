#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "I18n.h"
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

    // Add a string configuration variable
    void addConfigString(const char *section, const char *varName, std::string *var, size_t length, const char *description, bool advanced = false);
    // Add a string configuration variable with a localized description
    void addConfigString(const char *section, const char *varName, std::string *var, size_t length, Translation &description, bool advanced = false);

    // Add an integer configuration variable
    void addConfigInt(const char *section, const char *varName, int *var, const char *description, bool advanced = false);
    // Add an integer configuration variable with a localized description
    void addConfigInt(const char *section, const char *varName, int *var, Translation &description, bool advanced = false);

    // Add a float configuration variable
    void addConfigFloat(const char *section, const char *varName, float *var, const char *description, bool advanced = false);
    // Add a float configuration variable with a localized description
    void addConfigFloat(const char *section, const char *varName, float *var, Translation &description, bool advanced = false);

    // Add a boolean configuration variable
    void addConfigBool(const char *section, const char *varName, bool *var, const char *description, bool advanced = false);
    // Add a boolean configuration variable with a localized description
    void addConfigBool(const char *section, const char *varName, bool *var, Translation &description, bool advanced = false);

    // Add a color configuration variable
    void addConfigColor(const char *section, const char *varName, int *var, const char *description, bool advanced = false);
    // Add a color configuration variable with a localized description
    void addConfigColor(const char *section, const char *varName, int *var, Translation &description, bool advanced = false);

    // Add a ComboBox configuration variable
    void addConfigComboBox(const char *section, const char *varName, int *var, String options[], int numOptions, const char *description, bool advanced = false);
    // Add a ComboBox configuration variable with a localized description
    void addConfigComboBox(const char *section, const char *varName, int *var, String options[], int numOptions, Translation &description, bool advanced = false);
    // Add a ComboBox configuration variable with localized options and description
    template <size_t N>
    void addConfigComboBox(const char *section, const char *varName, int *var, TranslationMulti<N> &options, Translation &description, const bool advanced = false) {
        String optionsArray[N];
        i18nMultiStr(options, optionsArray);
        addConfigComboBox(section, varName, var, optionsArray, N, description, advanced);
    }

    // Retrieve a string configuration value, with a default fallback
    std::string getConfigString(const char *varName, std::string defaultValue);
    // Retrieve a boolean configuration value, with a default fallback
    bool getConfigBool(const char *varName, bool defaultValue);
    // Retrieve an integer configuration value, with a default fallback
    int getConfigInt(const char *varName, int defaultValue);
    // Retrieve a float configuration value, with a default fallback
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
