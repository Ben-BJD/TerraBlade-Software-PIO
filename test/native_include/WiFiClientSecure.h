#ifndef WIFI_CLIENT_SECURE_H
#define WIFI_CLIENT_SECURE_H

// A lightweight mock of the ESP32's WiFiClientSecure for native desktop testing
class WiFiClientSecure {
private:
    const char* _caCert = nullptr;
    bool _isInsecure = false;

public:
    WiFiClientSecure() = default;

    void setCACert(const char* cert) {
        _caCert = cert;
    }

    void setInsecure() {
        _isInsecure = true;
    }

    // Gettiing methods specifically for unit test assertions
    const char* getMockCACert() const { return _caCert; }
    bool isMockInsecure() const { return _isInsecure; }
};

#endif