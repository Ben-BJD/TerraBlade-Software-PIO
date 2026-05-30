#include <unity.h>
#include <Timer.h>
#include "../../test_helpers.h" // Pull in our decoupling helper

Timer* testTimer = nullptr;

void setUp(void) 
{
  // Sets up desktop simulation mocks OR does nothing on real hardware seamlessly
  init_test_mocks();
  
  testTimer = new Timer(1000); 
  testTimer->start();
}

void tearDown(void) 
{
  testTimer->stop();
  delete testTimer;
  testTimer = nullptr;
}

void test_timer_ticks(void) 
{
  TEST_ASSERT_EQUAL(true, testTimer->isRunning());
}

int runUnityTests(void) 
{
  UNITY_BEGIN();
  RUN_TEST(test_timer_ticks);
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