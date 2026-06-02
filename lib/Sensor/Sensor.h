#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>

// ========================================================================
// DESKTOP TESTING COMPATIBILITY PATCH
// ========================================================================
#if defined(NATIVE_TESTING)
    // 1. Stub the hardware attribute out of existence
    #ifndef IRAM_ATTR
        #define IRAM_ATTR
    #endif

    // 2. Provide a safe mutable variable scoped ONLY to this header instance
    static uint32_t local_mock_mv = 99;

    // 3. ADDED 'inline' - Prevents linker multiple-definition collisions!
    inline uint32_t analogReadMilliVolts(uint8_t pin) {
        return local_mock_mv;
    }

    // 4. ADDED 'inline' - Prevents linker multiple-definition collisions!
    inline void set_mock_analog_milli_volts(uint32_t mv) {
        local_mock_mv = mv;
    }
#endif
// ========================================================================

class Sensor {
  public:
  
    Sensor(int probePowerPin, int probeSignalPin, int batteryPin);
    void initialise();
    long measureSoilMoisture();
    float measureBatteryVoltage();
    
    static void IRAM_ATTR countPulse();

  private:
    static int _probePowerPin;
    static int _probeSignalPin;
    static int _batteryPin;
    static volatile uint32_t _pulse_count;
};

#endif