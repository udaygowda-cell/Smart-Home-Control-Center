#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "udaykumar";
const char* password = "12345678";

WebServer server(80);

// Device pins
const int kitchen_light = 2;
const int hall_light = 4;
const int hall_fan = 5;
const int reading_light = 15;

// State variables
bool kitchen_light_state = false;
bool hall_light_state = false;
bool hall_fan_state = false;
bool reading_light_state = false;

// Function declarations
void handleRoot();
void handleControl();
void handleVoiceCommand();
void handleStatus();
void handleTest();
void handleNotFound();
String processControlCommand(String device, String action);
String processVoiceInput(String command);
void setDevice(int pin, bool on);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("🚀 Starting Voice Controlled Home Automation...");
  
  // Initialize pins
  pinMode(kitchen_light, OUTPUT);
  pinMode(hall_light, OUTPUT);
  pinMode(hall_fan, OUTPUT);
  pinMode(reading_light, OUTPUT);

  // Turn off all devices initially
  digitalWrite(kitchen_light, LOW);
  digitalWrite(hall_light, LOW);
  digitalWrite(hall_fan, LOW);
  digitalWrite(reading_light, LOW);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("📶 Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi Connected!");
    Serial.print("📱 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("❌ WiFi Failed!");
    return;
  }

  // Server routes
  server.on("/", handleRoot);
  server.on("/control", handleControl);
  server.on("/voice", handleVoiceCommand);
  server.on("/status", handleStatus);
  server.on("/test", handleTest);
  
  // Handle CORS
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("✅ HTTP Server Started!");
  Serial.println("🎤 Voice Control Ready!");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Voice Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; margin: 20px; background: #f5f5f5; }
        .container { max-width: 500px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
        .btn { padding: 10px 15px; margin: 5px; background: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }
        .status { padding: 10px; margin: 10px 0; border-radius: 5px; }
        .on { background: #d4edda; color: #155724; }
        .off { background: #f8d7da; color: #721c24; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🏠 Voice Control Server</h1>
        <p>IP: )rawliteral" + WiFi.localIP().toString() + R"rawliteral(</p>
        <p>Use the web interface to control devices.</p>
        
        <button class="btn" onclick="control('kitchen_light', 'on')">Kitchen Light ON</button>
        <button class="btn" onclick="control('kitchen_light', 'off')">Kitchen Light OFF</button>
        <br>
        <button class="btn" onclick="control('hall_light', 'on')">Hall Light ON</button>
        <button class="btn" onclick="control('hall_light', 'off')">Hall Light OFF</button>
        <br>
        <button class="btn" onclick="control('hall_fan', 'on')">Hall Fan ON</button>
        <button class="btn" onclick="control('hall_fan', 'off')">Hall Fan OFF</button>
        <br>
        <button class="btn" onclick="control('reading_light', 'on')">Reading Light ON</button>
        <button class="btn" onclick="control('reading_light', 'off')">Reading Light OFF</button>
        <br>
        <button class="btn" onclick="control('all', 'on')">All ON</button>
        <button class="btn" onclick="control('all', 'off')">All OFF</button>
        
        <div id="status">Ready...</div>
    </div>

    <script>
        function control(device, action) {
            fetch('/control?device=' + device + '&action=' + action)
                .then(r => r.json())
                .then(data => {
                    document.getElementById('status').innerHTML = data.message;
                });
        }
    </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleControl() {
  // Enable CORS
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  String device = "";
  String action = "";
  
  if (server.hasArg("device")) device = server.arg("device");
  if (server.hasArg("action")) action = server.arg("action");

  device.toLowerCase();
  action.toLowerCase();

  Serial.println("🔧 Control: " + device + " -> " + action);

  String message = processControlCommand(device, action);
  String response = "{\"status\":\"success\",\"message\":\"" + message + "\"}";
  
  server.send(200, "application/json", response);
}

String processControlCommand(String device, String action) {
  if (device == "kitchen_light" || device == "l1") {
    if (action == "on") { 
      setDevice(kitchen_light, true); 
      kitchen_light_state = true; 
      return "Kitchen light ON"; 
    }
    else if (action == "off") { 
      setDevice(kitchen_light, false); 
      kitchen_light_state = false; 
      return "Kitchen light OFF"; 
    }
    else if (action == "toggle") { 
      kitchen_light_state = !kitchen_light_state; 
      setDevice(kitchen_light, kitchen_light_state); 
      return "Kitchen light toggled " + String(kitchen_light_state ? "ON" : "OFF"); 
    }
  } 
  else if (device == "hall_light" || device == "l2") {
    if (action == "on") { 
      setDevice(hall_light, true); 
      hall_light_state = true; 
      return "Hall light ON"; 
    }
    else if (action == "off") { 
      setDevice(hall_light, false); 
      hall_light_state = false; 
      return "Hall light OFF"; 
    }
    else if (action == "toggle") { 
      hall_light_state = !hall_light_state; 
      setDevice(hall_light, hall_light_state); 
      return "Hall light toggled " + String(hall_light_state ? "ON" : "OFF"); 
    }
  } 
  else if (device == "hall_fan" || device == "f1") {
    if (action == "on") { 
      setDevice(hall_fan, true); 
      hall_fan_state = true; 
      return "Hall fan ON"; 
    }
    else if (action == "off") { 
      setDevice(hall_fan, false); 
      hall_fan_state = false; 
      return "Hall fan OFF"; 
    }
    else if (action == "toggle") { 
      hall_fan_state = !hall_fan_state; 
      setDevice(hall_fan, hall_fan_state); 
      return "Hall fan toggled " + String(hall_fan_state ? "ON" : "OFF"); 
    }
  } 
  else if (device == "reading_light" || device == "f2") {
    if (action == "on") { 
      setDevice(reading_light, true); 
      reading_light_state = true; 
      return "Reading light ON"; 
    }
    else if (action == "off") { 
      setDevice(reading_light, false); 
      reading_light_state = false; 
      return "Reading light OFF"; 
    }
    else if (action == "toggle") { 
      reading_light_state = !reading_light_state; 
      setDevice(reading_light, reading_light_state); 
      return "Reading light toggled " + String(reading_light_state ? "ON" : "OFF"); 
    }
  } 
  else if (device == "all") {
    if (action == "on") {
      setDevice(kitchen_light, true); kitchen_light_state = true;
      setDevice(hall_light, true); hall_light_state = true;
      setDevice(hall_fan, true); hall_fan_state = true;
      setDevice(reading_light, true); reading_light_state = true;
      return "All devices ON";
    }
    else if (action == "off") {
      setDevice(kitchen_light, false); kitchen_light_state = false;
      setDevice(hall_light, false); hall_light_state = false;
      setDevice(hall_fan, false); hall_fan_state = false;
      setDevice(reading_light, false); reading_light_state = false;
      return "All devices OFF";
    }
  }

  return "Unknown command: " + device + " " + action;
}

void handleVoiceCommand() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  if (server.hasArg("command")) {
    String command = server.arg("command");
    command.toLowerCase();
    
    Serial.println("🎤 Voice: " + command);

    String response = processVoiceInput(command);
    server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"" + response + "\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No command\"}");
  }
}

String processVoiceInput(String command) {
  if (command.indexOf("turn on kitchen light") >= 0 || command.indexOf("kitchen light on") >= 0 || command.indexOf("kitchen on") >= 0) {
    setDevice(kitchen_light, true);
    kitchen_light_state = true;
    return "Kitchen light turned on";
  }
  else if (command.indexOf("turn off kitchen light") >= 0 || command.indexOf("kitchen light off") >= 0 || command.indexOf("kitchen off") >= 0) {
    setDevice(kitchen_light, false);
    kitchen_light_state = false;
    return "Kitchen light turned off";
  }
  else if (command.indexOf("turn on hall light") >= 0 || command.indexOf("hall light on") >= 0 || command.indexOf("hall on") >= 0) {
    setDevice(hall_light, true);
    hall_light_state = true;
    return "Hall light turned on";
  }
  else if (command.indexOf("turn off hall light") >= 0 || command.indexOf("hall light off") >= 0 || command.indexOf("hall off") >= 0) {
    setDevice(hall_light, false);
    hall_light_state = false;
    return "Hall light turned off";
  }
  else if (command.indexOf("turn on hall fan") >= 0 || command.indexOf("hall fan on") >= 0 || command.indexOf("fan on") >= 0) {
    setDevice(hall_fan, true);
    hall_fan_state = true;
    return "Hall fan turned on";
  }
  else if (command.indexOf("turn off hall fan") >= 0 || command.indexOf("hall fan off") >= 0 || command.indexOf("fan off") >= 0) {
    setDevice(hall_fan, false);
    hall_fan_state = false;
    return "Hall fan turned off";
  }
  else if (command.indexOf("turn on reading light") >= 0 || command.indexOf("reading light on") >= 0 || command.indexOf("reading on") >= 0) {
    setDevice(reading_light, true);
    reading_light_state = true;
    return "Reading light turned on";
  }
  else if (command.indexOf("turn off reading light") >= 0 || command.indexOf("reading light off") >= 0 || command.indexOf("reading off") >= 0) {
    setDevice(reading_light, false);
    reading_light_state = false;
    return "Reading light turned off";
  }
  else if (command.indexOf("all on") >= 0 || command.indexOf("turn on all") >= 0 || command.indexOf("everything on") >= 0) {
    setDevice(kitchen_light, true); kitchen_light_state = true;
    setDevice(hall_light, true); hall_light_state = true;
    setDevice(hall_fan, true); hall_fan_state = true;
    setDevice(reading_light, true); reading_light_state = true;
    return "All devices turned on";
  }
  else if (command.indexOf("all off") >= 0 || command.indexOf("turn off all") >= 0 || command.indexOf("everything off") >= 0) {
    setDevice(kitchen_light, false); kitchen_light_state = false;
    setDevice(hall_light, false); hall_light_state = false;
    setDevice(hall_fan, false); hall_fan_state = false;
    setDevice(reading_light, false); reading_light_state = false;
    return "All devices turned off";
  }
  else if (command.indexOf("status") >= 0 || command.indexOf("what's on") >= 0) {
    return "Status: Kitchen " + String(kitchen_light_state ? "ON" : "OFF") +
           ", Hall Light " + (hall_light_state ? "ON" : "OFF") +
           ", Hall Fan " + (hall_fan_state ? "ON" : "OFF") +
           ", Reading " + (reading_light_state ? "ON" : "OFF");
  }
  else {
    return "Command not recognized. Try: 'turn on kitchen light', 'all off', etc.";
  }
}

void handleStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  String json = "{";
  json += "\"kitchen_light\":" + String(kitchen_light_state ? "true" : "false") + ",";
  json += "\"hall_light\":" + String(hall_light_state ? "true" : "false") + ",";
  json += "\"hall_fan\":" + String(hall_fan_state ? "true" : "false") + ",";
  json += "\"reading_light\":" + String(reading_light_state ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleTest() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"ESP32 Voice Control Server is running!\"}");
}

void handleNotFound() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(204);
  } else {
    server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Endpoint not found\"}");
  }
}

void setDevice(int pin, bool on) {
  digitalWrite(pin, on ? HIGH : LOW);
  Serial.println("⚡ Pin " + String(pin) + " -> " + (on ? "HIGH" : "LOW"));
}