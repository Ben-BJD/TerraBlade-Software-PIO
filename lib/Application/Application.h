#ifndef Application_h
#define Application_h

class Application {
  public:
    enum class State {
            Uninitialized,
            ServiceMode,
            ServiceModeError,
            WifiConnecting,
            WifiConnectingError, 
            TelemetryTransmitting,
            TelemetryTransmittingError,
            Finished
        };
    Application();
    
    void init(bool debug = false);

    static State getState() { return _currentState; }

  private:
    static bool isValidMoistureReading(long moistureValue);
    static State _currentState;
};

#endif