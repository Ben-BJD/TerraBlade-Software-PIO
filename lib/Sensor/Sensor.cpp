#include "Sensor.h"
#include <Arduino.h>

// Initialize all static variables
volatile uint32_t Sensor::_pulse_count = 0; 
int Sensor::_probePowerPin = -1;
int Sensor::_probeSignalPin = -1;
int Sensor::_batteryPin = -1;

Sensor::Sensor(int probePowerPin, int probeSignalPin, int batteryPin) 
{
    _probePowerPin = probePowerPin;
    _probeSignalPin = probeSignalPin;
    _batteryPin = batteryPin;
}

void IRAM_ATTR Sensor::countPulse() 
{
    _pulse_count++;
}

void Sensor::initialise() 
{
    // 1. Configure the Parasitic Power Pin
    pinMode(_probePowerPin, OUTPUT);
    digitalWrite(_probePowerPin, LOW); // Ensure sensor is off

    // 2. Configure the Signal Pin
    pinMode(_probeSignalPin, INPUT);

    //We will read the value of the analog pin A0 and send it to thingspeak, this will read battery voltage
    pinMode(_batteryPin, INPUT);// initialize analog pin A0 as an input 
}

long Sensor::measureSoilMoisture()
{
    const int NUM_SAMPLES = 5;
    uint32_t total_frequency = 0;

    // 1. Power up the sensor ONCE for the whole batch
    digitalWrite(_probePowerPin, HIGH);

    // Wait 10ms to ensure the 555 timer and copper traces are fully saturated
    delay(10); 

    // ---------------------------------------------------
    // 2. THE THROWAWAY READ
    // We do one quick read to clear out any startup noise
    // ---------------------------------------------------
    _pulse_count = 0;
    attachInterrupt(digitalPinToInterrupt(_probeSignalPin), Sensor::countPulse, RISING);
    delayMicroseconds(20000); 
    detachInterrupt(digitalPinToInterrupt(_probeSignalPin));
    // (We don't save this value)

    // ---------------------------------------------------
    // 3. THE BATCH READ
    // ---------------------------------------------------
    for(int i = 0; i < NUM_SAMPLES; i++) 
    {
        _pulse_count = 0; // Reset counter

        attachInterrupt(digitalPinToInterrupt(_probeSignalPin), Sensor::countPulse, RISING);
        delayMicroseconds(20000); // Exactly 20ms
        detachInterrupt(digitalPinToInterrupt(_probeSignalPin));

        long current_freq = _pulse_count * 50;
        total_frequency += current_freq;
    }

    // 4. Kill the power immediately to save battery
    digitalWrite(_probePowerPin, LOW); 

    // 5. Calculate Average
    long average_freq = total_frequency / NUM_SAMPLES;

    return average_freq;
}

float Sensor::measureBatteryVoltage()
{
    uint32_t Vbatt = 0;
    for(int i = 0; i < 16; i++) 
    {
        Vbatt = Vbatt + analogReadMilliVolts(_batteryPin); // ADC with correction   
    }
    float Vbattf = 2 * Vbatt / 16 / 1000.0;     // attenuation ratio 1/2, mV --> V
    return Vbattf;
}