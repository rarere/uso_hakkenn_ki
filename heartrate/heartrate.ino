#include <Wire.h>
#include <ST7032.h>

int ledPin = 13; // LED表示用
volatile int hr_cnt = 0; // 外部割込みカウンタ
int time_cnt = 150; // 時間経過用カウンタ
int hr; // 心拍数
float hr_lpf = 70.0; // 心拍数のLPF

ST7032 lcd;

void setup() {
  lcd.begin(16, 2);
  lcd.setContrast(40);
  lcd.print("HR:");
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(3, hrcntup, FALLING); // 外部割込み立下り。1番ピン。
}

void loop() {
  // 時間計測用カウンタを加算
  time_cnt++;  

  if (hr_cnt >= 1) {
    // 外部割込み停止、LED点灯
    detachInterrupt(1);
    digitalWrite(ledPin, HIGH);

    // time_cntが24～300(心拍数が20以上250以下)の間の時だけ処理する。
    // 250より上はチャタリング対策
    // 20未満は生きてる？
    if ((24 <= time_cnt) && (time_cnt <= 300)) {
      // 前回の割り込みから今回の割り込みまでの間に
      // 10msカウンタが何回まわったか、ということから心拍数を計算。
      hr = 60000 / (time_cnt * 10);

      lcd.clear();

      // 急に心拍数が上がったらうそつき表示。
      // LPFは計算しないで元のまま
      // そうじゃないならLPFを計算して保存。
      if ((hr - hr_lpf) > 30) {
        lcd.setCursor(0, 1);
        lcd.print("USO!!!");
      } else {
        hr_lpf = float(hr) * 0.1 + hr_lpf * 0.9; // なんとなくLPF
      }

      // LCD表示
      lcd.setCursor(0, 0);
      lcd.print("HR: ");
      lcd.print(hr);
      lcd.print(", ");
      lcd.print(int(hr_lpf));

      // シリアル出力
      Serial.print(hr);
      Serial.print(",");
      Serial.println(int(hr_lpf));
    }

    // 各カウンタのクリア、LED消灯、外部割り込み開始
    hr_cnt = 0;
    time_cnt = 0;
    digitalWrite(ledPin, LOW);
    attachInterrupt(1, hrcntup, FALLING);
  }
  
  delay(10);
}

void hrcntup() {
  hr_cnt++;
}


