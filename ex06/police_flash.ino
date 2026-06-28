// 实验6：警车双闪灯效（双通道PWM反相渐变）
#define LED1_PIN 5   // 请使用未占用的GPIO
#define LED2_PIN 18  // 请使用未占用的GPIO

const int freq = 5000;
const int resolution = 8;

void setup() {
  // 绑定两个PWM通道（ESP32的LEDC可自动分配通道）
  ledcAttach(LED1_PIN, freq, resolution);
  ledcAttach(LED2_PIN, freq, resolution);
}

void loop() {
  // 从0到255渐增
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED1_PIN, duty);        // LED1 逐渐变亮
    ledcWrite(LED2_PIN, 255 - duty);  // LED2 逐渐变暗
    delay(8);
  }
  // 从255到0渐减
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED1_PIN, duty);        // LED1 逐渐变暗
    ledcWrite(LED2_PIN, 255 - duty);  // LED2 逐渐变亮
    delay(8);
  }
}
