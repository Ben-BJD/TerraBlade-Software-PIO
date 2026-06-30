# 🌱 TerraBlade - Modular Soil Scout (Software)

---
- [TerraBlade Sensor - Hardware PCB](https://github.com/Ben-BJD/TerraBlade-Sensor-PCB)
---

[![Watch the Tutorial](https://img.shields.io/badge/Watch-YouTube_Tutorial-red?style=for-the-badge&logo=youtube)](https://www.youtube.com/watch?v=VNJ1A-dLCrM) [![Read the Article](https://img.shields.io/badge/Read-Website_Article-blue?style=for-the-badge&logo=firefox)](https://bj-dehaan-solutions.com.au/articles/arduino-esp32-iot/I-Let-an-AI-Code-My-ESP32-Sensor-Designing-a-Low-Power-IoT-Capacitive-Soil-Sensor-Part-2)

Professional-grade, ultra-low-power soil moisture sensor software for Seeed XIAO ESP32-C3. Companion repository to the [Hardware PCB](https://github.com/Ben-BJD/TerraBlade-Sensor-PCB) and featured in YouTube Series "Part 2: Building a Production-Grade IoT Sensor (Coming soon ...)".

## 🔋 Key Software Features
- **Zero-Leakage Power Management**: GPIO 10 (PROBE_POWER_PIN) explicitly toggled HIGH *only during* sensor reads, achieving **0.0µA leakage** in sleep
- **Double-Tap Service Mode**: Wake via "Double-Tap" battery pull to trigger captive portal (WiFiManager) for WiFi configuration and optionally upload new firmware
- **Hardware-Calibrated Moisture**: Frequency-to-moisture conversion using TLC555 oscillator (106000Hz dry → 13000Hz wet)
- **Fail-Safe Architecture**: All operations enforce strict timeouts (2s sensor, 10s WiFi), with *any error* triggering immediate deep sleep

## ⚙️ Hardware Pinout (XIAO ESP32-C3)
| GPIO | Function               | Critical Behavior                     |
|------|------------------------|---------------------------------------|
| 10   | PROBE_POWER_PIN        | MUST be HIGH *only during* sensor read |
| 5    | PROBE_SIGNAL_PIN       | High-frequency input from TLC555      |
| 4    | LED_PIN                | Service Mode visual feedback          |
| 0/A0 | BATTERY_PIN            | 220kΩ voltage divider measurement     |

## 📂 Project Structure
```
lib/
├── Sensor/               # ISensor interface + ConcreteSensor (parasitic power control)
├── TelemetryTransport/   # ITransport interface + ThingSpeakTransport (Could be expanded to support other cloud providers)
├── Schedule/             # Deep sleep interval management
├── ServiceMode/          # Captive portal via WiFiManager (Configure Wifi credentials and/or upload new firmware)
└── Application/          # DI orchestrator (boot flow, error handling)
```
*Interface-Driven Design* enables mocking hardware dependencies during native testing via `ArduinoFake`.

## 🛠️ Building & Testing
```bash
# Run native unit tests (Add -v flag for verbosity)
pio test -e native #(Run all test: useful for continuous integration)
pio test -e native -f components/test_{COMPONENT_NAME} #Test an individual component: Useful for issolated unit tests and debugging
# Run device unit tests (Add -v flag for verbosity and Serial logging)
pio test -e seeed_xiao_esp32c3 -f components/test_{COMPONENT_NAME} #I reccomend running the tests component by component on the device

# Build for hardware
pio run -e seeed_xiao_esp32c3
```
**Note**: Native tests use `ArduinoFake` for hardware mocking (see `test/` directory). Hardware builds require WiFi/ThingSpeak credentials.

## 🤖 AI Development Methodology
This codebase was co-developed using a **Driver & Navigator workflow**:
1. **AI as Navigator**: Generated C++ implementations under strict hardware constraints
2. **Human as Driver**: Manual compilation/testing with `pio test -e native`
3. **Guardrails**: Enforced zero-leakage power, timeout enforcement, and deep-sleep failover

Full prompt history and session logs available in [`/gen-ai`](gen-ai/README.md).

## 🔐 Secrets Management
Create `include/secrets.h` with:
```cpp
#define AP_SSID "{Captive Portal Access Point SSID}" //This will appear in your wifi network list when configuring
#define AP_PASSWORD "{Captive Portal Password}" //This will be used to connect to the config access point
#define TEST_WIFI_SSID "{Your Wifi SSID}"		    //replace with your WiFi network name (Only used for unit testing)
#define TEST_WIFI_PASSWORD "{Wifi Password}}"	    //replace with your WiFi password (Only used for unit testing)
//Thingspeak Configuration
#define THINGSPEAK_CH_ID 0000000			                            // replace 0000000 with your channel number
#define THINGSPEAK_WRITE_APIKEY "XYZ"    // replace XYZ with your channel write API Key
```
*Never commit this file - it's excluded via `.gitignore`.*