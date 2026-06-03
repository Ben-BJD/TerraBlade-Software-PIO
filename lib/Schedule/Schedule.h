#ifndef Schedule_h
#define Schedule_h

#include <Arduino.h>

class Schedule 
{
  public:
    enum class BootSource {
        Uninitialized
        ,PowerOnReset
        ,DeepSleepWakeGPIO
        ,DeepSleepWakeTimer
    };

    Schedule(uint64_t sleepTimeSeconds, uint8_t buttonPin);
    int getBootCount() const;
    BootSource getBootSource();

    void initialize();
    
    // The critical architectural hook
    void enterDeepSleep();

  private:
    uint64_t _sleepTimeSeconds;
    uint8_t _buttonPin;
    static const uint64_t uS_TO_S_FACTOR = 1000000ULL;
    BootSource _bootSource = BootSource::Uninitialized;

    void incrementBootCount();
    void configureSleepWakeups();
    void processWakeupReason();
};

#endif