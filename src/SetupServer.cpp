#include "SetupServer.h"
#include "Config.h"
#include <WiFi.h>
#include <WebServer.h>

static WebServer server(80);

static const char SETUP_HTML[] PROGMEM = R"(
<!DOCTYPE html><html>
<head><title>Logger Setup</title>
<style>
body{font-family:sans-serif;max-width:400px;margin:40px auto;padding:0 20px}
input{width:100%;padding:8px;margin:4px 0 12px;box-sizing:border-box}
button{width:100%;padding:10px;background:#2563eb;color:white;border:none;cursor:pointer}
</style></head>
<body>
<h2>Logger Initial Setup</h2>
<form method="POST" action="/initial-setup">
<label>WiFi SSID</label>
<input name="ssid" required>
<label>WiFi Password</label>
<input name="pass" type="password">
<label>Hostname</label>
<input name="hostname" value="logger" required>
<label>Admin Password</label>
<input name="admin_pass" type="password" required>
<label>Confirm Admin Password</label>
<input name="admin_pass2" type="password" required>
<button type="submit">Save &amp; Connect</button>
</form>
</body></html>
)";

void startSetupMode() {
    WiFi.softAP("Logger-Setup");

    server.on("/", HTTP_GET, []() {
        server.sendHeader("Location", "/initial-setup");
        server.send(302);
    });

    server.on("/initial-setup", HTTP_GET, []() {
        server.send(200, "text/html", SETUP_HTML);
    });

    server.on("/initial-setup", HTTP_POST, []() {
        String ssid       = server.arg("ssid");
        String pass       = server.arg("pass");
        String hostname   = server.arg("hostname");
        String adminPass  = server.arg("admin_pass");
        String adminPass2 = server.arg("admin_pass2");

        if (ssid.isEmpty() || hostname.isEmpty() || adminPass.isEmpty()) {
            server.send(400, "text/html", "<h2>All fields required.</h2><a href='/initial-setup'>Back</a>");
            return;
        }
        if (adminPass != adminPass2) {
            server.send(400, "text/html", "<h2>Passwords do not match.</h2><a href='/initial-setup'>Back</a>");
            return;
        }

        Config::saveSetup(ssid, pass, hostname, adminPass);
        server.send(200, "text/html", "<h2>Saved. Device restarting...</h2>");
        delay(2000);
        ESP.restart();
    });

    server.begin();
}

void handleSetupLoop() {
    server.handleClient();
}