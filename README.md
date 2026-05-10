# ESP32-C3-SHT31-Logger

Firmware for an ESP32-C3 SuperMini reading temperature and humidity from an SHT31 sensor and posting to a REST API at a configurable interval. Designed for multi-device deployment with per-device configuration via a browser UI.

## Hardware

- ESP32-C3 SuperMini
- SHT31 temperature/humidity sensor

## Wiring

| SHT31 | ESP32-C3 SuperMini |
|-------|--------------------|
| VCC   | 3.3V               |
| GND   | GND                |
| SDA   | GPIO8              |
| SCL   | GPIO9              |

## Setup

### 1. Flash the firmware

Open the project in VS Code with PlatformIO installed, then build and flash:

- Build: `Ctrl+Alt+B`
- Flash: `Ctrl+Alt+U`

### 2. Initial configuration

On first boot the device starts in setup mode, creating a WiFi access point named `Logger-Setup`. From any device:

1. Connect to the `Logger-Setup` network
2. Open a browser and navigate to `http://<gateway-ip>/initial-setup` (check your network settings for the AP gateway address)
3. Enter your WiFi credentials, a hostname for this device, and an admin password
4. Click **Save & Connect**

The device will restart and join your network.

### 3. Logger configuration

Once on your network, find the device's IP address via your router and navigate to:

```
http://<device-ip>/config
```

From here, enter your admin password and configure:

- **API URL** — the endpoint to POST readings to
- **Logger Name** — identifies this device in the logged data
- **Log Interval** — how often to post a reading (in minutes)

Use **Log Now** to trigger an immediate reading at any time.

## API Payload

Each reading posts the following JSON:

```json
{
  "loggerName": "your-logger-name",
  "temperature": 23.45,
  "humidity": 60.10
}
```

Temperature is in **Celsius**. Humidity is **% Relative Humidity (RH)**.

## Dependencies

Managed via PlatformIO:

- [Adafruit SHT31 Library](https://github.com/adafruit/Adafruit_SHT31)
- [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)
