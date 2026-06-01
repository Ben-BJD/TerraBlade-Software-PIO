#include <unity.h>
#include <Timer.h>
#include "../../test_helpers.h"

Timer* testTimer = nullptr;

void setUp(void) 
{
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

/**
 * Structural Execution Handlers
 */
#if defined(NATIVE_TESTING)
int main(int argc, char **argv) 
{
    UNITY_BEGIN();
    RUN_TEST(test_timer_ticks);
    return UNITY_END();
}
#else
#include <Arduino.h>
void setup() 
{
    // 1. Initialize Native USB CDC
    Serial.begin(115200);
    
    // 2. CRITICAL TRAP: Wait for the Host PC's Serial Monitor to establish connection.
    // On the native USB stack, this blocks execution until PlatformIO opens the port.
    while (!Serial) {
        delay(10);
    }
    
    // Give the Windows buffer a moment to clear its eyes
    delay(3000); 
    
    Serial.println("\n===========================================");
    Serial.println("    TARGET TESTING ACTIVE: RUNNING SUITE");
    Serial.println("===========================================");
    Serial.flush();
    // 3. Run your tests cleanly
    UNITY_BEGIN();
    RUN_TEST(test_timer_ticks);
    
    // 4. THE CLEAN TERMINATION TRICK
    // We explicitly invoke Unity's output string generator but manually prevent
    // it from falling through to the toolchain's crashing exit(0) handler.
    UNITY_END(); 
    
    Serial.println("\n[SYSTEM] Tests completed cleanly. Entering idle holding state.");
}

void loop() 
{
    
}
#endif