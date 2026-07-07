// AP模式 - 触摸传感器实时监测仪表盘
#include <WiFi.h>
#include <WebServer.h>

#define TOUCH_SENSOR_PIN T0

const char* apName = "ESP2024117004";
const char* apPwd = "12345678";

WebServer webServer(80);

const char* dashboardHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <title>触摸感应监测面板</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      background: #f0f5ff;
      font-family: "Microsoft Yahei", sans-serif;
    }
    .monitor-panel {
      background: white;
      padding: 55px 70px;
      border-radius: 20px;
      box-shadow: 0 6px 20px rgba(24, 144, 255, 0.1);
      text-align: center;
    }
    .title-tip {
      font-size: 20px;
      color: #606266;
      margin-bottom: 20px;
    }
    .num-display {
      font-size: 72px;
      font-weight: bold;
      color: #1890ff;
      margin: 25px 0;
      font-family: "Consolas", monospace;
    }
    .foot-tip {
      font-size: 16px;
      color: #909399;
      margin-top: 15px;
    }
  </style>
</head>
<body>
  <div class="monitor-panel">
    <div class="title-tip">触摸感应实时数值</div>
    <div class="num-display" id="value">--</div>
    <div class="foot-tip">提示：手指贴近感应区时数值会减小</div>
  </div>
  <script>
    const valueElement = document.getElementById('value');
    function fetchValue() {
      fetch('/getTouch')
        .then(res => res.text())
        .then(num => { valueElement.textContent = num; })
        .finally(() => { setTimeout(fetchValue, 100); });
    }
    fetchValue();
  </script>
</body>
</html>
)HTML";

void handleDashboard() {
  webServer.send(200, "text/html", dashboardHtml);
}

void handleTouchRead() {
  int touchVal = touchRead(TOUCH_SENSOR_PIN);
  webServer.send(200, "text/plain", String(touchVal));
}

void setup() {
  Serial.begin(115200);

  // 开启AP热点
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apName, apPwd);
  Serial.println("ESP32监测热点已启动");
  Serial.print("热点名称: ");
  Serial.println(apName);
  Serial.print("仪表盘地址: http://");
  Serial.println(WiFi.softAPIP());

  webServer.on("/", handleDashboard);
  webServer.on("/getTouch", handleTouchRead);
  webServer.begin();
}

void loop() {
  webServer.handleClient();
}