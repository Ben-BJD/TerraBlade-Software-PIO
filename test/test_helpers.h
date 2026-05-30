#pragma once

// This header is included by all test files, and serves as a central place to manage test dependencies and common test setup.
// It conditionally includes the ArduinoFake library and sets up global mocks when running in a native desktop environment, while doing nothing when running on real hardware. This allows test code to be written once and run seamlessly in both environments.

// If we are on native desktop, we need the simulation mocks
#if defined(NATIVE_TESTING)
    #include <ArduinoFake.h>
    using namespace fakeit;

    // Undefine them if the environment brought them in as macros
    #undef interrupts
    #undef noInterrupts

    // Define them as simple, blank global functions so your code links perfectly
    inline void interrupts() {}
    inline void noInterrupts() {}

    #define D0 0
    #define D1 1
    #define D2 2
    #define D3 3
    #define D4 4
    #define D5 5
    #define D6 6
    #define D7 7
    #define D8 8
    #define D9 9
    #define D10 10

    inline void init_test_mocks() 
    {
        ArduinoFakeReset();
        // Global baseline mocks for desktop execution

        // ==========================================
        // 1. TIMING & DELAYS
        // ==========================================
        When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
        When(Method(ArduinoFake(), micros)).AlwaysReturn(0);
        When(Method(ArduinoFake(), delay)).AlwaysReturn();
        When(Method(ArduinoFake(), delayMicroseconds)).AlwaysReturn();

        // ==========================================
        // 2. DIGITAL & ANALOG I/O
        // ==========================================
        When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
        When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
        When(Method(ArduinoFake(), digitalRead)).AlwaysReturn(0); // Low by default
        When(Method(ArduinoFake(), analogRead)).AlwaysReturn(0);
        When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

        // ==========================================
        // 3. COMMON HARDWARE INTERFACES (Peripherals)
        // ==========================================
        // Serial Communication
        When(OverloadedMethod(ArduinoFake(Serial), begin, void(unsigned long)).Using(fakeit::_)).AlwaysReturn();
        When(OverloadedMethod(ArduinoFake(Serial), write, size_t(uint8_t))).AlwaysReturn(1);
        When(Method(ArduinoFake(Serial), available)).AlwaysReturn(0);
        When(Method(ArduinoFake(Serial), read)).AlwaysReturn(-1);
        // Note: We use OverloadedMethod for print/println to avoid template errors
        When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char*))).AlwaysReturn(0);
        When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char*))).AlwaysReturn(0);
        When(OverloadedMethod(ArduinoFake(Serial), println, size_t(void))).AlwaysReturn(0);

        // I2C / Wire 
        When(OverloadedMethod(ArduinoFake(Wire), begin, void())).AlwaysReturn();
        When(OverloadedMethod(ArduinoFake(Wire), beginTransmission, void(uint8_t))).AlwaysReturn();

        // 'write' has multiple variants (e.g., single byte vs buffer)
        When(OverloadedMethod(ArduinoFake(Wire), write, size_t(uint8_t))).AlwaysReturn(1);
        When(OverloadedMethod(ArduinoFake(Wire), write, size_t(const uint8_t*, size_t))).AlwaysReturn(1);

        // 'endTransmission' has a variant that takes a 'bool' or takes nothing
        When(OverloadedMethod(ArduinoFake(Wire), endTransmission, uint8_t(void))).AlwaysReturn(0);
        When(OverloadedMethod(ArduinoFake(Wire), endTransmission, uint8_t(bool))).AlwaysReturn(0);

        // 'requestFrom' has 2-argument and 3-argument variants commonly used
        When(OverloadedMethod(ArduinoFake(Wire), requestFrom, uint8_t(uint8_t, uint8_t))).AlwaysReturn(0);
        When(OverloadedMethod(ArduinoFake(Wire), requestFrom, uint8_t(uint8_t, uint8_t, uint8_t))).AlwaysReturn(0);

        // SPI
        When(Method(ArduinoFake(SPI), begin)).AlwaysReturn();
        When(OverloadedMethod(ArduinoFake(SPI), transfer, uint8_t(uint8_t)).Using(fakeit::_)).AlwaysReturn(0);
        When(Method(ArduinoFake(SPI), beginTransaction)).AlwaysReturn();
        When(Method(ArduinoFake(SPI), endTransaction)).AlwaysReturn();

        // ==========================================
        // 4. INTERRUPTS
        // ==========================================
        When(Method(ArduinoFake(), attachInterrupt)).AlwaysReturn();
        When(Method(ArduinoFake(), detachInterrupt)).AlwaysReturn();
    }
#else
    // If we are on the real ESP32 hardware, mocks do absolutely nothing
    inline void init_test_mocks() {
        // No-op: Real hardware uses real registers/drivers
    }
#endif