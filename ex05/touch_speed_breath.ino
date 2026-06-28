// 实验5：多档位触摸调速呼吸灯
#define TOUCH_PIN 4
#define LED_PIN   2
#define THRESHOLD 30   // 根据实际调整

const int freq = 5000;
const int resolution = 8;

int speedLevel = 1;            // 当前档位 1,2,3
bool lastTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 100;

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {
  // 检测触摸切换档位
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouch = (touchValue < THRESHOLD);
  
  if (currentTouch && !lastTouchState) {
    if (millis() - lastDebounceTime > debounceDelay) {
      // 切换档位（循环 1->2->3->1）
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;
      Serial.print("当前档位: ");
      Serial.println(speedLevel);
      lastDebounceTime = millis();
    }
  }
  lastTouchState = currentTouch;

  // 呼吸灯循环（根据档位调整速度和步长）
  int step = 1;          // 默认步长
  int delayTime = 10;    // 默认延迟(ms)
  
  switch(speedLevel) {
    case 1: step = 1; delayTime = 10; break;   // 慢速
    case 2: step = 2; delayTime = 8;  break;   // 中速
    case 3: step = 4; delayTime = 5;  break;   // 快速
  }

  // 渐亮
  for (int duty = 0; duty <= 255; duty += step) {
    ledcWrite(LED_PIN, duty);
    delay(delayTime);
    // 在呼吸过程中也要响应触摸（非阻塞检测，但这里用了delay，所以为了响应触摸，可改用millis，但为简单，我们允许呼吸期间暂不响应，用户可以在呼吸结束后切换，实际也可以，但演示效果足够）
    // 为了更实时，可以在这里插入触摸检测，但为了代码清晰，本次实验允许在呼吸周期结束后响应切换。
  }
  // 渐暗
  for (int duty = 255; duty >= 0; duty -= step) {
    ledcWrite(LED_PIN, duty);
    delay(delayTime);
  }
}
