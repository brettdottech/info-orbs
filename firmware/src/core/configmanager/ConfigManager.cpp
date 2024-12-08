#include "ConfigManager.h"

ConfigManager *ConfigManager::m_instance = nullptr;

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
    m_instance = this;
}

ConfigManager::~ConfigManager() {
    for (auto &param : parameters) {
        delete param.parameter;
    }
    preferences.end();
}

ConfigManager *ConfigManager::getInstance() {
    if (m_instance == nullptr) {
        Serial.println("ERR in ConfigManager.getInstance(): not initialized");
    }
    return m_instance;
}

void ConfigManager::setupWiFiManager() {
    WiFiManagerParameter *startLabel = new WiFiManagerParameter(Utils::copyString("<H1>InfoOrbs Configuration</H1>"));
    m_wm.addParameter(startLabel);
    char lastClassName[30];
    for (auto &param : parameters) {
#ifdef CM_DEBUG
        Serial.printf("Adding WebPortal parameter: %s, %s\n", param.className, param.variableName);
#endif
        if (!Utils::compareCharArrays(lastClassName, param.className)) {
            // New class variables -> add a separator
            WiFiManagerParameter *classLabel = new WiFiManagerParameter(Utils::createWithPrefixAndPostfix("<HR><H2 style='margin-block-end: 0;'>", param.className, "</H2>"));
            Serial.printf("New config area: %s\n", param.className);
            m_wm.addParameter(classLabel);
            strcpy(lastClassName, param.className);
        }
        if (param.type == CM_PARAM_TYPE_COLOR) {
            // Add extra <BR> for ColorParameters
            WiFiManagerParameter *brLabel = new WiFiManagerParameter("<BR>");
            m_wm.addParameter(brLabel);
        }
        m_wm.addParameter(param.parameter);
    }
    WiFiManagerParameter *endLabel = new WiFiManagerParameter(Utils::copyString("<HR><BR><H3><font color='red'>Saving will restart the InfoOrbs to apply the new config.</font></H3>"));
    m_wm.addParameter(endLabel);

    m_wm.setSaveParamsCallback([this]() {
#ifdef CM_DEBUG
        Serial.println("Variables saved in WebPortal");
        int count = m_wm.server->args();
        for (int i = 0; i < count; i++) {
            Serial.printf("Arg %d: %s = %s\n", i, m_wm.server->argName(i).c_str(), m_wm.server->arg(i).c_str());
        }
#endif
        saveAllConfigs();
        // Restart to apply new config
        Serial.println("New config values saved. Restarting ESP now.");
        ESP.restart();
    });
}

void ConfigManager::saveAllConfigs() {
    for (auto &param : parameters) {
        param.saveCallback(); // Save variables to preferences
        triggerChangeCallbacks(param.className, param.variableName); // Notify listeners
    }
}

std::string ConfigManager::makeKey(const std::string &className, const std::string &varName) {
    return className + "_" + varName;
}

void ConfigManager::triggerChangeCallbacks(const std::string &className, const std::string &varName) {
#ifdef CM_DEBUG
    Serial.printf("triggerChangeCallbacks, c=%s, v=%s\n", className.c_str(), varName.c_str());
#endif
    if (!varName.empty() && changeCallbacks.count(className + "_" + varName)) {
        for (const auto &callback : changeCallbacks[className + "_" + varName]) {
            callback(className, varName);
        }
    }
    if (changeCallbacks.count(className)) {
        for (const auto &callback : changeCallbacks[className]) {
            callback(className, varName);
        }
    }
}

template <typename T, typename ParameterType, typename... Args>
void ConfigManager::addConfig(int paramType, const std::string &className, const std::string &varName, T *var, const std::string &description,
                              std::function<void(T &)> loadFromPreferences, std::function<void(ParameterType *, T &)> setParameterValue, std::function<void(T &)> saveToPreferences,
                              Args... args) {

    // Convert std::string to char* once
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);

    // Load value from preferences
    loadFromPreferences(*var);

#ifdef CM_DEBUG
    Serial.printf("%s loaded %d (@%p)\n", varNameBuffer, *var, var);
#endif

    // Create parameter with additional arguments if needed
    ParameterType *param = new ParameterType(varNameBuffer, descBuffer, args..., *var);

    auto saveLambda = [this, classNameBuffer, varNameBuffer, var, param, setParameterValue, saveToPreferences]() {
        // Set parameter value
        setParameterValue(param, *var);
        // Save to preferences
        saveToPreferences(*var);
#ifdef CM_DEBUG
        // Debugging output
        Serial.printf("%s saved %d (@%p)\n", varNameBuffer, *var, var);
#endif
    };

    parameters.push_back({param, paramType, classNameBuffer, varNameBuffer, saveLambda});
}

void ConfigManager::addConfigString(const std::string &className, const std::string &varName, std::string *var, size_t length, const std::string &description) {
    addConfig<std::string, StringParameter>(
        CM_PARAM_TYPE_STRING, className, varName, var, description,
        [this, varName](std::string &var) { var = preferences.getString(varName.c_str(), var.c_str()).c_str(); },
        [](StringParameter *param, std::string &var) { var = param->getValue(); },
        [this, varName](std::string &var) { preferences.putString(varName.c_str(), var.c_str()); });
}

void ConfigManager::addConfigInt(const std::string &className, const std::string &varName, int *var, const std::string &description) {
    addConfig<int, IntParameter>(
        CM_PARAM_TYPE_INT, className, varName, var, description,
        [this, varName](int &var) { var = preferences.getInt(varName.c_str(), var); },
        [](IntParameter *param, int &var) { var = param->getValue(); },
        [this, varName](int &var) { preferences.putInt(varName.c_str(), var); });
}

void ConfigManager::addConfigBool(const std::string &className, const std::string &varName, bool *var, const std::string &description) {
    addConfig<bool, BoolParameter>(
        CM_PARAM_TYPE_BOOL, className, varName, var, description,
        [this, varName](bool &var) { var = preferences.getBool(varName.c_str(), var); },
        [this](BoolParameter *param, bool &var) { var = param->getValue(this->m_wm); },
        [this, varName](bool &var) { preferences.putBool(varName.c_str(), var); });
}

void ConfigManager::addConfigFloat(const std::string &className, const std::string &varName, float *var, const std::string &description) {
    addConfig<float, FloatParameter>(
        CM_PARAM_TYPE_FLOAT, className, varName, var, description,
        [this, varName](float &var) { var = preferences.getFloat(varName.c_str(), var); },
        [](FloatParameter *param, float &var) { var = param->getValue(); },
        [this, varName](float &var) { preferences.putFloat(varName.c_str(), var); });
}

void ConfigManager::addConfigColor(const std::string &className, const std::string &varName, int *var, const std::string &description) {
    addConfig<int, ColorParameter>(
        CM_PARAM_TYPE_COLOR, className, varName, var, description,
        [this, varName](int &var) { var = preferences.getInt(varName.c_str(), var); },
        [](ColorParameter *param, int &var) { var = param->getValue(); },
        [this, varName](int &var) { preferences.putInt(varName.c_str(), var); });
}

void ConfigManager::addConfigComboBox(const std::string &className, const std::string &varName, int *var, String options[], int numOptions, const std::string &description) {
    addConfig<int, ComboBoxParameter>(
        CM_PARAM_TYPE_COMBOBOX, className, varName, var, description,
        [this, varName](int &var) { var = preferences.getInt(varName.c_str(), var); },
        [this](ComboBoxParameter *param, int &var) { var = param->getValue(this->m_wm); },
        [this, varName](int &var) { preferences.putInt(varName.c_str(), var); },
        options, // Pass options array
        numOptions // Pass number of options
    );
}

String ConfigManager::getConfigString(const std::string &varName, String defaultValue) {
    String val = preferences.getString(varName.c_str(), defaultValue);
    return val;
}

bool ConfigManager::getConfigBool(const std::string &varName, bool defaultValue) {
    bool val = preferences.getBool(varName.c_str(), defaultValue);
    return val;
}

int ConfigManager::getConfigInt(const std::string &varName, int defaultValue) {
    int val = preferences.getInt(varName.c_str(), defaultValue);
    return val;
}

float ConfigManager::getConfigFloat(const std::string &varName, float defaultValue) {
    float val = preferences.getFloat(varName.c_str(), defaultValue);
    return val;
}

void ConfigManager::addOnChangeCallback(
    const std::string &className, const std::string &varName, const std::function<void(const std::string &className, const std::string &varName)> &callback) {
    changeCallbacks[className + "_" + varName].push_back(callback);
}

void ConfigManager::addOnChangeCallback(
    const std::string &className, const std::function<void(const std::string &className, const std::string &varName)> &callback) {
    changeCallbacks[className].push_back(callback);
}
