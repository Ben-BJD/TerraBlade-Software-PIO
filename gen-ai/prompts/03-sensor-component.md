---
Date: 2026-06-05
Topic: Sensor Component
Tool: OpenCode (Qwen3.6-Coder)
---

Read AGENTS.md and the architecture spec in docs/superpowers/specs/2026-06-05-soil-moisture-architecture-design.md.

Today we are only working on the Sensor Library.

Please read my existing files in lib/Sensor/Sensor.cpp, lib/Sensor/Sensor.h, and test/components/test_Sensor/.

I need you to update my Sensor code to fulfill any gaps in the architecture spec. Specifically:
- Ensure PROBE_POWER_PIN (GPIO 10) is turned HIGH before the reads, and immediately LOW after.
- Implement the calculateMoisturePercent() private method using constrained linear interpolation (106000Hz dry, 13000Hz wet).

Important: Just provide the updated C++ code for Sensor.cpp and Sensor.h. I will run the native tests myself and let you know if there are any compiler errors