#define LED_PIN           2
#define TOUCH_PIN         T0
#define TOUCH_THRESHOLD   400      // 可根据实际读数调整
#define PWM_FREQ          5000
#define PWM_RES           8

// 呼吸速度档位（步进延迟，单位毫秒）
const int gearDelays[3] = {20, 8, 2};

int currentGear = 0;             // 当前档位索引 (0,1,2)
int brightness = 0;              // 当前亮度 (0~255)
int stepDir = 1;                 // 亮度变化方向：1 递增，-1 递减

// 触摸消抖变量
bool lastTouchState = false;
bool touchConfirmed = false;     // 当前是否处于确认触摸状态
unsigned long lastTouchTime = 0;
const int debounceSamples = 3;   // 连续有效采样次数
const int sampleInterval = 10;   // 采样间隔（ms）
int touchValidCount = 0;

// 呼吸定时器
unsigned long lastStepTime = 0;

void setup() {
  Serial.begin(115200);

  // 初始化 LED PWM（新版 API）
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);

  // 可在此打印触摸基线参考值
  int base = touchRead(TOUCH_PIN);
  Serial.print("Touch baseline: "); Serial.println(base);
  Serial.println("Touch to switch gear (1->2->3->1...)");
}

void loop() {
  // ---------- 1. 非阻塞触摸检测（边沿触发 + 软件消抖） ----------
  int raw = touchRead(TOUCH_PIN);
  bool curr = (raw < TOUCH_THRESHOLD);

  // 只在状态变化时才启动消抖计数
  if (curr != lastTouchState) {
    lastTouchState = curr;
    touchValidCount = 0;          // 重置计数
    touchConfirmed = false;
  }

  // 如果当前状态为“触摸”，且未确认，则累加有效计数
  if (curr && !touchConfirmed) {
    // 采样间隔去抖
    if (millis() - lastTouchTime >= sampleInterval) {
      lastTouchTime = millis();
      touchValidCount++;
      if (touchValidCount >= debounceSamples) {
        // 确认为有效触摸上升沿
        touchConfirmed = true;
        // 切换档位
        currentGear = (currentGear + 1) % 3;
        Serial.print("Gear changed to ");
        Serial.println(currentGear + 1);
        // 可选：立即将亮度方向重置为递增，让用户看到速度变化（也可保持原方向）
        // 这里保持方向不变，速度变化会立即体现
      }
    }
  }

  // 如果释放触摸，重置确认标志（为下一次上升沿做准备）
  if (!curr) {
    touchConfirmed = false;
    touchValidCount = 0;
  }

  // ---------- 2. 非阻塞呼吸亮度更新 ----------
  unsigned long now = millis();
  int stepDelay = gearDelays[currentGear];

  if (now - lastStepTime >= stepDelay) {
    lastStepTime = now;

    // 更新亮度
    brightness += stepDir;
    if (brightness >= 255) {
      brightness = 255;
      stepDir = -1;
    } else if (brightness <= 0) {
      brightness = 0;
      stepDir = 1;
    }

    ledcWrite(LED_PIN, brightness);
  }
}