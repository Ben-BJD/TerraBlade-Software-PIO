#include <unity.h>

// 1. Force the primary framework classes to load completely first
#include <Arduino.h>
#include <vector>
#include <string>
#include <ThingSpeak.h>

#include "../../test_helpers.h"                 // Pull in our decoupling helper
#include <Application.h>                            // Pull in the header of the component we want to test

#if defined(NATIVE_TESTING)
ThingSpeakClass ThingSpeak;
#endif

Application* testApplication = nullptr;

void setUp(void) 
{
    init_test_mocks();
    testApplication = new Application();
}

void tearDown(void) 
{
    delete testApplication;
    testApplication = nullptr;
}

void test_app_run_app_run(void) 
{
    TEST_ASSERT_EQUAL(Application::State::Uninitialized, Application::getState());
    testApplication->init(true); // Pass 'true' to enable debug mode and prevent actual deep sleep during testing
    TEST_ASSERT_EQUAL(Application::State::Finished, Application::getState());
}

/**
 * Structural Execution Handlers
 */
#if defined(NATIVE_TESTING)
int main(int argc, char **argv) 
{
    UNITY_BEGIN();
    RUN_TEST(test_app_run_app_run);
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
    RUN_TEST(test_app_run_app_run);
    UNITY_END();
    Serial.println("\n[SYSTEM] Testing complete. Holding stable loop...");
}

void loop() {}
#endif