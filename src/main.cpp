#include <Arduino.h>
#include <Wire.h>
#include "Config.h"
#include "SetupServer.h"
#include "NormalMode.h"

static bool setupMode = false;

void setup() {
    Wire.begin();
    if (!Config::isConfigured()) {
        setupMode = true;
        startSetupMode();
    } else {
        startNormalMode();
    }
}

void loop() {
    if (setupMode)
        handleSetupLoop();
    else
        handleNormalLoop();
}