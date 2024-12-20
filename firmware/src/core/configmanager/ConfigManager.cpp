#include "ConfigManager.h"

#include "Button.h"
#include "Utils.h"
#include "config_helper.h"
#include "nvs_flash.h"

ConfigManager *ConfigManager::s_instance = nullptr;

// Assign pseudo parameters for /param
static WiFiManagerParameter s_scriptBlock(WEBPORTAL_PARAM_SCRIPT);
static WiFiManagerParameter s_styleBlock(WEBPORTAL_PARAM_STYLE);
static WiFiManagerParameter s_pageStart(WEBPORTAL_PARAM_PAGE_START);
static WiFiManagerParameter s_pageEnd(WEBPORTAL_PARAM_PAGE_END);
static WiFiManagerParameter s_fieldsetStart(WEBPORTAL_PARAM_FIELDSET_START);
static WiFiManagerParameter s_fieldsetEnd(WEBPORTAL_PARAM_FIELDSET_END);
static WiFiManagerParameter s_legendStart(WEBPORTAL_PARAM_LEGEND_START);
static WiFiManagerParameter s_legendEnd(WEBPORTAL_PARAM_LEGEND_END);
static WiFiManagerParameter s_divStartNonString(WEBPORTAL_PARAM_DIV_START);
static WiFiManagerParameter s_divStartString(WEBPORTAL_PARAM_DIV_STRING_START);
static WiFiManagerParameter s_divEnd(WEBPORTAL_PARAM_DIV_END);
static WiFiManagerParameter s_toggleAdvanced(WEBPORTAL_PARAM_TOGGLE_ADVANCED);
static WiFiManagerParameter s_spanAdvancedStart(WEBPORTAL_PARAM_SPAN_ADVANCED_START);
static WiFiManagerParameter s_spanEnd(WEBPORTAL_PARAM_SPAN_END);

ConfigManager::ConfigManager(WiFiManager &wm) : m_wm(wm) {
    Serial.println("Constructing ConfigManager");
    if (!m_preferences.begin("config", false)) {
        Serial.println("Failed to initialize NVS in ConfigManager.");
        Serial.println("...erasing NVS");
        nvs_flash_erase();
        Serial.println("...initializing NVS");
        nvs_flash_init();
        Serial.println("Retrying to init preferences...");
        if (!m_preferences.begin("config", false)) {
            Serial.println("...it didn't work. Giving up.");
        } else {
            Serial.println("...it worked!");
        }
    } else {
        Serial.println("NVS initialized successfully in ConfigManager");
        if (digitalRead(BUTTON_MIDDLE_PIN) == Button::PRESSED_LEVEL) {
            Serial.println("Middle button pressed -> Clearing preferences...");
            m_preferences.clear();
            Serial.println("...done");
        }
    }
    Serial.println("ConfigManager initialized");
    s_instance = this;
}

ConfigManager::~ConfigManager() {
    for (auto &param : m_parameters) {
        delete param.parameter;
    }
    m_preferences.end();
}

ConfigManager *ConfigManager::getInstance() {
    if (s_instance == nullptr) {
        Serial.println("ERR in ConfigManager.getInstance(): not initialized");
    }
    return s_instance;
}

void ConfigManager::setupWebPortal() {
    // Setup custom styles for params
    m_wm.addParameter(&s_styleBlock);
    m_wm.addParameter(&s_pageStart);
    char lastSection[30] = {0};
    bool advancedOpen = false;
    for (auto &param : m_parameters) {
#ifdef CM_DEBUG
        Serial.printf("Adding WebPortal parameter: %s, %s\n", param.section, param.variableName);
#endif
        if (strcmp(lastSection, param.section) != 0) {
            Serial.printf("New config section: %s\n", param.section);
            if (advancedOpen) {
                // close advanced params span
                m_wm.addParameter(&s_spanEnd);
                advancedOpen = false;
            }
            if (lastSection[0] != '\0') {
                // close previous section
                m_wm.addParameter(&s_fieldsetEnd);
            }
            m_wm.addParameter(&s_fieldsetStart);
            WiFiManagerParameter *legend = new WiFiManagerParameter(param.section);
            m_wm.addParameter(&s_legendStart);
            m_wm.addParameter(legend);
            m_wm.addParameter(&s_legendEnd);
            strcpy(lastSection, param.section);
        }
        if (param.advanced && !advancedOpen) {
            // add advanced toggle
            m_wm.addParameter(&s_toggleAdvanced);
            m_wm.addParameter(&s_spanAdvancedStart);
            advancedOpen = true;
        }
        // different divs for StringParameter and the rest, StringParameter should be in two lines, the rest in one
        m_wm.addParameter(param.type == ParamType::String ? &s_divStartString : &s_divStartNonString);
        m_wm.addParameter(param.parameter);
        m_wm.addParameter(&s_divEnd);
    }
    if (lastSection[0] != '\0') {
        // At least one section was created (should always be true...)
        if (advancedOpen) {
            // close advanced params span
            m_wm.addParameter(&s_spanEnd);
        }
        // close section
        m_wm.addParameter(&s_fieldsetEnd);
    }
    // Add Javascript
    m_wm.addParameter(&s_scriptBlock);
    m_wm.addParameter(&s_pageEnd);

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
            m_requiresRestart = true;
        } else {
            Serial.println("No confg values to save found. Skipping.");
        }
    });
}

void ConfigManager::saveAllConfigs() {
    for (auto &param : m_parameters) {
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
    if (varName[0] != '\0' && m_changeCallbacks.count(key)) {
        for (const auto &callback : m_changeCallbacks[key]) {
            callback(section, varName);
        }
    }
    if (m_changeCallbacks.count(section)) {
        for (const auto &callback : m_changeCallbacks[section]) {
            callback(section, varName);
        }
    }
}

template <typename T, typename ParameterType, typename... Args>
void ConfigManager::addConfig(ParamType paramType, const char *section, const char *varName, T *var, const char *description, uint8_t length, bool advanced,
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

    m_parameters.push_back({param, paramType, section, varName, advanced, saveLambda});
}

void ConfigManager::addConfigString(const char *section, const char *varName, std::string *var, size_t length, const char *description, bool advanced) {
    addConfig<std::string, StringParameter>(
        ParamType::String, section, varName, var, description, length, advanced,
        [this, varName](std::string &var) { var = m_preferences.getString(varName, var.c_str()).c_str(); },
        [](StringParameter *param, std::string &var) { var = param->getValue(); },
        [this, varName](std::string &var) { m_preferences.putString(varName, var.c_str()); });
}

void ConfigManager::addConfigInt(const char *section, const char *varName, int *var, const char *description, bool advanced) {
    addConfig<int, IntParameter>(
        ParamType::Int, section, varName, var, description, 10, advanced,
        [this, varName](int &var) { var = m_preferences.getInt(varName, var); },
        [](IntParameter *param, int &var) { var = param->getValue(); },
        [this, varName](int &var) { m_preferences.putInt(varName, var); });
}

void ConfigManager::addConfigBool(const char *section, const char *varName, bool *var, const char *description, bool advanced) {
    addConfig<bool, BoolParameter>(
        ParamType::Bool, section, varName, var, description, 2, advanced,
        [this, varName](bool &var) { var = m_preferences.getBool(varName, var); },
        [this](BoolParameter *param, bool &var) { var = param->getValue(this->m_wm); },
        [this, varName](bool &var) { m_preferences.putBool(varName, var); });
}

void ConfigManager::addConfigFloat(const char *section, const char *varName, float *var, const char *description, bool advanced) {
    addConfig<float, FloatParameter>(
        ParamType::Float, section, varName, var, description, 10, advanced,
        [this, varName](float &var) { var = m_preferences.getFloat(varName, var); },
        [](FloatParameter *param, float &var) { var = param->getValue(); },
        [this, varName](float &var) { m_preferences.putFloat(varName, var); });
}

void ConfigManager::addConfigColor(const char *section, const char *varName, int *var, const char *description, bool advanced) {
    addConfig<int, ColorParameter>(
        ParamType::Color, section, varName, var, description, 8, advanced,
        [this, varName](int &var) { var = m_preferences.getInt(varName, var); },
        [](ColorParameter *param, int &var) { var = param->getValue(); },
        [this, varName](int &var) { m_preferences.putInt(varName, var); });
}

void ConfigManager::addConfigComboBox(const char *section, const char *varName, int *var, String options[], int numOptions, const char *description, bool advanced) {
    addConfig<int, ComboBoxParameter>(
        ParamType::ComboBox, section, varName, var, description, 0, advanced,
        [this, varName](int &var) { var = m_preferences.getInt(varName, var); },
        [this](ComboBoxParameter *param, int &var) { var = param->getValue(this->m_wm); },
        [this, varName](int &var) { m_preferences.putInt(varName, var); },
        options, // Pass options array
        numOptions // Pass number of options
    );
}

std::string ConfigManager::getConfigString(const char *varName, std::string defaultValue) {
    return m_preferences.getString(varName, defaultValue.c_str()).c_str();
}

bool ConfigManager::getConfigBool(const char *varName, bool defaultValue) {
    return m_preferences.getBool(varName, defaultValue);
}

int ConfigManager::getConfigInt(const char *varName, int defaultValue) {
    return m_preferences.getInt(varName, defaultValue);
}

float ConfigManager::getConfigFloat(const char *varName, float defaultValue) {
    return m_preferences.getFloat(varName, defaultValue);
}

void ConfigManager::addOnChangeCallback(
    const char *section, const char *varName, const std::function<void(const char *section, const char *varName)> &callback) {
    m_changeCallbacks[makeKey(section, varName)].push_back(callback);
}

void ConfigManager::addOnChangeCallback(
    const char *section, const std::function<void(const char *section, const char *varName)> &callback) {
    m_changeCallbacks[section].push_back(callback);
}
