#include "Application.h"
#include "../../include/terrablade_config.h"
#include "../../include/secrets.h"

#include <Schedule.h>
#include <Sensor.h>
#include <ServiceMode.h>
#include <WiFiClientSecure.h>
#include <ThingSpeakTransport.h>

Application::State Application::_currentState = Application::State::Uninitialized;

Application::Application(){}

bool Application::isValidMoistureReading(long moistureValue) {
    // Check if moisture value is within valid range (0-100%)
    return (moistureValue >= 0 && moistureValue <= 100);
}

void Application::init(bool debug) 
{
    //Service Mode must be initialised first to ensure we can connect to Wi-Fi before doing anything else
    ServiceMode serviceMode(LED_PIN, RESET_BUTTON_PIN, AP_TITLE, AP_SSID, AP_PASSWORD, PREF_NAMESPACE, BOOT_COUNT_KEY);

    Schedule scheduler(SLEEP_TIME_SECONDS, RESET_BUTTON_PIN);
    scheduler.initialize();

    /*On Power-On Reset, we want to check if the user is trying to trigger a reset into Service Mode by 
    power-cycling the device in a specific pattern.
    We don't want to accidentally wipe the user's Wi-Fi credentials every time they power-cycle the device, 
    so we need a way to differentiate between normal power cycles and intentional ones meant to trigger a reset into Service Mode.
    Also we don't want to do this for regular deep sleep wakeups*/
    if(scheduler.getBootSource() == Schedule::BootSource::PowerOnReset)
    {
        if ( ServiceMode::checkDoubleTap() ) 
        {
            ServiceMode::clearConfig();
        }
    }

    _currentState = State::ServiceMode;
    serviceMode.init();

    if( ServiceMode::State::Configured == ServiceMode::getState() )
    {
        // Connect to WiFi with 10s timeout
        _currentState = State::WifiConnecting;
        unsigned long startAttemptTime = millis();
        
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MILLISECONDS) 
        {
            delay(300);
        }
        
        if (WiFi.status() != WL_CONNECTED) 
        {
            _currentState = State::WifiConnectingError;
            // WiFi connection failed, go to sleep
            if(!debug) 
            {
                scheduler.enterDeepSleep();
            }
            return;
        }

        //Initialisation
        Sensor sensor(PROBE_POWER_PIN, PROBE_SIGNAL_PIN, BATTERY_PIN, DRY_FREQUENCY, WET_FREQUENCY);
        sensor.initialise();

        WiFiClientSecure wifiClient;
        ThingSpeakTransport telemetryTransport(wifiClient, THINGSPEAK_CH_ID, THINGSPEAK_WRITE_APIKEY, tspeak_ca_cert_root);

        //Telemetry
        int bootCount = scheduler.getBootCount();
        long soilMoisture = sensor.measureSoilMoisture();
        float batteryVoltage = sensor.measureBatteryVoltage();
        
        // Convert raw frequency to percentage and validate
        long moisturePercent = sensor.calculateMoisturePercent(soilMoisture);
        
        // If the sensor fails or returns out-of-bounds frequencies, abort transmission
        if (!isValidMoistureReading(moisturePercent)) 
        {
            // Invalid moisture reading, go straight to deep sleep without sending data
            if(!debug) 
            {
                scheduler.enterDeepSleep();
            }
            return;
        }

        //Transmission with retry logic (3 attempts)
        _currentState = State::TelemetryTransmitting;
        
        telemetryTransport.mapField("soilMoisture", 1);
        telemetryTransport.mapField("Vbattf", 2);
        telemetryTransport.mapField("bootCount", 3);
        telemetryTransport.begin();

        std::vector<TelemetryTransport::DataPoint> payload = {
            {"soilMoisture", static_cast<long>(moisturePercent)},
            {"Vbattf", std::string(std::to_string(batteryVoltage))},
            {"bootCount", static_cast<long>(bootCount)}
        };

        // Retry logic: Attempt transmission up to n times
        TelemetryTransport::ResultState result = TelemetryTransport::ResultState::Uninitialized;
        int maxRetries = TRANSMISSION_MAX_RETRIES;
        int retries = 0;
        
        while (retries < maxRetries) {
            result = telemetryTransport.transmit(payload);
            
            if (result == TelemetryTransport::ResultState::Success) {
                break; // Success, exit retry loop
            }
            
            retries++;
            if (retries < maxRetries) {
                delay(TRANSMISSION_RETRY_DELAY_MILLISECONDS); // Wait before retrying
            }
        }
        
        if(TelemetryTransport::ResultState::Success == result)
        {
            //Now go to sleep - we have successfully transmitted our data and can rest until the next scheduled wakeup
            _currentState = State::Finished;
            
            if(!debug) 
            {
                scheduler.enterDeepSleep();
            }  
        }
        else
        {
            _currentState = State::TelemetryTransmittingError;
            // Any transmission error after retries triggers immediate deep sleep to preserve battery
            if(!debug) 
            {
                scheduler.enterDeepSleep();
            }
        } 
    }
    else
    {
        _currentState = State::ServiceModeError;
        // Service Mode failed, go to sleep
        if(!debug) 
        {
            scheduler.enterDeepSleep();
        }
    }
}