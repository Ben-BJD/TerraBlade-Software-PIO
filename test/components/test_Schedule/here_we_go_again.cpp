#include <unity.h>

#include "../../test_helpers.h"                 // Pull in our decoupling helper
#include "../../../include/secrets.h"           // Pull in test WiFi credentials
#include "../../../include/terrablade_config.h" // Pull in the config to get pin definitions
#include <Schedule.h>

Schedule* testSchedule = nullptr;

void setUp(void) 
{
    init_test_mocks();
    testSchedule = new Schedule(60, 3); // 60 seconds sleep time, GPIO 3 as wakeup source
}

void tearDown(void) 
{
    delete testSchedule;
    testSchedule = nullptr;
}

void test_boot_counter_increments_correctly(void) 
{
    int initialCount = testSchedule->getBootCount();
    testSchedule->initialize();
    
    TEST_ASSERT_EQUAL(initialCount + 1, testSchedule->getBootCount());
}

void test_get_boot_source(void) 
{
    TEST_ASSERT_EQUAL(Schedule::BootSource::Uninitialized, testSchedule->getBootSource());
    testSchedule->initialize();
    TEST_ASSERT_EQUAL(Schedule::BootSource::PowerOnReset, testSchedule->getBootSource());
}

/**
 * Structural Execution Handlers
 */
#if defined(NATIVE_TESTING)
int main(int argc, char **argv) 
{
    UNITY_BEGIN();
    RUN_TEST(test_boot_counter_increments_correctly);
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
    RUN_TEST(test_boot_counter_increments_correctly);
    RUN_TEST(test_get_boot_source);
    UNITY_END();
    Serial.println("\n[SYSTEM] Testing complete. Holding stable loop...");
}

void loop() {}
#endif