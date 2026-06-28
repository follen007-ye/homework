// 实验4：触摸自锁开关（带防抖）
#define TOUCH_PIN 4
#define LED_PIN   2
#define THRESHOLD 35   // 根据你的板子实测调整（触摸时值变小）

bool ledState = false;          // 当前LED状态
bool lastTouchState = false;    // 上一次触摸状态（true表示触摸中）
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 100; // 防抖时间（毫秒）

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // 读取触摸值，判断当前是否触摸（值小于阈值表示触摸）
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouch = (touchValue < THRESHOLD);

  // 检测到“按下瞬间”：上一次没触摸，现在触摸了
  if (currentTouch && !lastTouchState) {
    // 防抖判断：距离上次触发时间是否大于防抖间隔
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebounceTime > debounceDelay) {
      // 翻转LED状态并执行
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      lastDebounceTime = currentMillis; // 更新触发时间
      
      Serial.print("LED状态: ");
      Serial.println(ledState ? "ON" : "OFF");
    }
  }

  // 更新上一次触摸状态（用于边缘检测）
  lastTouchState = currentTouch;
  
  delay(20); // 适当降低轮询频率
}
// 防抖时间100ms
