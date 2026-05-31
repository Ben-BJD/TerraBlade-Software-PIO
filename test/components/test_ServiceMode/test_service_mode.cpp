#include <unity.h>
#include "../../test_helpers.h"                 // Pull in our decoupling helper
#include "../../../include/secrets.h"           // Pull in test WiFi credentials
#include "../../../include/terrablade_config.h" // Pull in the config to get pin definitions
#include <ServiceMode.h>

ServiceMode* testServiceMode = nullptr;

// Hardware-only helper to block execution and wait for human interaction
#if !defined(NATIVE_TESTING)
void waitForHardwareState(ServiceMode::State targetState, const char* message) {
    Serial.print("\n[WAITING] ");
    Serial.println(message);
    
    // Loop indefinitely until the state engine matches our target
    while (ServiceMode::getState() != targetState) {
        // Rapidly blink the indicator LED to show the test runner is waiting on YOU
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }
    Serial.println("[SUCCESS] Target state reached. Resuming test runner...");
}
#endif

void setUp(void) 
{
    init_test_mocks();
    testServiceMode = new ServiceMode(LED_PIN, RESET_BUTTON_PIN, AP_TITLE, AP_SSID, AP_PASSWORD);
    ServiceMode::clearConfig(); // Ensure we start with a clean slate for each test
}

void tearDown(void) 
{
    delete testServiceMode;
    testServiceMode = nullptr;
}

void test_service_mode_initialization(void) 
{
    TEST_ASSERT_EQUAL(ServiceMode::State::Uninitialized, ServiceMode::getState());
    
    #if defined(NATIVE_TESTING)
        testServiceMode->init();
        TEST_ASSERT_EQUAL(ServiceMode::State::Configured, ServiceMode::getState());
    #else
        Serial.println("\n==================================================");
        Serial.println("HARDWARE STEP REQUIRED:");
        Serial.print("1. Connect to Wi-Fi Access Point: "); Serial.println(AP_SSID);
        Serial.print("2. Enter the test router credentials: "); Serial.println(AP_PASSWORD);
        Serial.println("==================================================");

        // Run the initialization sequence directly.
        // If you need to clear settings for a clean test, do it inside ServiceMode itself
        // or ensure it's handled without duplicating local stack objects.
        testServiceMode->init(); 

        TEST_ASSERT_EQUAL(ServiceMode::State::Configured, ServiceMode::getState());
    #endif
}

void test_captive_portal_state_transition() 
{
    // On the device, the state might already be Configured from the previous test.
    // For a pure behavioral state check, we reset or manually trigger the transition hook.
    ServiceMode::configModeCallback(nullptr);
    TEST_ASSERT_EQUAL(ServiceMode::State::CaptivePortalActive, ServiceMode::getState());
}

void test_on_error_transitions_to_error_state() 
{
    // Pre-emptively disable reboot on the native side if needed, 
    // or intercept ESP.restart to make sure the device runner doesn't drop the serial line.
    #if defined(NATIVE_TESTING)
        ServiceMode::onError("Testing simulated failure");
        TEST_ASSERT_EQUAL(ServiceMode::State::Error, ServiceMode::getState());
    #else
        // We skip or handle this carefully on the device, because ESP.restart() 
        // will drop the USB Serial connection and kill the Unity test runner session!
        Serial.println("[SKIPPED] Skipping hard reboot test on physical hardware to prevent test runner disconnect.");
    #endif
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
    Serial.begin(115200);
    delay(3000); // Critical: Give the physical PIO serial monitor time to bind to the USB stack
    runUnityTests();
}

void loop() {}
#endif