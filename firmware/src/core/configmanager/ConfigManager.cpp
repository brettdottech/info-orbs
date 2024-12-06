#include "ConfigManager.h"

ConfigManager *ConfigManager::m_instance = nullptr;

ConfigManager::ConfigManager(WiFiManager &wm) : m_wm(wm) {
    Serial.println("Constructing ConfigManager");
    if (!preferences.begin("config", false)) {
        Serial.println("Failed to initialize NVS in ConfigManager");
    } else {
        Serial.println("NVS initialized successfully in ConfigManager");
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

ConfigManager &ConfigManager::getInstance() {
    return *m_instance;
}

void ConfigManager::setupWiFiManager() {
    WiFiManagerParameter *startLabel = new WiFiManagerParameter(Utils::copyString("<H1>InfoOrbs Configuration</H1>"));
    m_wm.addParameter(startLabel);
    char lastClassName[30];
    for (auto &param : parameters) {
        if (!Utils::compareCharArrays(lastClassName, param.className)) {
            // New class variables -> add a separator
            WiFiManagerParameter *classLabel = new WiFiManagerParameter(Utils::createWithPrefixAndPostfix("<HR><H2 style='margin-block-end: 0;'>", param.className, "</H2>"));
            Serial.printf("New area: %s\n", param.className);
            m_wm.addParameter(classLabel);
            // WiFiManagerParameter *newLine = new WiFiManagerParameter("<br/>");
            // wm.addParameter(newLine);
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
        saveAllConfigs();
        // Restart to apply new config
        ESP.restart();
    });
}

void ConfigManager::loadAllConfigs() {
    // for (auto& param : parameters) {
    //     param.saveCallback(); // Load values into variables
    // }
}

void ConfigManager::saveAllConfigs() {
    // preferences.begin("config", false);
    for (auto &param : parameters) {
        param.saveCallback(); // Save variables to preferences
    }
    // preferences.end();
}

std::string ConfigManager::makeKey(const std::string &className, const std::string &varName) {
    return className + "_" + varName;
}

void ConfigManager::triggerChangeCallbacks(const std::string &className, const std::string &varName) {
    Serial.printf("triggerChangeCallbacks, c=%s, v=%s\n", className.c_str(), varName.c_str());
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

void ConfigManager::addConfigString(const std::string &className, const std::string &varName, char *var, size_t length, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    String value = preferences.getString(varNameBuffer, var);
    strncpy(var, value.c_str(), length);

    WiFiManagerParameter *param = new WiFiManagerParameter(varNameBuffer, descBuffer, var, length);
    parameters.push_back({param, CM_PARAM_TYPE_STRING, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var]() {
                              preferences.putString(varNameBuffer, var);
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

void ConfigManager::addConfigInt(const std::string &className, const std::string &varName, int *var, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    *var = preferences.getInt(varNameBuffer, *var);

    IntParameter *param = new IntParameter(varNameBuffer, descBuffer, *var);
    parameters.push_back({param, CM_PARAM_TYPE_INT, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var, param]() {
                              *var = param->getValue();
                              preferences.putInt(varNameBuffer, *var);
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

void ConfigManager::addConfigFloat(const std::string &className, const std::string &varName, float *var, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    *var = preferences.getFloat(varNameBuffer, *var);

    FloatParameter *param = new FloatParameter(varNameBuffer, descBuffer, *var);
    parameters.push_back({param, CM_PARAM_TYPE_FLOAT, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var, param]() {
                              *var = param->getValue();
                              preferences.putFloat(varNameBuffer, *var);
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

void ConfigManager::addConfigIP(const std::string &className, const std::string &varName, IPAddress *var, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    uint32_t ip = preferences.getUInt(varNameBuffer, (uint32_t) (*var));
    var->fromString(IPAddress(ip).toString());

    IPAddressParameter *param = new IPAddressParameter(varNameBuffer, descBuffer, *var);
    parameters.push_back({param, CM_PARAM_TYPE_IP, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var, param]() {
                              param->getValue(*var);
                              preferences.putUInt(varNameBuffer, (uint32_t) (*var));
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

void ConfigManager::addConfigBool(const std::string &className, const std::string &varName, bool *var, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    *var = preferences.getBool(varNameBuffer, *var);
    Serial.printf("%s loaded: %d (%d)\n", varNameBuffer, *var, var);
    BoolParameter *param = new BoolParameter(varNameBuffer, descBuffer, *var);
    parameters.push_back({param, CM_PARAM_TYPE_BOOL, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var, param]() {
                              // We can't use .getValue(), but we have to check the server return parameters
                              *var = this->m_wm.server->hasArg(varNameBuffer);
                              preferences.putBool(varNameBuffer, *var);
                              Serial.printf("%s saved: %d (%d)\n", varNameBuffer, *var, var);
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

void ConfigManager::addConfigColor(const std::string &className, const std::string &varName, int *var, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    *var = preferences.getInt(varNameBuffer, *var);
    Serial.printf("%s loaded: %d (%d)\n", varNameBuffer, *var, var);
    ColorParameter *param = new ColorParameter(varNameBuffer, descBuffer, *var);
    parameters.push_back({param, CM_PARAM_TYPE_COLOR, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var, param]() {
                              *var = param->getValue();
                              preferences.putInt(varNameBuffer, *var);
                              Serial.printf("%s saved: %d (%d)\n", varNameBuffer, *var, var);
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

void ConfigManager::addConfigComboBox(const std::string &className, const std::string &varName, int *var, String options[], int numOptions, const std::string &description) {
    char *classNameBuffer = Utils::copyString(className);
    char *varNameBuffer = Utils::copyString(varName);
    char *descBuffer = Utils::copyString(description);
    *var = preferences.getInt(varNameBuffer, *var);
    Serial.printf("%s loaded: %d (%d)\n", varNameBuffer, *var, var);
    ComboBoxParameter *param = new ComboBoxParameter(varNameBuffer, descBuffer, options, numOptions, *var);
    parameters.push_back({param, CM_PARAM_TYPE_COLOR, classNameBuffer, varNameBuffer, [this, classNameBuffer, varNameBuffer, var, param]() {
                              // We can't use .getValue(), but we have to check the server return parameters
                              if (this->m_wm.server->hasArg(varNameBuffer)) {
                                  String arg = this->m_wm.server->arg(varNameBuffer);
                                  Serial.println(arg);
                                  *var = arg.toInt();
                              };
                              preferences.putInt(varNameBuffer, *var);
                              Serial.printf("%s saved: %d (%d)\n", varNameBuffer, *var, var);
                              triggerChangeCallbacks(classNameBuffer, varNameBuffer);
                          }});
}

bool ConfigManager::getConfigBool(const std::string &varName, bool defaultValue) {
    bool val = preferences.getBool(varName.c_str(), defaultValue);
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
