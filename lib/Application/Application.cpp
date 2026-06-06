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

void Application::init(bool debug) 
{
    //Service Mode must be initialised first to ensure we can connect to Wi-Fi before doing anything else
    ServiceMode serviceMode(LED_PIN, RESET_BUTTON_PIN, AP_TITLE, AP_SSID, AP_PASSWORD);

    //TODO: We need a way to trigger ServiceMode::clearConfig() here. Include a "Double-Tap" feature on the reset button to allow users 
    //to force entry into configuration mode if they need to change Wi-Fi credentials or other settings. 
    //This is critical for usability in the field where users may need to update their network connection without re-flashing the device.
    //we will use power-cycling to enter config mode
    //A dedicated component should be made for this
    //How it works: Device boots up. It reads a value from the EEPROM/Flash memory.
    //It increments a "Boot Count" to 1.
    //It waits for 3 seconds.
    //If it survives the 3 seconds, it resets the count to 0.
    //The User Experience: To reset the WiFi, you pull the battery out. 
	//You push the magazine in (Device boots), pull it out 1 second later, and push it in again. The ESP32 sees "Boot Count = 2", 
    //realizes it was intentionally power-cycled, 
	//and resets the wifi manager settings so we can reconfigure ServiceMode::clearConfig()

    _currentState = State::ServiceMode;
    serviceMode.init();

    if( ServiceMode::State::Configured == ServiceMode::getState() )
    {
        //TODO: We have Wi-Fi credentials but need guards here to ensure WI-Fi is actually connected before trying to do anything else
        // - this is a known point of failure that needs to be hardened with retry logic and timeouts to ensure we don't get stuck here 
        //indefinitely if there is a network issue or the credentials are wrong.

        _currentState = State::WifiConnecting;
        while (WiFi.status() != WL_CONNECTED) 
        {
            _currentState = State::WifiConnectingError;
            delay(300);
        }

        //Initialisation
        Schedule scheduler(SLEEP_TIME_SECONDS, RESET_BUTTON_PIN);
        scheduler.initialize();

        Sensor sensor(PROBE_POWER_PIN, PROBE_SIGNAL_PIN, BATTERY_PIN, DRY_FREQUENCY, WET_FREQUENCY);
        sensor.initialise();

        WiFiClientSecure wifiClient;
        ThingSpeakTransport telemetryTransport(wifiClient, THINGSPEAK_CH_ID, THINGSPEAK_WRITE_APIKEY, tspeak_ca_cert_root);

        //Telemetry
        int bootCount = scheduler.getBootCount();
        long soilMoisture = sensor.measureSoilMoisture();
        float batteryVoltage = sensor.measureBatteryVoltage();

        //TODO: soilMoisture returns a raw htz value that we want to convert to a percentage for the dashboard. 
        //We can do this by mapping the raw value to a 0-100 range based on our calibration data.
        //For example, if our calibration data tells us that a raw value of 100000 corresponds to 0% moisture (completely dry) 
        //and a raw value of 13000 corresponds to 100% moisture (fully saturated)
        //we created Sensor::calculateMoisturePercent() to do this mapping for us.
        //further we need to add range checks to ensure we don't report values outside of 0-100% due to sensor noise or anomalies.

        //Transmission
        _currentState = State::TelemetryTransmitting;
        
        telemetryTransport.mapField("soilMoisture", 1);
        telemetryTransport.mapField("Vbattf", 2);
        telemetryTransport.mapField("bootCount", 3);
        telemetryTransport.begin();

        std::vector<TelemetryTransport::DataPoint> payload = {
            {"soilMoisture", static_cast<long>(soilMoisture)},
            {"Vbattf", std::string(std::to_string(batteryVoltage))},
            {"bootCount", static_cast<long>(bootCount)}
        };

        TelemetryTransport::ResultState result = telemetryTransport.transmit(payload);
        
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
            //TODO: Handle different error states (NetworkError, ProviderError, ValidationError)
            // This is critical for robustness and should be expanded with retry logic
        } 
    }
    else
    {
        _currentState = State::ServiceModeError;
        //TODO: If we get here it means something went wrong with Service Mode.
        //Likely due to a timeout hosting the captive portal.
        //Here we should have some retry logic to attempt to re-enter Service Mode a few times before giving up.
    }
}