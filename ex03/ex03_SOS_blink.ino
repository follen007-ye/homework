// 实验3-2：使用 millis() 产生SOS信号（短闪·长闪·短闪 + 长停顿）
const int ledPin = 2;

const unsigned long dotDuration = 300;    // 短闪
const unsigned long dashDuration = 900;   // 长闪
const unsigned long symbolGap = 300;      // 闪与闪间隔
const unsigned long wordGap = 3000;       // 完整SOS后停顿

// 动作序列：1=短闪亮，2=长闪亮，0=间隔(灭)
const int sosPattern[] = {
  1, 0, 1, 0, 1, 0,      // S
  2, 0, 2, 0, 2, 0,      // O
  1, 0, 1, 0, 1, 0,      // S
  0                      // 结束标记
};

unsigned long previousMillis = 0;
int patternIndex = 0;
bool ledOn = false;
bool isPause = false;
unsigned long pauseStartTime = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  if (isPause) {
    if (currentMillis - pauseStartTime >= wordGap) {
      isPause = false;
      patternIndex = 0;
      ledOn = false;
      digitalWrite(ledPin, LOW);
      previousMillis = currentMillis;
    }
    return;
  }

  int action = sosPattern[patternIndex];

  if (action == 0 && patternIndex > 0) {
    isPause = true;
    pauseStartTime = currentMillis;
    digitalWrite(ledPin, LOW);
    return;
  }

  unsigned long duration;
  if (action == 1) duration = dotDuration;
  else if (action == 2) duration = dashDuration;
  else duration = symbolGap;   // action == 0

  if (currentMillis - previousMillis >= duration) {
    previousMillis = currentMillis;

    if (action == 1 || action == 2) {
      ledOn = !ledOn;
      digitalWrite(ledPin, ledOn ? HIGH : LOW);
      if (!ledOn) patternIndex++;
    } else {
      patternIndex++;  // 间隔结束，进入下一个
    }
  }
}
// SOS信号：短闪3次，长闪3次，短闪3次，然后长停顿
