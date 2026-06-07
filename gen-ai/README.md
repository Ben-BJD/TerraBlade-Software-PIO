# Generative AI Development Logs

This directory contains the prompts, architectures, and session transcripts used to generate the C++ codebase for the TerraBlade (Modular Soil Scout) project. 

Because embedded C++ development (specifically hardware mocking, memory management, and power states) is highly complex, this folder documents the exact guardrails and strategies we used to successfully pair-program with Large Language Models (LLMs).

## 📂 Directory Structure

*   **/prompts/**: Contains the isolated `.md` prompt files used to instruct the AI. These include the core architecture specs, component-by-component gap analyses, and strict rules for power management.
*   **/sessions/**: Contains raw markdown snapshots of the actual conversations and execution loops with the AI (using OpenCode). 

## 🧠 Models & Tooling
*   **Tool**: [OpenCode](https://github.com/opencode)
*   **Architecture Phase Model**: `Qwen3-235B-A22B-Thinking-2507` (Used for initial brainstorming and architecture specs).
*   **Implementation Model**: `Qwen3-Coder-480B-A35B-Instruct` (Used for surgical C++ component implementation).
*   **Provider**: Hugging Face

## 🛑 The "Driver & Navigator" Methodology

Initially, we attempted to use a fully autonomous agentic framework (Obra Superpowers) to write, compile, and test the code. However, due to the nuances of the `ArduinoFake` mocking framework and PlatformIO, the autonomous agent fell into an "Agentic Death Spiral"—burning API tokens by rapidly hallucinating fixes for compiler errors.

To solve this, we pivoted to a **Driver & Navigator** workflow, which yielded exceptional code quality at a fraction of the API cost (e.g., $0.14 for a complete Sensor component refactor):

1.  **AI as Navigator:** The AI is strictly forbidden from running terminal commands, modifying `platformio.ini`, or running unit tests.
2.  **Human as Driver:** We feed the AI the Architecture Spec and the existing component files. The AI writes the updated C++ code and halts.
3.  **Manual Compilation:** We manually paste the code and run `pio test -e native`. If there is a compiler error, we paste the exact terminal output back to the AI for a targeted fix.

## 🔑 Core Hardware Constraints Enforced by AI
Throughout these sessions, the AI was strictly prompted to enforce:
*   **Zero-Leakage Parasitic Power:** ESP32-C3 GPIO states must be explicitly managed (HIGH to read, LOW instantly after).
*   **Fail-Safe Timeouts:** No infinite `while()` loops. All network and sensor reads have strict timeouts (e.g., 10s WiFi timeout).
*   **Always Sleep:** Any caught error aborts the telemetry loop and forces the device into Deep Sleep.
*   **Flash-Safe State Management:** Using ESP32 `Preferences.h` (NVS) instead of EEPROM to track "Double-Tap" battery pulls for Service Mode entry.

---
*Note: If you are forking this repo and using these prompts in your own AI tools, ensure you read `AGENTS.md` in the root directory for the active system instructions.*