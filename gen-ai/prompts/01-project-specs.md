---
Date: 2026-06-03
Topic: Initial Project Planning
Tool: OpenCode (Qwen3.6 + Obra)
---

# Background

This project expands on a simple proof-of-concept test program written for the "TerraBlade" (Modular Soil Scout) hardware project: 
- Hardware Project (https://github.com/Ben-BJD/TerraBlade-Sensor-PCB)
- Test Program (https://github.com/Ben-BJD/Terrablade-Test-Program-PIO-Arduino)

We are building a robust, reliable, full-scale object-oriented C++ software project for the hardware. This software will be featured in "Part 2" of a YouTube series focusing on transforming an amateur breadboard prototype into a reliable, professional-grade IoT device with a 2+ year battery life.

# Hardware Context & Pinout

The hardware is a custom PCBA built around the **Seeed Studio XIAO ESP32-C3**. Power is supplied by 3x AA batteries.

*   `GPIO 10` (PROBE_POWER_PIN): Controls parasitic power to the TLC555 timer. Must be pulled HIGH to power the sensor, and LOW to kill power (0.0µA leakage).
*   `GPIO 5`  (PROBE_SIGNAL_PIN): Receives the 2.1MHz frequency from the 555 timer. Read via hardware pulse counter / interrupts.
*   `GPIO 3`  (RESET_BUTTON_PIN): Tactile push button. Used as a hardware wake-up source from deep sleep to trigger "Service Mode".
*   `GPIO 4`  (LED_PIN): 573nm Yellow-Green Config LED.
*   `GPIO 0 / A0` (BATTERY_PIN): Analog pin reading a 1% 220kΩ voltage divider for battery level.

# Core Features (The "What")

Refer to the "TODO" comments in the Application facade at: `@/lib/Application/Application.cpp`. The new architecture must implement:

1.  **Intelligent Sensor Polling & Calibration:**
    *   Handle the parasitic power sequencing (Power ON -> Throwaway Read -> Batch Average Read -> Power OFF).
    *   Implement calibration logic to map raw frequencies (e.g., 106,000Hz dry air, 13,000Hz water) to usable 0-100% soil moisture percentages.
    *   Range checks and hardware validation (handle severed traces or short circuits gracefully).
2.  **Robust Networking & Telemetry Transport:**
    *   Retry logic and strict timeouts for WiFi connections to prevent battery drain if the router is down.
    *   Retry logic and error state handling for transmitting data (ThingSpeak). 
3.  **Service Mode (Captive Portal):**
    *   When the device wakes via the physical button (GPIO 3), it should bypass normal telemetry and enter "Service Mode".
    *   Launch a WiFiManager/Captive Portal to allow the user to input WiFi credentials and calibration baselines headlessly via their phone.
    *   Provide visual feedback via the LED (GPIO 4) to indicate it is in Config Mode.
4.  **Fail-Safe Power Management:**
    *   Strict Deep Sleep scheduling. The ESP32 must *never* get stuck in an infinite loop. It must always fall back to Deep Sleep on failures.

# Software Scaffold (The "How")

A PlatformIO project has already been created with isolated core component libraries. We rely heavily on interface-driven design (Dependency Injection) to allow for native unit testing.

*   `lib/Application`: Application facade abstraction. Orchestrates the flow between sleep, read, connect, and send.
*   `lib/Schedule`: Responsible for waking, managing the sleep interval, identifying the wake-up reason (Timer vs. Button), and sending the device back to deep sleep.
*   `lib/Sensor`: Measures the hardware frequency via interrupts, calculates moisture percentage, and reads battery voltage.
*   `lib/ServiceMode`: Triggers the WiFi captive portal, handles user inputs, and supports future OTA firmware updates.
*   `lib/TelemetryTransport`: Responsible for securely transmitting our data payloads to the cloud (ThingSpeak/MQTT).

@/include/secrets.h includes sensitive config variables
@/include/terrablade_config.h includes standard config variables

# Testing Constraints

The components have unit tests under `/test/components`. Unit tests can be run natively on the host machine and on the device.
Two separate environments are configured in `platformio.ini` to support this: `env:native` and `env:seeed_xiao_esp32c3`.

*   **Constraint:** Coding agents may *only* run native tests. Device tests will be executed manually by the user.
*   Example native test commands (use `-v` flag for extra verbosity):
    *   `pio test -e native` (Runs all tests; useful for integration testing)
    *   `pio test -e native -f components/test_{ComponentName}` (Useful when iterating on an individual component)
*   The native environment defines the build flag `-D NATIVE_TESTING`. This is used to mock up Arduino/ESP32-specific functions, which are located in `/test/test_helpers.h`.

# Conventions

We split the application into isolated modules and place them into the special `lib` folder. PlatformIO uses the Library Dependency Finder (LDF) to find these modules and automatically add them to the project build tree based on included header files. Ensure circular dependencies are avoided.

# Output Request

Use your brainstorming skill to generate a comprehensive **Software Architecture and Implementation Specification** document. 

Your output should include:
1.  **Component Responsibilities:** A breakdown of the specific classes, methods, and responsibilities for each library in the scaffold.
2.  **State Machine / Execution Flow:** Step-by-step logic detailing the "Normal Boot Flow" vs. the "Service Mode Flow".
3.  **Error Handling Strategy:** How timeouts, failed sensor reads, and failed network connections will be caught and handled to protect battery life.
4.  **Mocking/Testing Strategy:** A brief plan on how to structure the interfaces (e.g., `ISensor`, `ITransport`) so they can be effectively tested under `-D NATIVE_TESTING`.