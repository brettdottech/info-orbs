#ifdef MQTT_WIDGET_HOST

    #include "MQTTWidget.h"

// Initialize the static instance pointer
MQTTWidget *MQTTWidget::instance = nullptr;

// Static callback proxy to forward MQTT messages to the class instance
void MQTTWidget::staticCallback(char *topic, byte *payload, unsigned int length) {
    if (instance) {
        instance->callback(topic, payload, length);
    }
}

// Constructor
MQTTWidget::MQTTWidget(ScreenManager &manager, const String &host, uint16_t port)
    : Widget(manager), mqttHost(host), mqttPort(port), mqttClient(wifiClient) {

    // Assign the current instance to the static pointer
    instance = this;

    // Set MQTT broker server and port
    mqttClient.setServer(mqttHost.c_str(), mqttPort);
    mqttClient.setBufferSize(2048);

    // Set the static callback proxy
    mqttClient.setCallback(staticCallback);
}

// Helper function to map color strings to uint16_t color values
uint16_t MQTTWidget::getColorFromString(const String &colorStr) {
    if (colorStr.equalsIgnoreCase("TFT_BLACK")) {
        return TFT_BLACK;
    } else if (colorStr.equalsIgnoreCase("TFT_NAVY")) {
        return TFT_NAVY;
    } else if (colorStr.equalsIgnoreCase("TFT_DARKGREEN")) {
        return TFT_DARKGREEN;
    } else if (colorStr.equalsIgnoreCase("TFT_DARKCYAN")) {
        return TFT_DARKCYAN;
    } else if (colorStr.equalsIgnoreCase("TFT_MAROON")) {
        return TFT_MAROON;
    } else if (colorStr.equalsIgnoreCase("TFT_PURPLE")) {
        return TFT_PURPLE;
    } else if (colorStr.equalsIgnoreCase("TFT_OLIVE")) {
        return TFT_OLIVE;
    } else if (colorStr.equalsIgnoreCase("TFT_LIGHTGREY")) {
        return TFT_LIGHTGREY;
    } else if (colorStr.equalsIgnoreCase("TFT_DARKGREY")) {
        return TFT_DARKGREY;
    } else if (colorStr.equalsIgnoreCase("TFT_BLUE")) {
        return TFT_BLUE;
    } else if (colorStr.equalsIgnoreCase("TFT_GREEN")) {
        return TFT_GREEN;
    } else if (colorStr.equalsIgnoreCase("TFT_CYAN")) {
        return TFT_CYAN;
    } else if (colorStr.equalsIgnoreCase("TFT_RED")) {
        return TFT_RED;
    } else if (colorStr.equalsIgnoreCase("TFT_MAGENTA")) {
        return TFT_MAGENTA;
    } else if (colorStr.equalsIgnoreCase("TFT_YELLOW")) {
        return TFT_YELLOW;
    } else if (colorStr.equalsIgnoreCase("TFT_WHITE")) {
        return TFT_WHITE;
    } else if (colorStr.equalsIgnoreCase("TFT_ORANGE")) {
        return TFT_ORANGE;
    } else if (colorStr.equalsIgnoreCase("TFT_GREENYELLOW")) {
        return TFT_GREENYELLOW;
    } else if (colorStr.equalsIgnoreCase("TFT_PINK")) {
        return TFT_PINK;
    } else if (colorStr.equalsIgnoreCase("TFT_BROWN")) {
        return TFT_BROWN;
    } else if (colorStr.equalsIgnoreCase("TFT_GOLD")) {
        return TFT_GOLD;
    } else if (colorStr.equalsIgnoreCase("TFT_SILVER")) {
        return TFT_SILVER;
    } else if (colorStr.equalsIgnoreCase("TFT_SKYBLUE")) {
        return TFT_SKYBLUE;
    } else if (colorStr.equalsIgnoreCase("TFT_VIOLET")) {
        return TFT_VIOLET;
    }
    // Add more color mappings as needed

    return TFT_BLACK; // Default color if unknown
}

// Setup method
void MQTTWidget::setup() {
    //    Serial.println("Inside setup method");
    /*
        // Initialize MQTT connection
        reconnect();

        // Subscribe to the setup topic
        if (mqttClient.connected()) {
            mqttClient.subscribe(MQTT_SETUP_TOPIC);
            Serial.println("Subscribed to setup topic1: " + String(MQTT_SETUP_TOPIC));
        }
        // Additional setup (e.g., initializing display elements) can be added here
    */
}

// Update method
void MQTTWidget::update(bool force) {
    //    Serial.println("Inside update method - " + String(mqttClient.connected()));

    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop(); // Process incoming MQTT messages
}

// Draw method: Draws all orbs (can be used for initial drawing or full refresh)
void MQTTWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);

    if (force) {
        for (const auto &orb : orbConfigs) {
            drawOrb(orb.orbid);
        }
    }
}

// ChangeMode method
void MQTTWidget::changeMode() {
    // Implement mode changes if applicable
    draw(true);
}

// Callback function for MQTT messages
void MQTTWidget::callback(char *topic, byte *payload, unsigned int length) {
    // Convert payload to String
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char) payload[i];
    }

    String receivedTopic = String(topic);
    Serial.print("Message arrived [");
    Serial.print(receivedTopic);
    Serial.print("]: ");
    Serial.println(message);

    if (receivedTopic.equals(MQTT_SETUP_TOPIC)) {
        handleSetupMessage(message);
    } else {
        // Handle data messages for orbs
        auto it = orbDataMap.find(receivedTopic);
        if (it != orbDataMap.end()) {
            // Identify the orb configuration associated with this topic
            OrbConfig *orb = nullptr;
            for (auto &o : orbConfigs) {
                if (o.topicSrc.equals(receivedTopic)) {
                    orb = &o;
                    break;
                }
            }

            if (orb) {
                if (orb->jsonField.length() > 0) {
                    // The orb expects a specific JSON field
                    JsonDocument dataDoc;
                    DeserializationError dataError = deserializeJson(dataDoc, message);
                    if (dataError) {
                        Serial.print("Failed to parse data JSON: ");
                        Serial.println(dataError.c_str());
                        return;
                    }

                    // Extract the specified JSON field using dynamic path traversal
                    JsonVariant fieldValue = dataDoc.as<JsonVariant>();
                    char path[100];
                    strcpy(path, orb->jsonField.c_str());
                    char *token = strtok(path, ".");

                    // Traverse the path tokens to get the desired field value
                    while (token != nullptr && !fieldValue.isNull()) {
                        // Check if the token contains an array index (e.g., "power_delivered[0]")
                        char *arrayBracket = strchr(token, '[');
                        if (arrayBracket) {
                            *arrayBracket = '\0'; // Null terminate to get the key part
                            fieldValue = fieldValue[token]; // Access the array key

                            // Extract the index part
                            int index = atoi(arrayBracket + 1); // Get the number after '['
                            if (fieldValue.is<JsonArray>()) {
                                fieldValue = fieldValue[index]; // Access the array element by index
                            } else {
                                Serial.println("Error: Expected an array for " + String(token));
                                return;
                            }
                        } else {
                            // Regular object traversal
                            fieldValue = fieldValue[token];
                        }
                        token = strtok(nullptr, ".");
                    }

                    // If we successfully extracted the value
                    if (!fieldValue.isNull()) {
                        String extractedValue;
                        if (fieldValue.is<float>()) {
                            extractedValue = String(fieldValue.as<float>(), 3); // 3 decimal places
                        } else if (fieldValue.is<int>()) {
                            extractedValue = String(fieldValue.as<int>());
                        } else if (fieldValue.is<const char *>()) {
                            extractedValue = String(fieldValue.as<const char *>());
                        } else {
                            extractedValue = String(fieldValue.as<String>());
                        }

                        // Compare with the stored value to decide if we need to update
                        if (orb->lastValuesMap[orb->jsonField] != extractedValue) {
                            // Store the new value
                            orb->lastValuesMap[orb->jsonField] = extractedValue;

                            // Update the display only if the value has actually changed
                            it->second = extractedValue;
                            Serial.println("Parsed " + orb->jsonField + ": " + extractedValue);

                            // Redraw the orb with updated data
                            drawOrb(orb->orbid);
                        } else {
                            Serial.println("No change detected for field: " + orb->jsonField);
                        }
                    } else {
                        Serial.println("JSON field '" + orb->jsonField + "' not found in payload.");
                        return;
                    }
                } else {
                    // The orb does not expect a JSON field; use the entire payload
                    if (it->second != message) {
                        it->second = message;
                        Serial.println("Updated data for " + receivedTopic + ": " + message);
                        drawOrb(orb->orbid);
                    } else {
                        Serial.println("No change detected for topic: " + receivedTopic);
                    }
                }
            } else {
                Serial.println("No orb configuration found for topic: " + receivedTopic);
            }
        } else {
            Serial.println("Received message for unknown topic: " + receivedTopic);
        }
    }
}

// Handle setup message to configure orbs
void MQTTWidget::handleSetupMessage(const String &message) {
    //    Serial.println("Handling setup message...");

    // Parse JSON configuration
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.print("Failed to parse setup JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Clear existing configurations and data
    orbConfigs.clear();
    orbDataMap.clear();

    // Parse "orbs" array
    JsonArray orbs = doc["orbs"].as<JsonArray>();
    for (JsonObject orbObj : orbs) {
        OrbConfig config;
        config.orbid = orbObj["orbid"].as<int>();
        config.topicSrc = orbObj["topicsrc"].as<String>();
        config.orbvalunit = orbObj["orbvalunit"].as<String>();
        config.orbdesc = orbObj["orbdesc"].as<String>();
        config.xpostxt = orbObj["xpostxt"].as<int>();
        config.ypostxt = orbObj["ypostxt"].as<int>();
        config.xposval = orbObj["xposval"].as<int>();
        config.yposval = orbObj["yposval"].as<int>();
        config.orbsize = orbObj["orbsize"].as<int>();
        String bgColorStr = orbObj["orb-bg"].as<String>();
        String textColorStr = orbObj["orb-textcol"].as<String>();

        // Parse "jsonfield" (optional)
        if (orbObj["jsonfield"].is<String>()) {
            config.jsonField = orbObj["jsonfield"].as<String>();
        } else {
            config.jsonField = ""; // Default to empty if not provided
        }

        // Convert color strings to actual color values using helper function
        config.orbBgColor = getColorFromString(bgColorStr);
        config.orbTextColor = getColorFromString(textColorStr);

        orbConfigs.push_back(config);
        Serial.println("Configured Orb: " + String(config.orbid) + " -> " + config.orbdesc);

        // Initialize data map with empty strings
        orbDataMap[config.topicSrc] = "";
    }

    // Subscribe to all configured topics
    subscribeToOrbs();

    // Trigger a redraw to display the configured orbs
    draw(true);
}

// Subscribe to all orb topics
void MQTTWidget::subscribeToOrbs() {
    //    Serial.println("Inside subscribeToOrbs method");

    for (const auto &orb : orbConfigs) {
        bool success = mqttClient.subscribe(orb.topicSrc.c_str());
        if (success) {
            Serial.println("Subscribed to topic: " + orb.topicSrc);
        } else {
            Serial.println("Failed to subscribe to topic: " + orb.topicSrc);
        }
    }
}

// Handle MQTT reconnection
void MQTTWidget::reconnect() {
    //    Serial.println("Inside reconnect method");

    // Loop until reconnected
    while (!mqttClient.connected()) {
        Serial.println("Attempting MQTT connection...");

        // Generate a random client ID
        String clientId = "MQTTWidgetClient-";
        clientId += String(random(0xffff), HEX);

        bool connected;

        // Check if username and password are provided
        if (strlen(MQTT_WIDGET_USER) > 0 && strlen(MQTT_WIDGET_PASS) > 0) {
            // Attempt to connect with username and password
            connected = mqttClient.connect(clientId.c_str(), MQTT_WIDGET_USER, MQTT_WIDGET_PASS);
            Serial.println("Attempting MQTT connection with authentication...");
        } else {
            // Attempt to connect without authentication
            connected = mqttClient.connect(clientId.c_str());
            Serial.println("Attempting MQTT connection without authentication...");
        }

        // Check the result of the connection attempt
        if (connected) {
            Serial.println("MQTT connected");
            // Once connected, subscribe to the setup topic
            if (mqttClient.subscribe(MQTT_SETUP_TOPIC)) {
                Serial.println("Subscribed to setup topic2: " + String(MQTT_SETUP_TOPIC));
            } else {
                Serial.println("Failed to subscribe to setup topic: " + String(MQTT_SETUP_TOPIC));
            }
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying again
            delay(5000);
        }
    }
}

// New method to draw a single orb based on orbid
void MQTTWidget::drawOrb(int orbid) {
    //    Serial.println("Inside drawOrb method");

    // Select the screen corresponding to the orbid
    m_manager.selectScreen(orbid);

    // Find the orb configuration
    OrbConfig *orb = nullptr;
    for (auto &o : orbConfigs) {
        if (o.orbid == orbid) {
            orb = &o;
            break;
        }
    }

    if (orb == nullptr) {
        Serial.println("Orb not found for orbid: " + String(orbid));
        return;
    }

    m_manager.fillScreen(orb->orbBgColor);

    // Define the position and size of the orb (adjust as needed)
    int x = 0; // Starting X position
    int y = 0; // Starting Y position
    int width = 240; // Width of the orb
    int height = 240; // Height of the orb
    int centre = 120;
    int screenWidth = SCREEN_SIZE;
    // int screenHeight = display.height();

    // Clear the display area with the background color
    // display.fillRect(x, y, screenWidth, screenHeight, orb->orbBgColor);

    // Set text properties
    m_manager.setFontColor(orb->orbTextColor, orb->orbBgColor);
    // m_manager.setTextSize(orb->orbsize);

    // Display orb description/title
    // display.drawString(orb->orbdesc, centre, orb->xpostxt, orb->ypostxt);
    m_manager.drawString(orb->orbdesc, orb->xpostxt, orb->ypostxt, orb->orbsize, Align::MiddleCenter);
    // m_manager.drawString(orb->orbdesc, centre, orb->ypostxt, orb->orbsize, Align::MiddleCenter);

    // Display orb data
    String data = orbDataMap[orb->topicSrc];
    // display.drawString(data + orb->orbvalunit, centre, orb->xposval, orb->yposval);
    m_manager.drawString(data + orb->orbvalunit, orb->xposval, orb->yposval, orb->orbsize, Align::MiddleCenter);
}

String MQTTWidget::getName() {
    return "MQTTWidget";
}

void MQTTWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        changeMode();
}

#endif
