// test/native_include/WiFiManager.h
#ifndef FAKE_WIFIMANAGER_H
#define FAKE_WIFIMANAGER_H

#include <vector>
#include <string>
#include <functional>
#include "WiFi.h"

// 1. Define the HTTP Upload State Constants expected by your firmware
enum HTTPUploadStatus {
    UPLOAD_FILE_START = 1,
    UPLOAD_FILE_WRITE = 2,
    UPLOAD_FILE_END   = 3,
    UPLOAD_FILE_ABORT = 4
};

// 2. Define the exact HTTPUpload structure matching the ESP32 WebServer core
struct HTTPUpload {
    HTTPUploadStatus status;
    std::string filename;
    std::string name;
    uint8_t* buf;
    size_t currentSize;
    size_t totalSize;
};

// Values to prevent compiler errors for HTTP constants
#define HTTP_GET 1
#define HTTP_POST 2
#define UPDATE_SIZE_UNKNOWN 0

// Mock the global Update instance used in your upload blocks
class FakeUpdate {
public:
    bool hasError() { return false; }
    bool begin(int size) { return true; }
    int write(uint8_t* buf, int size) { return size; }
    bool end(bool clear) { return true; }
    template <typename T> void printError(T& s) {}
};
extern FakeUpdate Update;

#if defined(NATIVE_TESTING)
inline FakeUpdate Update;
#endif

// 3. Update the FakeWebServer to use our real HTTPUpload structure
class FakeWebServer {
private:
    HTTPUpload _mockUpload;

public:
    FakeWebServer() {
        _mockUpload.status = UPLOAD_FILE_START;
        _mockUpload.filename = "firmware.bin";
        _mockUpload.currentSize = 0;
        _mockUpload.totalSize = 0;
        _mockUpload.buf = nullptr;
    }

    // FIX: Allows wm.server.get() to compile on desktop by returning a raw pointer to itself
    FakeWebServer* get() { 
        return this; 
    }

    // Allows normal -> routing calls to work if needed elsewhere
    FakeWebServer* operator->() { 
        return this; 
    }

    void on(const char* uri, int method, std::function<void()> handler) {}
    void on(const char* uri, int method, std::function<void()> handler, std::function<void()> uploadHandler) {}
    void send(int code, const char* content_type, const char* content) {}
    void send(int code, const char* content_type, std::string content) {}
    
    // Returns a reference to our mutable mock upload tracking object
    HTTPUpload& upload() { 
        return _mockUpload; 
    }
};

using WebServer = FakeWebServer;

// The main class definition to satisfy the compiler
class WiFiManager {
public:
    // FIX: Changed from a raw pointer to a stack instance.
    // This allows the desktop compiler to support the dot syntax `wm.server.get()`
    FakeWebServer server; 

    WiFiManager() {}
    ~WiFiManager() {}

    void setAPCallback(void (*func)(WiFiManager*)) {}
    void setConfigPortalTimeout(int seconds) {}
    void setDebugOutput(bool debug) {}
    
    // Fixed: Changed from reference to pass-by-value vector to match the production layout
    void setMenu(std::vector<const char*> menu) {} 
    void setTitle(const char* title) {}
    void setCustomMenuHTML(const char* html) {}
    
    // Forces execution of the callback immediately during native tests
    // so that your internal lambda routes compile and run cleanly
    void setWebServerCallback(std::function<void()> cb) { 
        if(cb) cb(); 
    } 
    
    bool autoConnect(const char* apName, const char* apPassword) { return true; }
    void resetSettings() {}
};

#endif