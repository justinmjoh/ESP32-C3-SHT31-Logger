#include "Config.h"

static const char* NS = "logger";

bool Config::isConfigured() {
    Preferences prefs;
    prefs.begin(NS, true);
    bool val = prefs.getBool("configured", false);
    prefs.end();
    return val;
}

AppConfig Config::load() {
    Preferences prefs;
    prefs.begin(NS, true);
    AppConfig cfg;
    cfg.wifiSsid    = prefs.getString("wifi_ssid", "");
    cfg.wifiPass    = prefs.getString("wifi_pass", "");
    cfg.hostname    = prefs.getString("hostname", "logger");
    cfg.adminPass   = prefs.getString("admin_pass", "");
    cfg.apiUrl      = prefs.getString("api_url", "");
    cfg.loggerName  = prefs.getString("logger_name", "");
    cfg.logInterval = prefs.getUInt("log_interval", 5);
    prefs.end();
    return cfg;
}

void Config::saveSetup(const String& ssid, const String& pass,
                        const String& hostname, const String& adminPass) {
    Preferences prefs;
    prefs.begin(NS, false);
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);
    prefs.putString("hostname", hostname);
    prefs.putString("admin_pass", adminPass);
    prefs.putBool("configured", true);
    prefs.end();
}

void Config::saveLogger(const AppConfig& cfg) {
    Preferences prefs;
    prefs.begin(NS, false);
    prefs.putString("api_url", cfg.apiUrl);
    prefs.putString("logger_name", cfg.loggerName);
    prefs.putUInt("log_interval", cfg.logInterval);
    prefs.end();
}

void Config::clear() {
    Preferences prefs;
    prefs.begin(NS, false);
    prefs.clear();
    prefs.end();
}