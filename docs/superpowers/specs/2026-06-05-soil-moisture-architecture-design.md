---
Date: 2026-06-05
Topic: Soil Moisture Architecture Design
---

# Component Responsibilities

## Sensor Library
- **ISensor Interface**: Defines methods for frequency measurement and moisture calculation.
- **ConcreteSensor Class**: 
  - Manages PROBE_POWER_PIN (GPIO 10) sequencing (HIGH during read, LOW after).
  - Uses PROBE_SIGNAL_PIN (GPIO 5) with pulse counter for frequency.
  - Throwaway Read' (to clear startup noise) followed by a 'Batch Read' (averaging 5 readings of 20ms each)
  - Implements linear interpolation between `cal_dry_freq` (106000Hz) and `cal_wet_freq` (13000Hz).
  - Constrains output to 0-100% moisture.
  - Read the battery voltage on BATTERY_PIN (GPIO 0 / A0) using the 220kΩ voltage divider logic.

## TelemetryTransport Library
- **ITransport Interface**: Handles data transmission.
- **ThingSpeakTransport Class**:
  - Sends data to ThingSpeak

## Schedule Library
- Manages deep sleep intervals.

## ServiceMode Library
- Triggers captive portal on "Double-Tap" reset/power cycling.
- Collects WiFi credentials
- Optionally upload new firmware

## Application Facade
- Connects to WiFi with 10s timeout.
- Orchestrates boot flow:
  1. Check wake reason (timer/button)
  2. Read sensor, transmit data
  4. Handle errors centrally, trigger deep sleep on failure

# State Machine Flow

## Normal Boot Flow
1. Wake from deep sleep (timer)
2. Power sensor (GPIO 10 HIGH)
3. Read frequency (2s timeout)
4. The sensor must be powered OFF (GPIO 10 LOW) immediately after the read is complete
5. Calculate moisture (0-100% constrained)
6. Calculate the battery voltage
7. Connect to WiFi (10s timeout)
8. Transmit data (moisture, voltage, boot count)
9. Return to deep sleep

## Service Mode Flow
1. Trigger from a "Double-Tap" feature on the reset button/power cycling or if Wifi Credentials are missing
2. Launch captive portal (WiFiManager)
3. Turn on the LED_PIN (GPIO 4) to give the user visual feedback
4. Collect: WiFi SSID/password - Optionally upload new firmware
5. Save settings, reboot to normal mode

# Error Handling Strategy
- All timeouts/retries managed by Application facade
- On WiFi timeout (10s): log error, proceed to sleep
- On sensor timeout (2s): If the sensor fails or returns out-of-bounds frequencies, do NOT send a default reading. Abort the ThingSpeak transmission entirely and go straight to deep sleep. We do not want to push junk data to the cloud
- Any error triggers immediate deep sleep to preserve battery

# Testing Strategy
- Interfaces (ISensor, ITransport) mocked in native tests
- `-D NATIVE_TESTING` enables ArduinoFake mocks
- Unit tests verify:
  - Moisture calculation with edge frequencies
  - Timeout behavior in simulated failures
  - Service Mode configuration persistence