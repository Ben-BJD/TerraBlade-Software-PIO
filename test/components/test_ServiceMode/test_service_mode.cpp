#include <unity.h>
#include "../../test_helpers.h" // Pull in our decoupling helper

#include "../../../include/secrets.h" // Pull in test WiFi credentials
#include "../../../include/terrablade_config.h" // Pull in the config to get pin definitions

#include <ServiceMode.h>

ServiceMode* testServiceMode = nullptr;

void setUp(void) 
{
  // Sets up desktop simulation mocks OR does nothing on real hardware seamlessly
  init_test_mocks();
  
  testServiceMode = new ServiceMode(LED_PIN, RESET_BUTTON_PIN, AP_TITLE, AP_SSID, AP_PASSWORD);
}

void tearDown(void) 
{
  delete testServiceMode;
  testServiceMode = nullptr;
}

void test_service_mode_initialization(void) 
{
    TEST_ASSERT_EQUAL(ServiceMode::State::Uninitialized, ServiceMode::getState());
    testServiceMode->init();
    TEST_ASSERT_EQUAL(ServiceMode::State::Configured, ServiceMode::getState());
}

void test_captive_portal_state_transition() 
{
    // Simulate WiFiManager executing the fallback callback
    ServiceMode::configModeCallback(nullptr);

    // Verify the state engine tracked it perfectly!
    TEST_ASSERT_EQUAL(ServiceMode::State::CaptivePortalActive, ServiceMode::getState());
}

void test_on_error_transitions_to_error_state() 
{
    // Trigger error directly
    ServiceMode::onError("Testing simulated failure");

    // Verify state engine transitions to Error
    TEST_ASSERT_EQUAL(ServiceMode::State::Error, ServiceMode::getState());
}

int runUnityTests(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_service_mode_initialization);
    RUN_TEST(test_captive_portal_state_transition);
    RUN_TEST(test_on_error_transitions_to_error_state);
    return UNITY_END();
}

/**
 * Structural Execution Handlers
 */
#if defined(NATIVE_TESTING)
extern "C" {
    int main(int argc, char **argv) 
    {
        return runUnityTests();
    }
}
#else
#include <Arduino.h>
void setup() 
{
  delay(2000); // Give the ESP32 serial monitor time to connect
  runUnityTests();
}

void loop() {}
#endif