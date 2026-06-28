#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "长夜醉今宵的iPhone";
const char* password = "88888888";

#define TOUCH_PIN 4
#define THRESHOLD 30

WebServer server(80);

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>触摸仪表盘</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    #value { font-size: 4em; font-weight: bold; color: #007BFF; }
    #status { font-size: 1.5em; margin-top: 20px; }
  </style>
</head>
<body>
  <h1> 实时触摸传感器</h1>
  <div id="value">--</div>
  <div id="status">未触摸</div>
  <script>
    function fetchData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('value').innerText = data.value;
          const status = data.touched ? '触摸中' : '未触摸';
          document.getElementById('status').innerText = status;
        })
        .catch(err => console.error(err));
    }
    setInterval(fetchData, 200);
    fetchData();
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  int val = touchRead(TOUCH_PIN);
  bool touched = (val < THRESHOLD);
  String json = "{\"value\":" + String(val) + ",\"touched\":" + (touched ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() { server.handleClient(); }
// 刷新间隔改为200ms
