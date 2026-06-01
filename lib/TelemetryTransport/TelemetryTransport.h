#ifndef TELEMETRY_TRANSPORT_H
#define TELEMETRY_TRANSPORT_H

#include <string>
#include <vector>
#include <variant>

class TelemetryTransport 
{
public:
    // The Explicit Transmission Result State Machine Engine
    enum class ResultState {
        Success,
        NetworkError,
        ProviderError,
        ValidationError,
        Uninitialized
    };

    // 1. Define the supported types using a type-safe variant
    using ValueType = std::variant<long, float, std::string>;

    // 2. Updated DataPoint now holds our multi-type variant
    struct DataPoint {
        std::string key;
        ValueType value;
    };

    virtual ~TelemetryTransport() = default;

    // 3. Pure Virtual Interface Contracts
    virtual bool begin() = 0;
    virtual ResultState transmit(const std::vector<DataPoint>& payload) = 0;
    virtual int getLatestSystemCode() const = 0; // Returns underlying raw HTTP/MQTT codes for debugging
};

#endif