// AP热点模式 - Web端LED无级调光器
#include <WiFi.h>
#include <WebServer.h>

#define LED_PWM_PIN   2
#define PWM_BASE_FREQ 5000
#define PWM_RES_BITS  8

const char* apSSID = "ESP2024117004";
const char* apPassword = "12345678";

WebServer webServer(80);

// ========== 完整的 HTML 页面 ==========
const char* homeHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <title>LED亮度调节控制台</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      background: linear-gradient(135deg, #e0f7fa 0%, #b2ebf2 100%);
      font-family: "Microsoft Yahei", sans-serif;
    }
    .card {
      background: white;
      padding: 50px 60px;
      border-radius: 20px;
      box-shadow: 0 8px 24px rgba(0, 150, 180, 0.15);
      text-align: center;
      width: 90%;
      max-width: 420px;
    }
    h2 { color: #00838f; margin-bottom: 30px; font-size: 28px; }
    .bright-show {
      font-size: 32px;
      font-weight: bold;
      color: #00acc1;
      margin: 25px 0;
    }
    #slider {
      width: 100%;
      height: 8px;
      border-radius: 4px;
      background: #e0f2f1;
      outline: none;
      -webkit-appearance: none;
    }
    #slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 22px;
      height: 22px;
      border-radius: 50%;
      background: #00acc1;
      cursor: pointer;
      box-shadow: 0 2px 6px rgba(0, 172, 193, 0.4);
    }
  </style>
</head>
<body>
  <div class="card">
    <h2>LED亮度无级调节</h2>
    <div class="bright-show">当前亮度：<span id="value">0</span></div>
    <input type="range" id="slider" min="0" max="255" value="0">
  </div>
  <script>
    const slider = document.getElementById('slider');
    const valueText = document.getElementById('value');
    slider.addEventListener('input', () => {
      const bright = slider.value;
      valueText.textContent = bright;
      fetch('/set?bright=' + bright).catch(err => console.error(err));
    });
  </script>
</body>
</html>
)HTML";

void handleHomePage() {
  webServer.send(200, "text/html", homeHtml);
}

void handleBrightnessSet() {
  if (webServer.hasArg("bright")) {
    int brightVal = webServer.arg("bright").toInt();
    brightVal = constrain(brightVal, 0, 255);
    ledcWrite(LED_PWM_PIN, brightVal);   // 直接使用引脚号
    webServer.send(200, "text/plain", "亮度已设置为: " + String(brightVal));
  } else {
    webServer.send(400, "text/plain", "请求参数有误");
  }
}

void setup() {
  Serial.begin(115200);

  // ✅ 正确：ledcAttach(引脚, 频率, 分辨率)
  ledcAttach(LED_PWM_PIN, PWM_BASE_FREQ, PWM_RES_BITS);
  ledcWrite(LED_PWM_PIN, 0);   // 初始熄灭

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);
  Serial.println("ESP32 AP热点已启动");
  Serial.print("控制台地址: http://");
  Serial.println(WiFi.softAPIP());

  webServer.on("/", handleHomePage);
  webServer.on("/set", handleBrightnessSet);
  webServer.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  webServer.handleClient();
}