#include "ConfigManager.h"

#include "Button.h"
#include "Utils.h"
#include "config_helper.h"
#include "nvs_flash.h"

ConfigManager *ConfigManager::s_instance = nullptr;

ConfigManager::ConfigManager(WiFiManager &wm) : m_wm(wm) {
    Serial.println("Constructing ConfigManager");
    if (!preferences.begin("config", false)) {
        Serial.println("Failed to initialize NVS in ConfigManager.");
        Serial.println("...erasing NVS");
        nvs_flash_erase();
        Serial.println("...initializing NVS");
        nvs_flash_init();
        Serial.println("Retrying to init preferences...");
        if (!preferences.begin("config", false)) {
            Serial.println("...it didn't work. Giving up.");
        } else {
            Serial.println("...it worked!");
        }
    } else {
        Serial.println("NVS initialized successfully in ConfigManager");
        if (digitalRead(BUTTON_OK) == Button::PRESSED_LEVEL) {
            Serial.println("Middle button pressed -> Clearing preferences...");
            preferences.clear();
            Serial.println("...done");
        }
    }
    Serial.println("ConfigManager initialized");
    s_instance = this;
}

ConfigManager::~ConfigManager() {
    for (auto &param : parameters) {
        delete param.parameter;
    }
    preferences.end();
}

ConfigManager *ConfigManager::getInstance() {
    if (s_instance == nullptr) {
        Serial.println("ERR in ConfigManager.getInstance(): not initialized");
    }
    return s_instance;
}

void ConfigManager::setupWebPortal() {
    WiFiManagerParameter *startLabel = new WiFiManagerParameter("<H1>InfoOrbs Configuration</H1>");
    m_wm.addParameter(startLabel);
    char lastSection[30];
    for (auto &param : parameters) {
#ifdef CM_DEBUG
        Serial.printf("Adding WebPortal parameter: %s, %s\n", param.section, param.variableName);
#endif
        if (!Utils::compareCharArrays(lastSection, param.section)) {
            // New class variables -> add a separator
            WiFiManagerParameter *classLabel = new WiFiManagerParameter(Utils::createConstCharBufferAndConcat("<HR><H2 style='margin-block-end: 0;'>", param.section, "</H2>"));
            Serial.printf("New config area: %s\n", param.section);
            m_wm.addParameter(classLabel);
            strcpy(lastSection, param.section);
        }
        WiFiManagerParameter *divStart = new WiFiManagerParameter("<DIV>");
        WiFiManagerParameter *divEnd = new WiFiManagerParameter("</DIV>");
        m_wm.addParameter(divStart);
        m_wm.addParameter(param.parameter);
        m_wm.addParameter(divEnd);
    }
    WiFiManagerParameter *endLabel = new WiFiManagerParameter("<HR><BR><H3><font color='red'>Saving will restart the InfoOrbs to apply the new config.</font></H3>");
    m_wm.addParameter(endLabel);

    m_wm.setSaveParamsCallback([this]() {
        int count = m_wm.server->args();
#ifdef CM_DEBUG
        Serial.println("Variables saved in WebPortal");
        for (int i = 0; i < count; i++) {
            Serial.printf("Arg %d: %s = %s\n", i, m_wm.server->argName(i).c_str(), m_wm.server->arg(i).c_str());
        }
#endif
        if (count > 0) {
            saveAllConfigs();
            Serial.printf("%d config values saved.\n", count);
            // Restart to apply new config
            requiresRestart = true;
        } else {
            Serial.println("No confg values to save found. Skipping.");
        }
    });
}

void ConfigManager::saveAllConfigs() {
    for (auto &param : parameters) {
        param.saveCallback(); // Save variables to preferences
        triggerChangeCallbacks(param.section, param.variableName); // Notify listeners
    }
}

std::string ConfigManager::makeKey(const char *section, const char *varName) {
    return std::string(section).append("_").append(varName);
}

void ConfigManager::triggerChangeCallbacks(const char *section, const char *varName) {
#ifdef CM_DEBUG
    Serial.printf("triggerChangeCallbacks, c=%s, v=%s\n", section, varName);
#endif
    std::string key = makeKey(section, varName);
    if (varName[0] != '0' && changeCallbacks.count(key)) {
        for (const auto &callback : changeCallbacks[key]) {
            callback(section, varName);
        }
    }
    if (changeCallbacks.count(section)) {
        for (const auto &callback : changeCallbacks[section]) {
            callback(section, varName);
        }
    }
}

template <typename T, typename ParameterType, typename... Args>
void ConfigManager::addConfig(int paramType, const char *section, const char *varName, T *var, const char *description, uint8_t length,
                              std::function<void(T &)> loadFromPreferences, std::function<void(ParameterType *, T &)> setParameterValue, std::function<void(T &)> saveToPreferences,
                              Args... args) {

    // Load value from preferences
    loadFromPreferences(*var);

#ifdef CM_DEBUG
    Serial.printf("%s loaded %d (@%p)\n", varName, *var, var);
#endif

    // Create parameter with additional arguments if needed
    ParameterType *param = new ParameterType(varName, description, args..., *var, length);

    auto saveLambda = [this, section, varName, var, param, setParameterValue, saveToPreferences]() {
        // Set parameter value
        setParameterValue(param, *var);
        // Save to preferences
        saveToPreferences(*var);
#ifdef CM_DEBUG
        // Debugging output
        Serial.printf("%s saved %d (@%p)\n", varName, *var, var);
#endif
    };

    parameters.push_back({param, paramType, section, varName, saveLambda});
}

void ConfigManager::addConfigString(const char *section, const char *varName, std::string *var, size_t length, const char *description) {
    addConfig<std::string, StringParameter>(
        CM_PARAM_TYPE_STRING, section, varName, var, description, length,
        [this, varName](std::string &var) { var = preferences.getString(varName, var.c_str()).c_str(); },
        [](StringParameter *param, std::string &var) { var = param->getValue(); },
        [this, varName](std::string &var) { preferences.putString(varName, var.c_str()); });
}

void ConfigManager::addConfigInt(const char *section, const char *varName, int *var, const char *description) {
    addConfig<int, IntParameter>(
        CM_PARAM_TYPE_INT, section, varName, var, description, 10,
        [this, varName](int &var) { var = preferences.getInt(varName, var); },
        [](IntParameter *param, int &var) { var = param->getValue(); },
        [this, varName](int &var) { preferences.putInt(varName, var); });
}

void ConfigManager::addConfigBool(const char *section, const char *varName, bool *var, const char *description) {
    addConfig<bool, BoolParameter>(
        CM_PARAM_TYPE_BOOL, section, varName, var, description, 2,
        [this, varName](bool &var) { var = preferences.getBool(varName, var); },
        [this](BoolParameter *param, bool &var) { var = param->getValue(this->m_wm); },
        [this, varName](bool &var) { preferences.putBool(varName, var); });
}

void ConfigManager::addConfigFloat(const char *section, const char *varName, float *var, const char *description) {
    addConfig<float, FloatParameter>(
        CM_PARAM_TYPE_FLOAT, section, varName, var, description, 10,
        [this, varName](float &var) { var = preferences.getFloat(varName, var); },
        [](FloatParameter *param, float &var) { var = param->getValue(); },
        [this, varName](float &var) { preferences.putFloat(varName, var); });
}

void ConfigManager::addConfigColor(const char *section, const char *varName, int *var, const char *description) {
    addConfig<int, ColorParameter>(
        CM_PARAM_TYPE_COLOR, section, varName, var, description, 8,
        [this, varName](int &var) { var = preferences.getInt(varName, var); },
        [](ColorParameter *param, int &var) { var = param->getValue(); },
        [this, varName](int &var) { preferences.putInt(varName, var); });
}

void ConfigManager::addConfigComboBox(const char *section, const char *varName, int *var, String options[], int numOptions, const char *description) {
    addConfig<int, ComboBoxParameter>(
        CM_PARAM_TYPE_COMBOBOX, section, varName, var, description, 0,
        [this, varName](int &var) { var = preferences.getInt(varName, var); },
        [this](ComboBoxParameter *param, int &var) { var = param->getValue(this->m_wm); },
        [this, varName](int &var) { preferences.putInt(varName, var); },
        options, // Pass options array
        numOptions // Pass number of options
    );
}

std::string ConfigManager::getConfigString(const char *varName, std::string defaultValue) {
    std::string val = preferences.getString(varName, defaultValue.c_str()).c_str();
    return val;
}

bool ConfigManager::getConfigBool(const char *varName, bool defaultValue) {
    bool val = preferences.getBool(varName, defaultValue);
    return val;
}

int ConfigManager::getConfigInt(const char *varName, int defaultValue) {
    int val = preferences.getInt(varName, defaultValue);
    return val;
}

float ConfigManager::getConfigFloat(const char *varName, float defaultValue) {
    float val = preferences.getFloat(varName, defaultValue);
    return val;
}

void ConfigManager::addOnChangeCallback(
    const char *section, const char *varName, const std::function<void(const char *section, const char *varName)> &callback) {
    changeCallbacks[makeKey(section, varName)].push_back(callback);
}

void ConfigManager::addOnChangeCallback(
    const char *section, const std::function<void(const char *section, const char *varName)> &callback) {
    changeCallbacks[section].push_back(callback);
}
