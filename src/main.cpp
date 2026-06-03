#include <Arduino.h>
#include <Application.h> 

void setup() 
{
  Application* testApplication = new Application();
  testApplication->init(false); // Pass 'false' to disable debug mode and allow deep sleep in production
}

void loop() 
{
  //do nothing - all logic is handled in Application::init() and we will enter deep sleep from there when finished
}