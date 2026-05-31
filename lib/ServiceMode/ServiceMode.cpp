#include "ServiceMode.h"
#include <WiFi.h>
#include <WiFiManager.h>

// Initialize all static variables
volatile bool ServiceMode::_factoryResetRequested = false;
int ServiceMode::_ledPin = -1;
int ServiceMode::_btnPin = -1;

// Explicitly allocate the static string pointers
const char* ServiceMode::_apTitle = nullptr;
const char* ServiceMode::_apSSID = nullptr;
const char* ServiceMode::_apPassword = nullptr;

// Allocate and initialize the starting state
ServiceMode::State ServiceMode::_currentState = ServiceMode::State::Uninitialized;

ServiceMode::ServiceMode(int ledPin, int btnPin, const char* apTitle, const char* apSSID, const char* apPassword)
{
    _ledPin = ledPin;
    _btnPin = btnPin;
    _apTitle = apTitle;
    _apSSID = apSSID;
    _apPassword = apPassword;
}

void ServiceMode::init()
{
    // Transition to Configuring immediately as we start setup
    _currentState = State::Configuring;

    // Set up the reset button with an interrupt
    pinMode(_btnPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_btnPin), onFactoryReset, RISING);

    pinMode(_ledPin, OUTPUT);// initialize digital pin LED_BUILTIN as an output.
    digitalWrite(_ledPin, LOW); // Start with the LED off. We will turn it on in config mode.

    // Initialize WiFiManager
    // Local intialization. There is no need to keep it in memory after setup() has finished.
    WiFiManager wm;

    // Custom HTML for the update page. This will be served when the user clicks the "Firmware Update" button in the menu.
    const char* update_html = "<h2>Firmware Update</h2><form method='POST' action='/do_update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Upload'></form>";

    wm.setAPCallback(configModeCallback);// Set the callback that will run when we enter configuration mode

    // Set a timeout so the ESP doesn't hang forever if nobody configures the WiFi
    wm.setConfigPortalTimeout(300); // Auto close portal after 5 minutes

    // Print debug info to the Serial monitor (helpful for troubleshooting)
    wm.setDebugOutput(false); 
    // -------------------------

    // Wifi Manager CUSTOMISATIONS: 

    // ==========================================================
    // CUSTOMIZE THE MENU
    // ==========================================================
    // Available options: "wifi", "wifinoscan", "info", "param", "close", "sep", "erase", "restart", "exit"
    // Let's remove all but the configuration and update options to simplify the user experience.
    std::vector<const char *> menu = {"wifi", "sep", "update"};
    wm.setMenu(menu);

    //Custom Title
    wm.setTitle(_apTitle);

    // --- Bind the custom "Update" page ---
    // This adds a new button to the menu that will serve the update form when clicked.
    wm.setCustomMenuHTML("<form action='/update' method='get'><button>Firmware Update</button></form>");

    // --- Handle the Routes ---
    // This serves the upload form
    auto* serverPtr = wm.server; // Grab the raw pointer to the server sub-component

    wm.setWebServerCallback([serverPtr, update_html]() 
    {
        // Capture the raw pointer by value. 
        // The pointer address stays identical, so it references the exact same web server.
        serverPtr->on("/update", HTTP_GET, [serverPtr, update_html]() {
            serverPtr->send(200, "text/html", update_html);
        });

        // 2. Capture 'this' explicitly so the lambda can safely call the class method 'onError' later.
        serverPtr->on("/do_update", HTTP_POST, [serverPtr]() 
        {
            serverPtr->send(200, "text/plain", (Update.hasError()) ? "Update Failed" : "Update Success! Rebooting...");
            delay(1000);
            ESP.restart();
        }, [serverPtr]()
        {
            HTTPUpload& upload = serverPtr->upload();
            if (upload.status == UPLOAD_FILE_START) 
            {
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) 
                {
                    ServiceMode::onError("Failed to begin update: " + upload.filename);
                }
            } 
            else if (upload.status == UPLOAD_FILE_WRITE) 
            {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) 
                {
                    ServiceMode::onError("Failed to write update: " + upload.filename);
                }
            } 
            else if (upload.status == UPLOAD_FILE_END) 
            {
                if (!Update.end(true))
                {
                    ServiceMode::onError("Failed to complete update: " + upload.filename);
                }
            }
        });
    });

    // autoConnect() performs the magic.
    // It tries to connect to the last saved WiFi. 
    //    - WiFiManager stores the SSID/password as key/value pairs via NVS and reads them back on boot.
    // If it fails or no WiFi is saved, it sets up an Access Point.
    bool res;
    res = wm.autoConnect(_apSSID, _apPassword);

    // Check the result
    if(!res)
    {
        ServiceMode::onError("Failed to complete WifiManager configuration");
    } 
    else 
    {
        digitalWrite(_ledPin, LOW); // Turn off the LED since we're no longer in config mode

        // If you get here, you have successfully connected to the local router!
        //The ESP will continue with the rest of your setup() code after this init() function finishes.
        _currentState = State::Configured;
    }
}

void ServiceMode::onError(const std::string& errorMsg)
{
    _currentState = State::Error;
    digitalWrite(_ledPin, LOW); // Turn off the LED since we're no longer in config mode
    delay(1000);
    ESP.restart();
}

// 3. The actual raw ISR execution block
void IRAM_ATTR ServiceMode::onFactoryReset() 
{
    _factoryResetRequested = true;
}

void ServiceMode::onLoop() 
{
    // Check if the factory reset button was pressed
    if(_factoryResetRequested)
    {
        // do the heavy lifting outside of interrupt context
        WiFiManager wm;
        wm.resetSettings();        // clears WifiManager NVS
        delay(100);                // give NVS a moment to commit
        ESP.restart();             // now reboot
    }
}

void ServiceMode::configModeCallback(WiFiManager *wiFiManager) 
{
    _currentState = State::CaptivePortalActive;
    digitalWrite(_ledPin, HIGH);
}