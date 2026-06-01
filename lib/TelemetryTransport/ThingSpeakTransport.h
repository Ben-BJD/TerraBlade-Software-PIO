#ifndef THINGSPEAK_TRANSPORT_H
#define THINGSPEAK_TRANSPORT_H

#define TS_ENABLE_SSL

#include <Arduino.h>
#include "TelemetryTransport.h"
#include <WiFiClientSecure.h>
#include "ThingSpeak.h"
#include <map>

class ThingSpeakTransport : public TelemetryTransport {
private:
    WiFiClientSecure& _client;
    unsigned long _channelId;
    std::string _writeApiKey;
    int _lastHttpCode;
    const char* _rootCaCertificate; // Store the long-lived root CA anchor
    
    // Maps your clean string keys to ThingSpeak's literal Field Numbers (1-8)
    std::map<std::string, int> _fieldMapping; 

public:
    ThingSpeakTransport(WiFiClientSecure& client, unsigned long channelId, const std::string& apiKey, const char* rootCa);
    
    // Map individual string identity keys to dedicated backend dashboard registers
    void mapField(const std::string& key, int fieldNumber);

    bool begin() override;
    ResultState transmit(const std::vector<DataPoint>& payload) override;
    int getLatestSystemCode() const override;
};

#endif