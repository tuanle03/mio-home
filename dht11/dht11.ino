#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

// Wi-Fi credentials
const char *ssid = "Tuan Le";
const char *password = "*1234567890*";

// Web server on port 80
WebServer server(80);

// DHT sensor settings
DHT dht(12, DHT11);

// Sensor data
unsigned long lastDHTReadTime = 0;
float lastTemperature = NAN;
float lastHumidity = NAN;
float currentTemperature = NAN;
float currentHumidity = NAN;

// HTML page
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'>
    <title>Mionguci Server</title>
    <style>
        html { font-family: Arial; text-align: center; margin: 0px auto; }
        h2 { font-size: 2.5rem; }
        h3 { font-size: 2rem; color: #ff5722; }
        p { font-size: 1.5rem; margin: 10px; }
        .units { font-size: 1.2rem; }
        .dht-labels { font-size: 1.5rem; vertical-align: middle; }
    </style>
    <script>
        async function pollData() {
            try {
                const response = await fetch('/sensor-data');
                if (response.status === 200) {
                    const data = await response.json();
                    document.getElementById('temperature').innerText = data.temperature.toFixed(2);
                    document.getElementById('humidity').innerText = data.humidity.toFixed(2);
                }
            } catch (error) {
                console.error('Error fetching sensor data:', error);
            } finally {
                // continue polling data after receiving response
                pollData();
            }
        }
        window.onload = pollData;
    </script>
</head>
<body>
    <h2>Mionguci Server</h2>
    <h3>🔥 Cnmeo@@ 🔥</h3>
    <p>
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>
        <span class='dht-labels'>Temperature:</span>
        <span id='temperature'>--</span>
        <sup class='units'>&deg;C</sup>
    </p>
    <p>
        <i class='fas fa-tint' style='color:#00add6;'></i>
        <span class='dht-labels'>Humidity:</span>
        <span id='humidity'>--</span>
        <sup class='units'>&percnt;</sup>
    </p>
</body>
</html>
)rawliteral";

// Update DHT data and check for changes
bool updateDHTData() {
    unsigned long currentTime = millis();
    if (currentTime - lastDHTReadTime >= 2000) { // update every 2 seconds
        lastDHTReadTime = currentTime;
        float newTemperature = dht.readTemperature();
        float newHumidity = dht.readHumidity();

        if (isnan(newTemperature) || isnan(newHumidity)) {
            Serial.println("Failed to read from DHT sensor!");
            return false;
        }

        // check if the new data is different from the current data
        if (newTemperature != currentTemperature || newHumidity != currentHumidity) {
            lastTemperature = currentTemperature;
            lastHumidity = currentHumidity;
            currentTemperature = newTemperature;
            currentHumidity = newHumidity;
            return true; // have changes
        }
    }
    return false; // no changes
}

// Serve the main HTML page
void handleRoot() {
    server.send(200, "text/html; charset=utf-8", HTML_PAGE);
}

// Long polling logic
void handleSensorData() {
    unsigned long startTime = millis();
    while (true) {
        if (updateDHTData()) { // have changes
            char json[50];
            snprintf(json, sizeof(json), "{\"temperature\":%.2f,\"humidity\":%.2f}", currentTemperature, currentHumidity);
            server.send(200, "application/json", json);
            return;
        }

        // max waiting time is 30 seconds
        if (millis() - startTime >= 30000) {
            server.send(204, "application/json", ""); // no content
            return;
        }

        delay(100); // loop delay to avoid busy-waiting
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Start MDNS responder
    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    // Configure server routes
    server.on("/", handleRoot);
    server.on("/sensor-data", handleSensorData);

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
