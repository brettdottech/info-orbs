#include "MainHelper.h"

Button buttonLeft(BUTTON_LEFT);
Button buttonOK(BUTTON_OK);
Button buttonRight(BUTTON_RIGHT);

// initialize static members
WiFiManager *MainHelper::s_wifiManager = nullptr;
ConfigManager *MainHelper::s_configManager = nullptr;
WidgetSet *MainHelper::s_widgetSet = nullptr;
#ifdef WIDGET_CYCLE_DELAY
int MainHelper::s_widgetCycleDelay = WIDGET_CYCLE_DELAY; // Automatically cycle widgets every X seconds, set to 0 to disable
#else
int MainHelper::s_widgetCycleDelay = 0;
#endif
unsigned long MainHelper::s_widgetCycleDelayPrev = 0;
bool MainHelper::s_invertedOrbs = INVERTED_ORBS;

void MainHelper::init(WiFiManager *wm, ConfigManager *cm, WidgetSet *ws) {
    s_wifiManager = wm;
    s_configManager = cm;
    s_widgetSet = ws;
}

/**
 * The ISR handlers must be static
 */
void MainHelper::isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void MainHelper::isrButtonChangeMiddle() { buttonOK.isrButtonChange(); }
void MainHelper::isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void MainHelper::setupButtons() {
    buttonLeft.begin();
    buttonOK.begin();
    buttonRight.begin();

    attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), isrButtonChangeLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_OK), isrButtonChangeMiddle, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), isrButtonChangeRight, CHANGE);
}

void MainHelper::setupConfig() {
    s_configManager->addConfigInt("General", "widgetCycDelay", &s_widgetCycleDelay, "Automatically cycle widgets every X seconds, set to 0 to disable");
    s_configManager->addConfigBool("General", "invertedOrbs", &s_invertedOrbs, "Inverted Orbs (enable if using InfoOrbs upside down)");
}

void MainHelper::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (state == BTN_NOTHING) {
        // nothing to do
        return;
    }
    // Reset cycle timer whenever a button is pressed
    if (buttonId == BUTTON_LEFT && state == BTN_SHORT) {
        // Left short press cycles widgets backward
        Serial.println("Left button short pressed -> switch to prev Widget");
        s_widgetCycleDelayPrev = millis();
        s_widgetSet->prev();
    } else if (buttonId == BUTTON_RIGHT && state == BTN_SHORT) {
        // Right short press cycles widgets forward
        Serial.println("Right button short pressed -> switch to next Widget");
        s_widgetCycleDelayPrev = millis();
        s_widgetSet->next();
    } else {
        // Everying else that is not BTN_NOTHING will be forwarded to the current widget
        if (buttonId == BUTTON_LEFT) {
            Serial.printf("Left button pressed, state=%d\n", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_LEFT, state);
        } else if (buttonId == BUTTON_OK) {
            Serial.printf("Middle button pressed, state=%d\n", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_OK, state);
        } else if (buttonId == BUTTON_RIGHT) {
            Serial.printf("Right button pressed, state=%d\n", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_RIGHT, state);
        }
    }
}

void MainHelper::checkButtons() {
    ButtonState leftState = buttonLeft.getState();
    if (leftState != BTN_NOTHING) {
        buttonPressed(BUTTON_LEFT, leftState);
    }
    ButtonState middleState = buttonOK.getState();
    if (middleState != BTN_NOTHING) {
        buttonPressed(BUTTON_OK, middleState);
    }
    ButtonState rightState = buttonRight.getState();
    if (rightState != BTN_NOTHING) {
        buttonPressed(BUTTON_RIGHT, rightState);
    }
}

void MainHelper::checkCycleWidgets() {
    if (s_widgetSet && s_widgetCycleDelay > 0 && (s_widgetCycleDelayPrev == 0 || (millis() - s_widgetCycleDelayPrev) >= s_widgetCycleDelay * 1000)) {
        s_widgetSet->next();
        s_widgetCycleDelayPrev = millis();
    }
}

// Handle simulated button state
void MainHelper::handleEndpointButton() {
    if (s_wifiManager->server->hasArg("name") && s_wifiManager->server->hasArg("state")) {
        String inButton = s_wifiManager->server->arg("name");
        String inState = s_wifiManager->server->arg("state");
        uint8_t buttonId = Button::stringToButtonId(inButton);
        ButtonState state = Button::stringToButtonState(inState);

        if (buttonId != 0 && state != BTN_NOTHING) {
            buttonPressed(buttonId, state);
            s_wifiManager->server->send(200, "text/plain", "OK " + inButton + "/" + inState + " -> " + String(buttonId) + "/" + String(state));
            return;
        }
    }
    s_wifiManager->server->send(500, "text/plain", "ERR");
}

// Show button web page
void MainHelper::handleEndpointButtons() {
    String msg = "<html><body style='background: black';><table>";
    String buttons[] = {"left", "middle", "right"};
    String states[] = {"short", "medium", "long"};
    int numButtons = 3; // number of buttons
    int numStates = 3; // number of states
    for (int s = 0; s < numStates; s++) {
        msg += "<tr>";
        for (int b = 0; b < numButtons; b++) {
            msg += "<td style='padding: 10px;'><button style='height: 50px; width: 150px;' onclick=\"sendReq('" + buttons[b] + "', '" + states[s] + "')\">" + buttons[b] + " " + states[s] + "</button></td>";
        }
        msg += "</tr>";
    }
    msg += "</table><script>function sendReq(name, state) {fetch(`/button?name=${name}&state=${state}`);}</script></body></html>";
    s_wifiManager->server->send(200, "text/html", msg);
}

void MainHelper::setupWebPortalEndpoints() {
    // To simulate button presses call e.g. http://<ip>/button?name=right&state=short
    s_wifiManager->server->on("/button", handleEndpointButton);
    s_wifiManager->server->on("/buttons", handleEndpointButtons);
}

void MainHelper::showWelcome(ScreenManager *screenManager) {
    screenManager->fillAllScreens(TFT_BLACK);
    screenManager->setFontColor(TFT_WHITE);

    screenManager->selectScreen(0);
    screenManager->drawCentreString("Welcome", ScreenCenterX, ScreenCenterY, 29);

    screenManager->selectScreen(1);
    screenManager->drawCentreString("Info Orbs", ScreenCenterX, ScreenCenterY - 50, 22);
    screenManager->drawCentreString("by", ScreenCenterX, ScreenCenterY - 5, 22);
    screenManager->drawCentreString("brett.tech", ScreenCenterX, ScreenCenterY + 30, 22);
    screenManager->setFontColor(TFT_RED);
    screenManager->drawCentreString("version: 1.1.0", ScreenCenterX, ScreenCenterY + 65, 14);

    screenManager->selectScreen(2);
    screenManager->drawJpg(0, 0, logo_start, logo_end - logo_start);
}

void MainHelper::resetCycleTimer() {
    s_widgetCycleDelayPrev = millis();
}