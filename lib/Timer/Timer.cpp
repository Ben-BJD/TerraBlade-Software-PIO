#include "Arduino.h"
#include "Timer.h"

Timer::Timer(unsigned long interval) 
  : _interval(interval), _previousMillis(0), _running(false) 
{

}

void Timer::start() 
{
  _previousMillis = millis();
  _running = true;
}

void Timer::stop() 
{
  _running = false;
}

bool Timer::isRunning() 
{
  return _running;
}

bool Timer::update() 
{
  if (_running) 
  {
    unsigned long currentMillis = millis();
    if (currentMillis - _previousMillis >= _interval) 
    {
      _previousMillis = currentMillis;
      return true;
    }
  }

    return false;
}

