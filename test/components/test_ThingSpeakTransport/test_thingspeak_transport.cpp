#include <unity.h>

// 1. Force the primary framework classes to load completely first
#include <Arduino.h>
#include <vector>
#include <string>

#include "../../test_helpers.h"                 
#include "../../../include/secrets.h"           
#include "../../../include/terrablade_config.h" 
#include <ThingSpeakTransport.h>

#if defined(NATIVE_TESTING)
ThingSpeakClass ThingSpeak;
#endif

// Global static instances allocated outside the limited thread stack
static WiFiClientSecure secureClient;
ThingSpeakTransport* transport = nullptr;

// Hardware-only helper to block execution and wait for human interaction
#if !defined(NATIVE_TESTING)
void initWifi() 
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(TEST_WIFI_SSID, TEST_WIFI_PASSWORD);

    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print('.');
        delay(300);
    }
    Serial.println("\n[SYSTEM] WiFi Connected cleanly!");
    Serial.print("[SYSTEM] IP Address: ");
    Serial.println(WiFi.localIP());
}
#else
void initWifi() {}
#endif

void setUp(void) 
{
    init_test_mocks();
    
    #if !defined(NATIVE_TESTING)
    // DIAGNOSTIC FALLBACK: If the certificate string is corrupt, this stops the panic.
    // Try this first. If it works, your certificate macro layout has a string literal bug.
    secureClient.setInsecure(); 
    
    // If you want to use the strict root CA instead, uncomment the line below and comment out setInsecure()
    // secureClient.setCACert(tspeak_ca_cert_root);
    #endif

    transport = new ThingSpeakTransport(secureClient, THINGSPEAK_CH_ID, THINGSPEAK_WRITE_APIKEY, tspeak_ca_cert_root);
}

void tearDown(void) 
{
    delete transport;
    transport = nullptr;
}

void test_tspeak_transmission() 
{
    transport->mapField("frequency", 1);
    transport->mapField("Vbattf", 2);
    transport->mapField("bootCount", 3);
    transport->begin();

    std::vector<TelemetryTransport::DataPoint> testPayload = {
        {"frequency", static_cast<long>(999999)},
        {"Vbattf", std::string("6.7")},
        {"bootCount", static_cast<long>(99)}
    };

    TelemetryTransport::ResultState result = transport->transmit(testPayload);
    TEST_ASSERT_EQUAL(TelemetryTransport::ResultState::Success, result);

    #if defined(NATIVE_TESTING)
    TEST_ASSERT_EQUAL_STRING("999999", ThingSpeak.getMockField(1).c_str());
    TEST_ASSERT_EQUAL_STRING("6.7", ThingSpeak.getMockField(2).c_str());
    TEST_ASSERT_EQUAL_STRING("99", ThingSpeak.getMockField(3).c_str());
    #endif
}

int runUnityTests(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_tspeak_transmission);
    UNITY_END();
    return 0;
}

/**
 * Structural Execution Handlers
 */
#if defined(NATIVE_TESTING)
extern "C" {
    int main(int argc, char **argv) 
    {
        return runUnityTests();
    }
}
#else
void setup() 
{
    // Fire up hardware UART pipeline instantly
    Serial.begin(115200);

    delay(3000); // Give the ESP32 serial monitor time to connect
    
    Serial.println("\n===========================================");
    Serial.println("   TARGET TESTING ACTIVE: RUNNING SUITE");
    Serial.println("===========================================");
    
    initWifi();
    
    Serial.println("[SYSTEM] Invoking Unity Test Framework Engine...");
    runUnityTests();

    Serial.println("\n[SYSTEM] Testing complete. Holding stable loop...");
}

void loop() 
{
    // Keep the CPU cleanly ticking over here forever so PlatformIO 
    // can comfortably read the buffers and exit on its own terms.
    delay(1000);
}
#endif