// 实验2：呼吸灯 (PWM)
const int ledPin = 2;
const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
}

void loop() {
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(ledPin, dutyCycle);
    delay(12);
  }
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    ledcWrite(ledPin, dutyCycle);
    delay(12);
  }
  Serial.println("Breathing cycle completed");
}
// 呼吸周期约5.1秒
