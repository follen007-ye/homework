#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "长夜醉今宵的iPhone";
const char* password = "88888888";

#define TOUCH_PIN 4
#define LED_PIN   2
#define THRESHOLD 30

WebServer server(80);
bool armed = false;
bool alarmTriggered = false;
unsigned long lastBlinkTime = 0;
bool ledState = false;

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>安防报警器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    button { font-size: 1.5em; padding: 10px 20px; margin: 10px; }
    #status { font-size: 1.5em; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>安防报警系统</h1>
  <div>
    <button onclick="arm()"> 布防</button>
    <button onclick="disarm()"> 撤防</button>
  </div>
  <div id="status">系统状态: <span id="state">撤防</span></div>
  <script>
    function arm() {
      fetch('/arm').then(() => document.getElementById('state').innerText = '布防中');
    }
    function disarm() {
      fetch('/disarm').then(() => document.getElementById('state').innerText = '撤防');
    }
    setInterval(() => {
      fetch('/status').then(r => r.text()).then(t => {
        document.getElementById('state').innerText = t;
      });
    }, 1000);
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleArm() {
  armed = true;
  alarmTriggered = false;
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "Armed");
}
void handleDisarm() {
  armed = false;
  alarmTriggered = false;
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "Disarmed");
}
void handleStatus() {
  if (alarmTriggered) server.send(200, "text/plain", "报警中！");
  else if (armed) server.send(200, "text/plain", "布防中");
  else server.send(200, "text/plain", "撤防");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  int touchValue = touchRead(TOUCH_PIN);
  bool touched = (touchValue < THRESHOLD);
  if (armed && touched && !alarmTriggered) {
    alarmTriggered = true;
    Serial.println("ALARM TRIGGERED!");
  }
  if (alarmTriggered) {
    unsigned long now = millis();
    if (now - lastBlinkTime >= 100) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
}
// 调整触摸阈值，提高灵敏度
