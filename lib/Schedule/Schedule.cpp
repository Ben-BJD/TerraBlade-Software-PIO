#include "Schedule.h"

#if defined(NATIVE_TESTING)
    // Desktop simulation variables to test your tracking logic
    static int mock_wakeup_reason = 0; 
    void set_mock_wakeup_reason(int reason) { mock_wakeup_reason = reason; }
    bool has_attempted_sleep = false; // Flag to assert against in tests

    #ifndef RTC_DATA_ATTR
        #define RTC_DATA_ATTR
    #endif

#endif

// Keep your RTC variable isolated here or in main
RTC_DATA_ATTR static int global_bootCount = 0;

Schedule::Schedule(uint64_t sleepTimeSeconds, uint8_t buttonPin) : _sleepTimeSeconds(sleepTimeSeconds), _buttonPin(buttonPin) {}

void Schedule::incrementBootCount()
{
    global_bootCount++;
}

int Schedule::getBootCount() const 
{
    return global_bootCount;
}

void Schedule::configureSleepWakeups() 
{
    #if !defined(NATIVE_TESTING)
        // Real ESP32-C3 hardware register mappings
        esp_deep_sleep_enable_gpio_wakeup(BIT(_buttonPin), ESP_GPIO_WAKEUP_GPIO_LOW);
        esp_sleep_enable_timer_wakeup(_sleepTimeSeconds * uS_TO_S_FACTOR);
    #endif
}

void Schedule::processWakeupReason() 
{
#if defined(NATIVE_TESTING)
    // Desktop Simulation paths
    _bootSource = BootSource::PowerOnReset; // Default to PowerOnReset for testing
#else
    // Real Hardware Paths
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason == ESP_SLEEP_WAKEUP_GPIO) 
    {
        _bootSource = BootSource::DeepSleepWakeGPIO;
    } else if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) 
    {
        _bootSource = BootSource::DeepSleepWakeTimer;
    } else 
    {
        _bootSource = BootSource::PowerOnReset;
    }
#endif
}

Schedule::BootSource Schedule::getBootSource() 
{
    return _bootSource;
}

void Schedule::initialize() 
{
    incrementBootCount();
    configureSleepWakeups();
    processWakeupReason();
}

void Schedule::enterDeepSleep() 
{

#if defined(NATIVE_TESTING)
    // 1. Desktop simulation tracking
    has_attempted_sleep = true; 

#elif defined(UNIT_TEST)
   has_attempted_sleep = true; 

#else
    // 3. PRODUCTION HARDWARE RUNTIME
    // This only executes during standard deployments ('pio run' / upload)
    esp_deep_sleep_start();
#endif
}