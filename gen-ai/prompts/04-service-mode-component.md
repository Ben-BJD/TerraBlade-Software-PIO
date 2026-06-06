---
Date: 2026-06-05
Topic: Service Mode Component
Tool: OpenCode (Qwen3.6-Coder)
---

Read AGENTS.md and the architecture spec in docs/superpowers/specs/2026-06-05-soil-moisture-architecture-design.md.

Today we are only working on the Service Mode Library.

Please read my existing files in lib/ServiceMode/ServiceMode.cpp, lib/ServiceMode/ServiceMode.h, and test/components/test_ServiceMode/.

I need you to update my ServiceMode code to fulfill any gaps in the architecture spec. Specifically:
- Trigger from a "Double-Tap" feature on power cycling

On Power-On Reset, we want to check if the user is trying to trigger a reset into Service Mode by 
power-cycling the device in a specific pattern.
We don't want to accidentally wipe the user's Wi-Fi credentials every time they power-cycle the device, 
so we need a way to differentiate between normal power cycles and intentional ones meant to trigger a reset into Service Mode.

We need a new function that will trigger ServiceMode::clearConfig(). That will include a "Double-Tap" feature on power cycling to allow users to force entry into configuration mode if they need to change Wi-Fi credentials or other settings. 
This is critical for usability in the field where users may need to update their network connection without re-flashing the device.
We will use power-cycling to enter config mode. This is a common pattern for headless IoT devices that lack physical buttons or 
interfaces for configuration. How it works: 
- Device boots up. It reads a value from the EEPROM/Flash memory.
- It increments a "Boot Count" to 1.
- It waits for 3 seconds.
- If it survives the 3 seconds, it resets the count to 0.

The User Experience: To reset the WiFi, you pull the battery out. 
You push the magazine in (Device boots), pull it out 1 second later, and push it in again. The ESP32 sees "Boot Count = 2", 
realizes it was intentionally power-cycled, 
and resets the wifi manager settings so we can reconfigure ServiceMode::clearConfig()

Important: Just provide the updated C++ code for ServiceMode.cpp and ServiceMode.h. I will run the native tests myself and let you know if there are any compiler errors