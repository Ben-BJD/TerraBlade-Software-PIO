# AGENTS.md

## Key Commands
- **Build for ESP32**: Do not build for the device. Only run native tests.
- **Native testing**: `pio test -e native`
- **Upload to device**: Do not upload to the device. Only run native tests.

## Environment Notes
- Use `native` env for Unity tests (requires `-D NATIVE_TESTING`).
- ESP32 build uses custom partitions (`ota_partitions.csv`).
- WiFiManager@2.0.17 and ThingSpeak@2.1.1 required for board builds.
- ArduinoFake only used in native testing (ignored in ESP32 builds).

## Hardware & Pinout Rules (XIAO ESP32-C3) 
- **PROBE_POWER_PIN (GPIO 10)**: Parasitic power. MUST be explicitly turned HIGH before reading and LOW immediately after to maintain 0.0µA leakage.
- **PROBE_SIGNAL_PIN (GPIO 5)**: High-frequency input from TLC555. Read via interrupts/pulse counter.
- **RESET_BUTTON_PIN (GPIO 3)**: Deep sleep wake-up source. Triggers Service Mode / Captive Portal.
- **LED_PIN (GPIO 4)**: 573nm LED. Used for Service Mode feedback.
- **BATTERY_PIN (GPIO 0 / A0)**: Analog input for 220kΩ voltage divider.

## Architectural & Coding Standards
- **Interface-Driven Design**: Always use Dependency Injection. This is strictly required so `ArduinoFake` and mock classes can be used in the `native` testing environment.
- **Fail-Safe Power Management**: Power is the most precious resource. **Never** use infinite or blocking `while()` loops for network or sensor operations. Every operation must have a strict timeout.
- **Always Sleep**: On any caught error, network timeout, or hardware fault, the device must gracefully fail and fall back into Deep Sleep to prevent battery drain.
- **Separation of Concerns**: Keep PlatformIO libraries (`/lib`) strictly isolated. `Application.cpp` should act as the orchestrator, while logic remains decoupled inside `Sensor`, `TelemetryTransport`, `Schedule`, and `ServiceMode`.