#ifndef THINGSPEAK_MOCK_H
#define THINGSPEAK_MOCK_H

#include <string>
#include <map>
#include "WiFiClientSecure.h"

class String;

class ThingSpeakClass {
private:
    WiFiClientSecure* _clientRef = nullptr;
    unsigned long _lastChannelId = 0;
    std::string _lastApiKey = "";
    
    std::map<int, std::string> _mockFields;
    int _mockResponseCode = 200; 

public:
    ThingSpeakClass() = default;

    void begin(WiFiClientSecure& client) {
        _clientRef = &client;
    }

    int setField(int field, int value) {
        _mockFields[field] = std::to_string(value);
        return 1;
    }

    int setField(int field, long value) {
        _mockFields[field] = std::to_string(value);
        return 1;
    }

    int setField(int field, float value) {
        _mockFields[field] = std::to_string(value);
        return 1;
    }

    // Defined inline right here to satisfy the compiler without an external implementation file
    inline int setField(int field, const String& value);

    int writeFields(unsigned long channelNumber, const char* writeAPIKey) {
        _lastChannelId = channelNumber;
        _lastApiKey = writeAPIKey ? writeAPIKey : "";
        return _mockResponseCode;
    }

    // --- TEST HOOKS ---
    void resetMock() {
        _mockFields.clear();
        _lastChannelId = 0;
        _lastApiKey = "";
        _mockResponseCode = 200;
    }

    void setMockResponseCode(int code) { _mockResponseCode = code; }
    std::string getMockField(int field) { return _mockFields[field]; }
    unsigned long getMockChannelId() const { return _lastChannelId; }
    std::string getMockApiKey() const { return _lastApiKey; }
};

// We need to resolve what String looks like before defining our inline function body
#if defined(NATIVE_TESTING)
#include <Arduino.h>
#endif

inline int ThingSpeakClass::setField(int field, const String& value) {
    _mockFields[field] = std::string(value.c_str());
    return 1;
}

// Declared extern so any file including this header knows it exists globally
extern ThingSpeakClass ThingSpeak;


#endif