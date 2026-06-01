#include "ThingSpeakTransport.h"

ThingSpeakTransport::ThingSpeakTransport(WiFiClientSecure& client, unsigned long channelId, const std::string& apiKey, const char* rootCa)
    : _client(client), _channelId(channelId), _writeApiKey(apiKey), _lastHttpCode(0), _rootCaCertificate(rootCa) {}

void ThingSpeakTransport::mapField(const std::string& key, int fieldNumber) 
{
    if (fieldNumber >= 1 && fieldNumber <= 8) {
        _fieldMapping[key] = fieldNumber;
    }
}

bool ThingSpeakTransport::begin() 
{
    _client.setCACert(_rootCaCertificate);
    
    ThingSpeak.begin(_client);
    _lastHttpCode = 0;
    return true;
}

TelemetryTransport::ResultState ThingSpeakTransport::transmit(const std::vector<DataPoint>& payload) 
{
    if (_fieldMapping.empty()) {
        return ResultState::ValidationError;
    }

    // 1. Map raw key-value elements directly into ThingSpeak's internal multi-field register buffers
    for (const auto& dataPoint : payload) 
    {
        auto it = _fieldMapping.find(dataPoint.key);
        if (it != _fieldMapping.end()) 
        {
            int targetField = it->second;

            // Use std::visit to resolve the variant type at runtime 
            // and compile the matching ThingSpeak overloaded call.
            std::visit([targetField](auto&& arg) 
            {
                // Inside here, 'arg' is automatically deduced to be either long, float, or std::string
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, std::string>) 
                {
                    // Convert std::string to Arduino String expected by ThingSpeak library
                    ThingSpeak.setField(targetField, String(arg.c_str()));
                } 
                else 
                {
                    // Handles float and long directly via native library overloading
                    ThingSpeak.setField(targetField, arg);
                }
            }, dataPoint.value);
        }
    }

    // 2. Dispatch payload block over the active Wi-Fi radio stack
    _lastHttpCode = ThingSpeak.writeFields(_channelId, _writeApiKey.c_str());

    // 3. Map the raw HTTP response code cleanly to your abstract status enum
    if (_lastHttpCode == 200) {
        return ResultState::Success;
    } else if (_lastHttpCode == -301 || _lastHttpCode == -302 || _lastHttpCode == -303) {
        return ResultState::NetworkError;
    } else {
        return ResultState::ProviderError;
    }
}

int ThingSpeakTransport::getLatestSystemCode() const {
    return _lastHttpCode;
}