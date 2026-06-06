#include <unity.h>

#include "../../test_helpers.h"                 // Pull in our decoupling helper
#include "../../../include/secrets.h"           // Pull in test WiFi credentials
#include "../../../include/terrablade_config.h" // Pull in the config to get pin definitions
#include <Sensor.h>

Sensor* testSensor = nullptr;

void setUp(void) 
{
    init_test_mocks();
    testSensor = new Sensor(PROBE_POWER_PIN, PROBE_SIGNAL_PIN, BATTERY_PIN, DRY_FREQUENCY, WET_FREQUENCY);
}

void tearDown(void) 
{
    delete testSensor;
    testSensor = nullptr;
}

void test_sensor_initialization(void) 
{
    TEST_ASSERT_NOT_NULL(testSensor);
    testSensor->initialise();
    long moisture = testSensor->measureSoilMoisture();
    float voltage = testSensor->measureBatteryVoltage();
    #if defined(NATIVE_TESTING)
        TEST_ASSERT_EQUAL(0, moisture);
        TEST_ASSERT_EQUAL(0, voltage);
    #else
        Serial.println("\nMoisture Reading: " + String(moisture));
        Serial.println("Battery Voltage: " + String(voltage));

        TEST_ASSERT_EQUAL(0, moisture);
        TEST_ASSERT_EQUAL(true, voltage >= 0 && voltage <= 6.0);
    #endif
}

void test_calculate_moisture_percent(void) 
{
    TEST_ASSERT_NOT_NULL(testSensor);
    testSensor->initialise();
    float dry = testSensor->calculateMoisturePercent(DRY_FREQUENCY);
    float wet = testSensor->calculateMoisturePercent(WET_FREQUENCY);

    TEST_ASSERT_EQUAL(0.0, dry);
    TEST_ASSERT_EQUAL(100.0, wet);
}

/**
 * Structural Execution Handlers
 */
#if defined(NATIVE_TESTING)
int main(int argc, char **argv) 
{
    UNITY_BEGIN();
    RUN_TEST(test_sensor_initialization);
    RUN_TEST(test_calculate_moisture_percent);
    return UNITY_END();
}
#else
#include <Arduino.h>
void setup() 
{
    Serial.begin(115200);

    while (!Serial) {
        delay(10);
    }

    delay(3000); // Critical: Give the physical PIO serial monitor time to bind to the USB stack
    Serial.println("\n===========================================");
    Serial.println("   TARGET TESTING ACTIVE: RUNNING SUITE");
    Serial.println("===========================================");
    Serial.flush();
    // 3. Run your tests cleanly
    UNITY_BEGIN();
    RUN_TEST(test_sensor_initialization);
    RUN_TEST(test_calculate_moisture_percent);
    UNITY_END();
    Serial.println("\n[SYSTEM] Testing complete. Holding stable loop...");
}

void loop() {}
#endif