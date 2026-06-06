---
Date: 2026-06-05
Topic: Application Facade Component
Tool: OpenCode (Qwen3.6-Coder)
---

Read AGENTS.md and the architecture spec in docs/superpowers/specs/2026-06-05-soil-moisture-architecture-design.md.

Today we are only working on the Application Library.

Please read my existing files in lib/Application/Application.cpp, lib/Application/Application.h, and test/components/test_Application/.

I need you to update my Application code to fulfill any gaps in the architecture spec. Specifically:
- WiFi 10s timeout
- Handle errors centrally, retry logic, trigger deep sleep on failure
    - All timeouts/retries managed by Application facade
    - If the sensor fails or returns out-of-bounds frequencies, do NOT send a default reading. Abort the ThingSpeak transmission entirely and go straight to deep sleep. We do not want to push junk data to the cloud

You are responsible for implementing all of the TODO comment blocks in Application.cpp

Important: Just provide the updated C++ code for Application.cpp and Application.h. I will run the native tests myself and let you know if there are any compiler errors