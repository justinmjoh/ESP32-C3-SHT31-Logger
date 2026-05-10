#pragma once
#include <Arduino.h>
#include <Preferences.h>

struct AppConfig {
    String wifiSsid;
    String wifiPass;
    String hostname;
    String adminPass;
    String apiUrl;
    String loggerName;
    uint32_t logInterval; // minutes
};

class Config {
public:
    static bool isConfigured();
    static AppConfig load();
    static void saveSetup(const String& ssid, const String& pass,
                        const String& hostname, const String& adminPass);
    static void saveLogger(const AppConfig& cfg);
    static void clear();
};