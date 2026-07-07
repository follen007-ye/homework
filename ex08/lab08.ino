// ============================================================
// 物联网安防报警器（AP模式 + Web控制 + 触摸触发自锁报警）
#include <WiFi.h>
#include <WebServer.h>

// ========== 硬件引脚定义 ==========
#define LED_PIN          2    // 报警指示灯（GPIO2）
#define TOUCH_PIN        T0   // 触摸输入（GPIO4）

// ========== 触摸阈值（请根据实际读数调整） ==========
#define TOUCH_THRESHOLD  400  

// ========== AP热点配置 ==========
const char* apSSID = "ESP2024117004";
const char* apPassword = "12345678";

WebServer server(80);

// ========== 全局状态变量 ==========
bool isArmed = false;        // 是否布防
bool isAlarming = false;     // 是否处于报警状态（自锁）
unsigned long lastBlinkTime = 0;
const int BLINK_INTERVAL = 100; // 闪烁间隔（ms）
bool ledState = false;

// ========== HTML页面（内嵌CSS/JS，适配移动端） ==========
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>安防报警系统</title>
  <style>
    * { margin:0; padding:0; box-sizing:border-box; }
    body {
      font-family: 'Microsoft YaHei', sans-serif;
      background: #f0f2f5;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
    }
    .container {
      background: white;
      padding: 40px 50px;
      border-radius: 20px;
      box-shadow: 0 8px 30px rgba(0,0,0,0.12);
      text-align: center;
      max-width: 420px;
      width: 90%;
    }
    h1 { color: #1a1a2e; margin-bottom: 10px; }
    .sub { color: #888; font-size: 14px; margin-bottom: 30px; }
    .status-box {
      background: #f8f9fa;
      padding: 20px;
      border-radius: 12px;
      margin-bottom: 30px;
      font-size: 20px;
      font-weight: 600;
      transition: 0.3s;
    }
    .status-box.armed { background: #ff4757; color: white; }
    .status-box.disarmed { background: #2ed573; color: white; }
    .status-box.alarm { background: #ff6b81; color: white; animation: pulse 0.5s infinite alternate; }
    @keyframes pulse {
      from { opacity: 1; }
      to { opacity: 0.6; }
    }
    .btn-group {
      display: flex;
      gap: 15px;
      justify-content: center;
      flex-wrap: wrap;
    }
    .btn {
      padding: 14px 40px;
      border: none;
      border-radius: 50px;
      font-size: 18px;
      font-weight: 600;
      color: white;
      cursor: pointer;
      transition: 0.2s;
      flex: 1;
      min-width: 120px;
    }
    .btn-arm { background: #ff4757; }
    .btn-arm:hover { background: #ff6b81; }
    .btn-disarm { background: #2ed573; }
    .btn-disarm:hover { background: #7bed9f; }
    .btn:disabled { opacity: 0.5; cursor: not-allowed; }
    .footer { margin-top: 30px; font-size: 12px; color: #aaa; }
  </style>
</head>
<body>
  <div class="container">
    <h1>🔐 安防报警器</h1>
    <div class="sub">ESP32 智能主机</div>
    <div id="statusDisplay" class="status-box disarmed">🔓 未布防</div>
    <div class="btn-group">
      <button class="btn btn-arm" id="armBtn" onclick="sendCmd('arm')">🔒 布防</button>
      <button class="btn btn-disarm" id="disarmBtn" onclick="sendCmd('disarm')">🔓 撤防</button>
    </div>
    <div class="footer">状态每 500ms 自动刷新</div>
  </div>
  <script>
    function sendCmd(cmd) {
      fetch('/' + cmd)
        .then(response => response.text())
        .then(data => {
          console.log(data);
          updateStatus(); // 立即刷新状态
        })
        .catch(err => console.error('Error:', err));
    }

    function updateStatus() {
      fetch('/status')
        .then(res => res.json())
        .then(data => {
          const box = document.getElementById('statusDisplay');
          box.textContent = data.text;
          box.className = 'status-box ' + data.cssClass;
        })
        .catch(err => console.error('Status error:', err));
    }

    // 每500ms自动刷新状态
    setInterval(updateStatus, 500);
    // 首次加载立即刷新
    window.onload = updateStatus;
  </script>
</body>
</html>
)rawliteral";

// ========== Web请求处理函数 ==========
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

void handleStatus() {
  String text, cssClass;
  if (isAlarming) {
    text = "🚨 警报已触发！";
    cssClass = "alarm";
  } else if (isArmed) {
    text = "🔒 已布防 (监控中)";
    cssClass = "armed";
  } else {
    text = "🔓 未布防 (安全)";
    cssClass = "disarmed";
  }
  String json = "{\"text\":\"" + text + "\",\"cssClass\":\"" + cssClass + "\"}";
  server.send(200, "application/json", json);
}

void handleArm() {
  isArmed = true;
  isAlarming = false;        // 布防时清除之前的报警状态
  digitalWrite(LED_PIN, LOW);
  Serial.println("系统已布防");
  server.send(200, "text/plain", "Armed");
}

void handleDisarm() {
  isArmed = false;
  isAlarming = false;
  digitalWrite(LED_PIN, LOW);
  Serial.println("系统已撤防");
  server.send(200, "text/plain", "Disarmed");
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 启动AP热点
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);
  Serial.println("AP热点已启动");
  Serial.print("请连接 Wi-Fi: ");
  Serial.println(apSSID);
  Serial.print("访问地址: http://");
  Serial.println(WiFi.softAPIP());

  // 配置Web服务器路由
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
  Serial.println("Web服务器已启动");
}

// ========== Loop ==========
void loop() {
  server.handleClient();

  // ----- 触摸检测（仅在布防且未报警时检测） -----
  if (isArmed && !isAlarming) {
    int touchVal = touchRead(TOUCH_PIN);
    // 调试：可取消下行注释查看触摸值（但会频繁输出）
    // Serial.printf("Touch: %d\n", touchVal);
    if (touchVal < TOUCH_THRESHOLD) {
      isAlarming = true;
      Serial.println("⚠️ 触发报警！LED闪烁");
    }
  }

  // ----- 报警状态：LED高频闪烁（自锁） -----
  if (isAlarming) {
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  } else {
    // 非报警状态保证LED熄灭
    digitalWrite(LED_PIN, LOW);
  }
}