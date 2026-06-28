#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "长夜醉今宵的iPhone";
const char* password = "88888888";

#define LED_PIN 2
#define FREQ 5000
#define RESOLUTION 8

WebServer server(80);

void setupPWM() { ledcAttach(LED_PIN, FREQ, RESOLUTION); ledcWrite(LED_PIN, 0); }

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 调光器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    input[type=range] { width: 80%; max-width: 400px; }
    #value { font-size: 2em; font-weight: bold; }
  </style>
</head>
<body>
  <h1>LED 亮度控制</h1>
  <input type="range" min="0" max="255" value="0" id="slider" oninput="updateSlider(this.value)">
  <p>当前亮度: <span id="value">0</span></p>
  <script>
    function updateSlider(val) {
      document.getElementById('value').innerText = val;
      fetch('/set?value=' + val)
        .then(response => console.log('OK'))
        .catch(err => console.error(err));
    }
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("value")) {
    int val = server.arg("value").toInt();
    if (val >= 0 && val <= 255) {
      ledcWrite(LED_PIN, val);
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid value");
}
void setup() {
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  server.on("/", handleRoot);
  server.on("/set", handleSet);

void loop() { server.handleClient(); }  server.begin();
  Serial.println("HTTP started");
}
  Serial.begin(115200);
  setupPWM();
  WiFi.begin(ssid, password);

// 完成所有Web实验（调光器、安防报警、仪表盘）
