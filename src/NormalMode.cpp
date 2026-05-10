#include "NormalMode.h"
#include "Config.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Adafruit_SHT31.h>
#include <ESPmDNS.h>

static WebServer server(80);
static AppConfig cfg;
static Adafruit_SHT31 sht31;
static unsigned long lastPost = 0;

static void connectWifi() {
    WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPass.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
        delay(500);
        attempts++;
    }
}

static void postReading(float temp, float humidity) {
    if (WiFi.status() != WL_CONNECTED) return;
    HTTPClient http;
    http.begin(cfg.apiUrl);
    http.addHeader("Content-Type", "application/json");
    String body = "{\"loggerName\":\"" + cfg.loggerName +
                "\",\"temperature\":"  + String(temp, 2) +
                ",\"humidity\":"       + String(humidity, 2) + "}";
    http.POST(body);
    http.end();
}

static const char CONFIG_HTML[] PROGMEM = R"(
<!DOCTYPE html><html>
<head><title>Logger Config</title>
<style>
body{font-family:sans-serif;max-width:400px;margin:40px auto;padding:0 20px}
input{width:100%;padding:8px;margin:4px 0 12px;box-sizing:border-box}
button{width:100%;padding:10px;background:#2563eb;color:white;border:none;cursor:pointer;margin-bottom:8px}
.log-btn{background:#16a34a}
.readings{background:#f3f4f6;padding:12px;border-radius:4px;margin-bottom:20px}
.error{color:#dc2626;margin-bottom:12px}
</style></head>
<body>
<h2>Logger Config</h2>
<div class="readings"><strong>Current:</strong> %TEMP%&deg;C | %HUMIDITY%% RH</div>
%ERROR%
<form method="POST">
<label>API URL</label>
<input name="api_url" value="%API_URL%" required>
<label>Logger Name</label>
<input name="logger_name" value="%LOGGER_NAME%" required>
<label>Log Interval (minutes)</label>
<input name="log_interval" type="number" min="1" value="%LOG_INTERVAL%" required>
<label>Admin Password</label>
<input name="admin_pass" type="password" required>
<button type="submit" formaction="/config">Save</button>
<button type="submit" formaction="/log-now" class="log-btn">Log Now</button>
</form>
</body></html>
)";

static String buildConfigPage(const String& error = "") {
    float temp = sht31.readTemperature();
    float hum  = sht31.readHumidity();

    String page = String(CONFIG_HTML);
    page.replace("%TEMP%",         isnan(temp) ? "--" : String(temp, 1));
    page.replace("%HUMIDITY%",     isnan(hum)  ? "--" : String(hum, 1));
    page.replace("%API_URL%",      cfg.apiUrl);
    page.replace("%LOGGER_NAME%",  cfg.loggerName);
    page.replace("%LOG_INTERVAL%", String(cfg.logInterval));
    page.replace("%ERROR%",        error.isEmpty() ? "" : "<p class='error'>" + error + "</p>");
    return page;
}

void startNormalMode() {
    cfg = Config::load();
    sht31.begin(0x44);
    connectWifi();
    MDNS.begin(cfg.hostname.c_str());

    server.on("/", HTTP_GET, []() {
        server.sendHeader("Location", "/config");
        server.send(302);
    });

    server.on("/config", HTTP_GET, []() {
        server.send(200, "text/html", buildConfigPage());
    });

    server.on("/config", HTTP_POST, []() {
        if (server.arg("admin_pass") != cfg.adminPass) {
            server.send(200, "text/html", buildConfigPage("Incorrect password."));
            return;
        }
        cfg.apiUrl      = server.arg("api_url");
        cfg.loggerName  = server.arg("logger_name");
        cfg.logInterval = server.arg("log_interval").toInt();
        Config::saveLogger(cfg);
        server.send(200, "text/html", buildConfigPage());
    });

    server.on("/log-now", HTTP_POST, []() {
        if (server.arg("admin_pass") != cfg.adminPass) {
            server.send(200, "text/html", buildConfigPage("Incorrect password."));
            return;
        }
        float temp = sht31.readTemperature();
        float hum  = sht31.readHumidity();
        if (!isnan(temp) && !isnan(hum))
            postReading(temp, hum);
        server.sendHeader("Location", "/config");
        server.send(302);
    });

    server.begin();
    lastPost = millis();
}

void handleNormalLoop() {
    if (WiFi.status() != WL_CONNECTED)
        connectWifi();

    server.handleClient();

    unsigned long interval = (unsigned long)cfg.logInterval * 60UL * 1000UL;
    if (millis() - lastPost >= interval) {
        float temp = sht31.readTemperature();
        float hum  = sht31.readHumidity();
        if (!isnan(temp) && !isnan(hum))
            postReading(temp, hum);
        lastPost = millis();
    }
}