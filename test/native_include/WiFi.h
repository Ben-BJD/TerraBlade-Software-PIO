// test/native_include/WiFi.h
#ifndef FAKE_WIFI_H
#define FAKE_WIFI_H

// Fix for desktop compilers handling ESP32 hardware attributes
#ifndef IRAM_ATTR
    #define IRAM_ATTR
#endif

#include <stdint.h>

// Create a mock class for the ESP system object
class MockEspClass {
private:
    bool _restartRequested = false;

public:
    void restart() {
        _restartRequested = true;
    }

    // Test-only helper methods to verify behavior
    bool isRestartRequested() { return _restartRequested; }
    void reset() { _restartRequested = false; }
};

// 2. Expose it as a global variable named ESP
extern MockEspClass ESP;

// Core Arduino network constants
typedef enum {
    WL_NO_SHIELD        = 255,
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
} wl_status_t;

// A lightweight stub of the global WiFi class
class WiFiClass {
private:
    wl_status_t _mockStatus = WL_CONNECTED;

public:
    // Methods to allow your production code to compile
    void begin(const char* ssid, const char* password = nullptr) {
        // Automatically simulate a successful connection for tests
        _mockStatus = WL_CONNECTED; 
    }

    wl_status_t status() { 
        return _mockStatus; 
    }

    void disconnect(bool wifioff = false) {
        _mockStatus = WL_DISCONNECTED;
    }

    // Add a helper method *only* visible during testing to manipulate the state
    void setMockStatus(wl_status_t status) {
        _mockStatus = status;
    }
};

// Expose the global instance just like the real WiFi library does
extern WiFiClass WiFi;

#endif