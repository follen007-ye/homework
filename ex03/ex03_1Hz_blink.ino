// 实验3-1：使用 millis() 实现1Hz稳定闪烁（非阻塞）
const int ledPin = 2;
const unsigned long interval = 500;  // 500ms亮 → 500ms灭 → 周期1秒
unsigned long previousMillis = 0;
int ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}
