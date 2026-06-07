---
Date: 2026-06-07
Topic: Project Readme
Tool: OpenCode (Qwen3.6-Thinking)
---

# Session Context: Generate Repository README.md

**Role:** You are a Senior Developer Advocate and Open-Source Maintainer.
**Task:** I need you to write the official `README.md` for the root of this repository. This repository is the software component of the "TerraBlade" (Modular Soil Scout) project, which is featured in "Part 2" of my YouTube series on building a professional-grade, low-power IoT soil moisture sensor.

**CRITICAL RULE:** Do NOT modify any existing code, tests, or config files. Your only job is to read the repository and output the Markdown text for the `README.md` in the chat.

**Step 1: Read the Context**
Please use your file-reading tools to scan the following files to understand the project:
1. `platformio.ini` (To see the environments, board, and dependencies).
2. `docs/superpowers/specs/2026-06-05-soil-moisture-architecture-design.md` (To understand the core architecture and features).
3. `gen-ai/README.md` (To understand how AI was used to build this).
4. Briefly scan the headers in the `lib/` folder (`Sensor`, `ServiceMode`, `TelemetryTransport`, `Schedule`, `Application`) to see the Object-Oriented structure.

**Step 2: Generate the README.md**
Based on what you read, write a highly professional, engaging, and well-structured README.md. Please include the following sections:

1. **Title & Introduction:** Catchy title, brief description of the TerraBlade project, and mention that this is the Software repo (accompanying the Hardware PCB repo and YouTube series).
2. **Key Software Features:** Highlight the ultra-low-power focus (zero-leakage parasitic power on GPIO 10), the "Double-Tap" battery pull Service Mode (Captive Portal via WiFiManager), the hardware frequency-to-moisture calibration, and the fail-safe deep sleep architecture.
3. **Hardware Pinout:** A quick table or list of the Seeed XIAO ESP32-C3 GPIO mappings used in this project.
4. **Project Structure:** Briefly explain the PlatformIO `lib/` component architecture (Dependency Injection) and why it's structured this way.
5. **Building & Testing:** Instructions on how to run the native unit tests (`pio test -e native` using ArduinoFake) vs. building for the actual hardware.
6. **AI Development Methodology:** A short section mentioning that this codebase was co-programmed with AI using a "Driver & Navigator" workflow, linking to the `gen-ai/` folder for prompt logs.

Explain that the /include/secrets.h file is not included in the repo for privacy. Explain that secrets.h will need to be created. Explain what paramaters it will need.

Please output the complete Markdown content inside a code block so I can copy and paste it into my root `README.md`.