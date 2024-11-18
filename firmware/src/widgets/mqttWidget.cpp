// mqttWidget.cpp

#include "widgets/mqttWidget.h"

// Initialize the static instance pointer
MQTTWidget* MQTTWidget::instance = nullptr;

// Static callback proxy to forward MQTT messages to the class instance
void MQTTWidget::staticCallback(char* topic, byte* payload, unsigned int length) {
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
    
    if (force)
    {
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
void MQTTWidget::callback(char* topic, byte* payload, unsigned int length) {
//    Serial.println("Inside callback method");
    
    // Convert payload to String
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
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
            OrbConfig* orb = nullptr;
            for (auto &o : orbConfigs) {
                if (o.topicSrc.equals(receivedTopic)) {
                    orb = &o;
                    break;
                }
            }

            if (orb) {
                if (orb->jsonField.length() > 0) {
                    // The orb expects a specific JSON field
                    StaticJsonDocument<2048> dataDoc;
                    DeserializationError dataError = deserializeJson(dataDoc, message);
                    if (dataError) {
                        Serial.print("Failed to parse data JSON: ");
                        Serial.println(dataError.c_str());
                        return;
                    }

                    // Extract the specified JSON field
                    if (dataDoc.containsKey(orb->jsonField)) {
                        JsonVariant fieldValue = dataDoc[orb->jsonField];
                        // Convert the field value to String
                        String extractedValue;
                        if (fieldValue.is<float>()) {
                            extractedValue = String(fieldValue.as<float>(), 2); // 2 decimal places
                        } else if (fieldValue.is<int>()) {
                            extractedValue = String(fieldValue.as<int>());
                        } else if (fieldValue.is<const char*>()) {
                            extractedValue = String(fieldValue.as<const char*>());
                        } else {
                            extractedValue = fieldValue.as<String>();
                        }

                        it->second = extractedValue;
                        Serial.println("Parsed " + orb->jsonField + ": " + extractedValue);
                    } else {
                        Serial.println("JSON field '" + orb->jsonField + "' not found in payload.");
                        // Optionally, decide how to handle missing fields
                        return;
                    }
                } else {
                    // The orb does not expect a JSON field; use the entire payload
                    it->second = message;
                    Serial.println("Updated data for " + receivedTopic + ": " + message);
                }

                // Redraw the orb with updated data
                drawOrb(orb->orbid);
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
    StaticJsonDocument<2048> doc;
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
        if (orbObj.containsKey("jsonfield")) {
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

    // Access the display
    TFT_eSPI &display = m_manager.getDisplay();

    // Find the orb configuration
    OrbConfig* orb = nullptr;
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

    display.fillScreen(TFT_BLACK);

    // Define the position and size of the orb (adjust as needed)
    int x = 0; // Starting X position
    int y = 0; // Starting Y position
    int width = 240;  // Width of the orb
    int height = 240;  // Height of the orb
    int centre = 120;
    int screenWidth = display.width();
    int screenHeight = display.height();

    // Clear the display area with the background color
    display.fillRect(x, y, screenWidth, screenHeight, orb->orbBgColor);

    // Set text properties
    display.setTextColor(orb->orbTextColor, orb->orbBgColor);
    display.setTextSize(orb->orbsize);
    //display.setTextDatum(MC_DATUM); // Middle Center Datum

    display.setTextColor(TFT_WHITE);

    // Display orb description/title
    display.drawString(orb->orbdesc, centre, orb->xpostxt, orb->ypostxt);
//    display.drawString(orb->orbdesc, centre, 118, 1);

    // Display orb data
    String data = orbDataMap[orb->topicSrc];
    /*
    display.setCursor(orb->xposval, orb->yposval);
    display.println(data + orb->orbvalunit);
*/
    display.drawString(data + orb->orbvalunit, centre, orb->xposval, orb->yposval);

}

String MQTTWidget::getName() {
    return "MQTTWidget";
}

void MQTTWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        changeMode();
}