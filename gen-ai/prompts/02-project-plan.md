---
Date: 2026-06-05
Topic: Initial Project Planning
Tool: OpenCode (Qwen3.6 + Obra)
---

The brainstorming phase is complete and the specifications are finalized in our spec file @/docs/superpowers/specs/2026-06-05-soil-moisture-architecture-design.md.

Let's create an implementation plan for the spec.

Read the existing file, then invoke the Superpowers planning skill to generate our step-by-step TDD implementation plan.

Before we proceed to coding, I need to inform you that I have already built the initial class files (.cpp and .h) and unit tests for each of the components in the lib/ and test/ directories.

CRITICAL RULE: Do not generate any components from scratch. Do not overwrite files blindly. From now on, your job is to read my existing files, compare them to our updated Architecture Spec, and only make surgical modifications to align my existing code with the new requirements.

---

# Session Context: Global Codebase Audit & Master Refactoring Plan

**Role:** You are a Staff-Level Embedded C++ Architect. 
**Objective:** We are transitioning from a prototype codebase to a production-grade Object-Oriented architecture. I want to perform a single-pass global audit of the entire existing codebase to see exactly where we stand.

**CRITICAL RULES FOR THIS SESSION:**
1. I have already scaffolded the project, written initial classes, and created unit tests across multiple directories. 
2. **DO NOT write, modify, or overwrite any `.cpp` or `.h` files yet.** 
3. Your job in this prompt is purely analytical. I want a comprehensive master plan first.

**Task Instructions:**
1. Read the system instructions in `AGENTS.md`.
2. Read the final architecture requirements in `docs/superpowers/specs/2026-06-05-soil-moisture-architecture-design.md`.
3. Read all my existing codebase files located in `lib/Application`, `lib/Sensor`, `lib/Schedule`, `lib/ServiceMode`, and `lib/TelemetryTransport`.
4. Read all my existing native unit tests in `test/components/`.

**Output Required (The Global Gap Analysis):**
Please provide a comprehensive, component-by-component Gap Analysis. For each component, explicitly list:
*   **What is correct/reusable** in my existing code.
*   **What is missing or violates the new spec** (e.g., missing interfaces, blocking while-loops, missing the 10s WiFi timeout, missing deep sleep fallbacks, missing battery voltage, missing the 0-100% moisture math, etc.).
*   **Unit Test Deficiencies:** What native tests need to be written or updated to cover the new constraints.

**Execution Roadmap:**
After the analysis, propose a numbered **Execution Roadmap** (e.g., Step 1: Update Interfaces, Step 2: Refactor Sensor, Step 3: Refactor Transport, etc.). 

Stop and wait for my approval after providing this document. Once I review and approve the roadmap, I will instruct you to execute it step-by-step.