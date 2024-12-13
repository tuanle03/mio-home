#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <IRremote.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi credentials
const char* ssid = "Cnmeow";
const char* password = "iluvyou";

// Web server on port 80
WebServer server(80);

// NTP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // GMT+7 (Vietnam)

// DHT sensor settings
DHT dht(19, DHT11);

// Sensor data
unsigned long lastDHTReadTime = 0;
float lastTemperature = NAN;
float lastHumidity = NAN;
float currentTemperature = NAN;
float currentHumidity = NAN;

// IR remote control settings
int RECV_PIN = 14;
#define IR_SEND_PIN 2

// Fan IR signals
unsigned long NEW_FAN_POWER = 0x1FE58A7;
unsigned long NEW_FAN_SPEED = 0x1FE807F;
unsigned long NEW_FAN_SWING = 0x1FEC03F;
unsigned long NEW_FAN_MODE = 0x1FE20DF;
unsigned long NEW_FAN_COOL = 0x1FEE01F;
unsigned long NEW_FAN_IONIZER = 0x1FE906F;

// Timer settings
unsigned long fanOffTime = 0;
int scheduledHour = -1;
int scheduledMinute = -1;
bool isFanOffScheduled = false;

// Temperature-based auto control settings
float autoControlHighTemp = 30.0;
float autoControlLowTemp = 25.0;
bool autoControlEnabled = false;
bool fanState = false;

// HTML page
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'>
    <title>Mio Control</title>
    <style>
        html {
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0 auto;
            background: #f0f4f8;
            color: #333;
        }
        body {
            margin: 0;
            padding: 0;
        }
        h2 {
            font-size: 2.5rem;
            margin: 20px 0;
        }
        .container {
            max-width: 800px;
            margin: 20px auto;
            padding: 20px;
            background: #fff;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            box-sizing: border-box;
        }
        .buttons button {
            font-size: 1.2rem;
            padding: 10px 20px;
            margin: 10px;
            cursor: pointer;
            border: none;
            border-radius: 5px;
            color: white;
            transition: background-color 0.3s;
        }
        .btn-power { background-color: #f44336; }
        .btn-speed { background-color: #ff9800; }
        .btn-swing { background-color: #555555; }
        .btn-mode { background-color: #e7e7e7; color: black; }
        .btn-cool { background-color: #2196f3; }
        .btn-ionizer { background-color: #4caf50; }
        .weather, .sensor {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin: 15px 0;
            padding: 10px;
            border-radius: 8px;
            background: #f9f9f9;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        .weather-icon {
            width: 50px;
            height: 50px;
        }
        #notify {
            position: fixed;
            bottom: 20px;
            right: 20px;
            background-color: #333;
            color: white;
            padding: 15px;
            border-radius: 5px;
            display: none;
        }
        .settings-wrapper {
            display: flex;
            flex-wrap: wrap;
            justify-content: space-between;
            gap: 20px;
        }
        .settings-container {
            flex: 1 1 calc(50% - 20px); /* Set width to 50% minus gap */
            max-width: calc(50% - 20px);
            padding: 15px;
            background: #f9f9f9;
            border-radius: 10px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            box-sizing: border-box;
            margin-top: 15px;
            mar
        }
        .settings-container h3 {
            margin-top: 0;
        }
        .timer-inputs, .temperature-inputs {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 5px;
        }
        .timer-inputs select, .temperature-inputs input {
            width: 60px;
            padding: 5px;
            border: 1px solid #ccc;
            border-radius: 5px;
            text-align: center;
        }
        button {
            width: 100px;
            height: 50px;
            padding: 0;
            font-size: 14px;
            border: none;
            border-radius: 5px;
            color: white;
            text-align: center;
            cursor: pointer;
            transition: background-color 0.3s ease;
            margin-top: 5px;
        }
        button:hover {
            opacity: 0.9;
            transform: scale(1.05);
        }
        .settings {
            flex: 1;
            text-align: center;
            padding: 10px;
            background: #f9f9f9;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        .settings input, .settings select {
            padding: 10px;
            margin: 5px;
            border: 1px solid #ccc;
            border-radius: 5px;
            text-align: center;
        }
        .settings label {
            display: block;
            margin: 10px 0 5px;
        }
    </style>
    <script>
        async function fetchWeather() {
            try {
                const response = await fetch('https://api.weatherstack.com/current?access_key=**********&query=10.870725,106.802152');
                const data = await response.json();
                document.getElementById('outdoor-temperature').innerText = data.current.temperature + '°C';
                document.getElementById('outdoor-humidity').innerText = data.current.humidity + '%';
                document.getElementById('weather-description').innerText = data.current.weather_descriptions[0];
                document.getElementById('weather-icon').src = data.current.weather_icons[0];
            } catch (error) {
                console.error('Error fetching weather data:', error);
            }
        }

        async function pollSensorData() {
            try {
                const response = await fetch('/sensor-data');
                if (response.status === 200) {
                    const data = await response.json();
                    document.getElementById('indoor-temperature').innerText = data.temperature.toFixed(1) + '°C';
                    document.getElementById('indoor-humidity').innerText = data.humidity.toFixed(0) + '%';
                }
            } catch (error) {
                console.error('Error fetching sensor data:', error);
            } finally {
                setTimeout(pollSensorData, 5000);
            }
        }

        async function sendCommand(command) {
            try {
                const response = await fetch('/' + command);
                const result = await response.text();
                showNotification(result);
            } catch (error) {
                console.error('Error sending command:', error);
            }
        }

        async function fetchSettings() {
            try {
                const response = await fetch('/settings');
                if (response.status === 200) {
                    const data = await response.json();
                    document.getElementById('current-off-time').innerText = data.offTime === '-1:-1' ? 'Not Set' : data.offTime;
                    document.getElementById('current-threshold').innerText = data.autoThreshold;
                }
            } catch (error) {
                console.error('Error fetching settings:', error);
            }
        }

        async function setTimer() {
            const hour = document.getElementById('timer-hour').value;
            const minute = document.getElementById('timer-minute').value;
            if (hour !== '' && minute !== '') {
                const time = `${hour.padStart(2, '0')}:${minute.padStart(2, '0')}`;
                await sendCommand(`set-timer?time=${time}`);
            } else {
                alert('Please select both hour and minute.');
            }
        }

        async function setAutoControl() {
            const lowTemp = document.getElementById('low-temp').value;
            const highTemp = document.getElementById('high-temp').value;

            if (lowTemp === '' || highTemp === '') {
                alert('Vui lòng nhập cả nhiệt độ thấp và nhiệt độ cao.');
                return;
            }

            if (parseFloat(lowTemp) >= parseFloat(highTemp)) {
                alert('Nhiệt độ thấp phải nhỏ hơn nhiệt độ cao.');
                return;
            }

            try {
                const response = await fetch(`/set-auto-threshold?low=${lowTemp}&high=${highTemp}`);
                if (response.ok) {
                    showNotification('Thiết lập thành công!');
                } else {
                    showNotification('Có lỗi xảy ra khi thiết lập. Vui lòng thử lại.');
                }
            } catch (error) {
                console.error('Error setting auto control:', error);
                showNotification('Không thể kết nối tới server.');
            }
        }

        function showNotification(message) {
            const notify = document.getElementById('notify');
            notify.innerText = message;
            notify.style.display = 'block';
            setTimeout(() => { notify.style.display = 'none'; }, 3000);
        }

        window.onload = () => {
            fetchWeather();
            pollSensorData();
            fetchSettings();
        };
    </script>
</head>
<body>
    <div class="container">
        <h2>Meow Remote</h2>

        <div class="weather">
            <div>
                <h3>Outdoor Weather</h3>
                <p>Temperature: <span id="outdoor-temperature">--</span></p>
                <p>Humidity: <span id="outdoor-humidity">--</span></p>
                <p>Description: <span id="weather-description">--</span></p>
            </div>
            <img id="weather-icon" class="weather-icon" src="" alt="Weather Icon">
        </div>

        <div class="sensor">
            <h3>Indoor Conditions</h3>
            <p>Temperature: <span id="indoor-temperature">--</span></p>
            <p>Humidity: <span id="indoor-humidity">--</span></p>
        </div>

        <div class="buttons">
            <button class="btn btn-power" onclick="sendCommand('power')">Power</button>
            <button class="btn btn-speed" onclick="sendCommand('speed')">Speed</button>
            <button class="btn btn-swing" onclick="sendCommand('swing')">Swing</button>
            <button class="btn btn-mode" onclick="sendCommand('mode')">Mode</button>
            <button class="btn btn-cool" onclick="sendCommand('cool')">Cool</button>
            <button class="btn btn-ionizer" onclick="sendCommand('ionizer')">Ionizer</button>
        </div>

        <div class="settings">
            <h3>Current Settings</h3>
            <p>Timer Off Time: <span id="current-off-time">Not Set</span></p>
            <p>Auto Control: <span id="current-threshold">Disabled</span></p>
        </div>

        <div class="settings-wrapper">
            <div class="settings-container">
                <h3>Timer Settings</h3>
                <label for="timer-hour">Timer On/Off</label>
                <div class="timer-inputs">
                    <select id="timer-hour">
                        <option value="">Hour</option>
                        <script>
                            for (let i = 0; i < 24; i++) {
                                document.write(`<option value="${i}">${i}</option>`);
                            }
                        </script>
                    </select>
                    <span>:</span>
                    <select id="timer-minute">
                        <option value="">Minute</option>
                        <script>
                            for (let i = 0; i < 60; i++) {
                                document.write(`<option value="${i}">${i}</option>`);
                            }
                        </script>
                    </select>
                </div>
                <button onclick="setTimer()" style="background-color: #6C7575;">Set</button>
            </div>

            <div class="settings-container">
                <h3>Auto Control</h3>
                <label for="temperature-range">Temperature Range (°C)</label>
                <div class="temperature-inputs">
                    <input type="number" id="low-temp" step="0.1" placeholder="Low">
                    <span>-</span>
                    <input type="number" id="high-temp" step="0.1" placeholder="High">
                </div>
                <button onclick="setAutoControl()" style="background-color: #6C7575;">Set</button>
            </div>
        </div>
    </div>

    <div id="notify"></div>
</body>
</html>
)rawliteral";

// Function prototypes
void handleRoot();
void handleSensorData();
void handleCommand();
void handleSetOffTime();
void handleSetAutoThreshold();

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Initialize DHT sensor
  dht.begin();

  // Start NTP client
  timeClient.begin();

  // Initialize IR receiver and sender
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN);

  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/sensor-data", handleSensorData);
  server.onNotFound(handleCommand);
  server.on("/set-timer", handleSetOffTime);
  server.on("/set-auto-threshold", handleSetAutoThreshold);
  server.on("/settings", handleGetSettings);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  timeClient.update();

  // Read DHT sensor periodically
  if (millis() - lastDHTReadTime >= 2000) {
    lastDHTReadTime = millis();
    currentTemperature = dht.readTemperature();
    currentHumidity = dht.readHumidity();

    if (isnan(currentTemperature) || isnan(currentHumidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    if (!isnan(currentTemperature)) {
      lastTemperature = currentTemperature;
    }
    if (!isnan(currentHumidity)) {
      lastHumidity = currentHumidity;
    }
  }

  // Turn off fan if off time is reached
  if (isFanOffScheduled) {
    checkFanOffTime();
  }

  // Logic auto control
  if (autoControlEnabled) {
    if (!fanState && currentTemperature >= autoControlHighTemp) {
      fanState = true;
      Serial.println("Temperature exceeded high threshold. Turning fan ON.");
      IrSender.sendNEC(NEW_FAN_POWER, 32);
    } else if (fanState && currentTemperature <= autoControlLowTemp) {
      fanState = false;
      Serial.println("Temperature dropped below low threshold. Turning fan OFF.");
      IrSender.sendNEC(NEW_FAN_POWER, 32);
    }
  }

  // Check IR receiver for new signal
  if (IrReceiver.decode()) {
    Serial.print("IR signal received: ");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    Serial.print("IR signal protocol: ");
    Serial.println(IrReceiver.decodedIRData.protocol);
    IrReceiver.resume();
  }
}

void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", HTML_PAGE);
}

void handleSensorData() {
  String jsonResponse = "{\"temperature\":" + String(lastTemperature) + ",\"humidity\":" + String(lastHumidity) + "}";
  server.send(200, "application/json", jsonResponse);
}

void handleCommand() {
  String command = server.uri().substring(1);

  if (command == "power") {
    IrSender.sendNEC(NEW_FAN_POWER, 32);
    server.send(200, "text/plain", "Fan Power Toggled");
    Serial.println("Fan Power Toggled");
  } else if (command == "speed") {
    IrSender.sendNEC(NEW_FAN_SPEED, 32);
    server.send(200, "text/plain", "Fan Speed Changed");
    Serial.println("Fan Speed Changed");
  } else if (command == "swing") {
    IrSender.sendNEC(NEW_FAN_SWING, 32);
    server.send(200, "text/plain", "Fan Swing Toggled");
    Serial.println("Fan Swing Toggled");
  } else if (command == "mode") {
    IrSender.sendNEC(NEW_FAN_MODE, 32);
    server.send(200, "text/plain", "Fan Mode Changed");
    Serial.println("Fan Mode Changed");
  } else if (command == "cool") {
    IrSender.sendNEC(NEW_FAN_COOL, 32);
    server.send(200, "text/plain", "Fan Cool Mode Toggled");
    Serial.println("Fan Cool Mode Toggled");
  } else if (command == "ionizer") {
    IrSender.sendNEC(NEW_FAN_IONIZER, 32);
    server.send(200, "text/plain", "Fan Ionizer Toggled");
    Serial.println("Fan Ionizer Toggled");
  } else {
    server.send(404, "text/plain", "Command Not Found");
    Serial.println("Command Not Found");
  }
}

void handleSetOffTime() {
  if (server.hasArg("time")) {
    String time = server.arg("time");
    int separator = time.indexOf(':');
    if (separator != -1) {
      scheduledHour = time.substring(0, separator).toInt();
      scheduledMinute = time.substring(separator + 1).toInt();
      isFanOffScheduled = true;

      Serial.printf("Timer set to %02d:%02d\n", scheduledHour, scheduledMinute);
      server.send(200, "text/plain", "Off time set successfully");
    } else {
      server.send(400, "text/plain", "Invalid time format");
    }
  } else {
    server.send(400, "text/plain", "Time parameter missing");
  }
}

void checkFanOffTime() {
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  if (currentHour == scheduledHour && currentMinute == scheduledMinute) {
    isFanOffScheduled = false;
    // Turn off the fan (Add your IR command here)
    fanState = false;
    IrSender.sendNEC(NEW_FAN_POWER, 32);
    Serial.println("Fan turned off as scheduled.");
  }
}

void handleSetAutoThreshold() {
  if (!server.hasArg("low") || !server.hasArg("high")) {
    server.send(400, "text/plain", "Thiếu tham số low hoặc high.");
    return;
  }

  float lowTemp = server.arg("low").toFloat();
  float highTemp = server.arg("high").toFloat();

  if (lowTemp >= highTemp) {
    server.send(400, "text/plain", "Nhiệt độ thấp phải nhỏ hơn nhiệt độ cao.");
    return;
  }

  autoControlLowTemp = lowTemp;
  autoControlHighTemp = highTemp;
  autoControlEnabled = true;

  server.send(200, "text/plain", "Đã thiết lập nhiệt độ tự động.");
  Serial.printf("Auto control set: Low = %.2f, High = %.2f\n", lowTemp, highTemp);
}

// Web server handler to get current settings
void handleGetSettings() {
  String jsonResponse = "{";
  jsonResponse += "\"offTime\":\"" + String(scheduledHour) + ":" + String(scheduledMinute) + "\",";
  jsonResponse += "\"autoThreshold\":\"" + String(autoControlLowTemp) + "°C - " + String(autoControlHighTemp) + "°C\"";
  jsonResponse += "}";
  server.send(200, "application/json", jsonResponse);
}
