#ifndef ServiceMode_h
#define ServiceMode_h

#include <Arduino.h>

// Note: If you use Approach 1 for native tests, 
// your platformio.ini will automatically find your native WiFi.h / WiFiManager.h stubs
#include <WiFi.h>
#include <WiFiManager.h>

class ServiceMode {
  public:
    enum class State {
        Uninitialized,
        Configured,         // Wi-Fi credentials found, connected successfully
        Configuring,        // Attempting connection or initializing routes
        CaptivePortalActive, // In configuration access point mode
        Error               // onError was triggered
    };

    ServiceMode(int ledPin, int btnPin, const char* apTitle, const char* apSSID, const char* apPassword);
    void init();
    void onLoop();
    
    // Both of these must be static to work with raw function pointer callbacks
    static void IRAM_ATTR onFactoryReset();
    static void configModeCallback(WiFiManager *wiFiManager);
    static void onError(const std::string& errorMsg);
    static void clearConfig();

    static State getState() { return _currentState; }

  private:
    static int _ledPin;
    static int _btnPin;
    static volatile bool _factoryResetRequested;
    static const char* _apTitle;
    static const char* _apSSID;
    static const char* _apPassword;

    static State _currentState;
};

#endif