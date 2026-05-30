#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer 
{
  public:
    Timer(unsigned long interval);
    void start();
    void stop();
    bool isRunning();
    bool update();
  private:
    unsigned long _interval;
    unsigned long _previousMillis;
    bool _running;
};

#endif

