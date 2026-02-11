// src/main.cpp
#include <Arduino.h>
#include <control.h>
#include <motor_mx1508.h>

// ===== ДАТЧИКИ LM393 (DO) =====
const uint8_t PIN_SENS_L = A0; // цифровой вход
const uint8_t PIN_SENS_R = A1; // цифровой вход

// ===== MX1508 (2 PWM на мотор) =====
// Левый мотор
const uint8_t PIN_ML_IN1 = 5;   // PWM
const uint8_t PIN_ML_IN2 = 6;   // PWM
// Правый мотор
const uint8_t PIN_MR_IN1 = 9;   // PWM
const uint8_t PIN_MR_IN2 = 10;  // PWM

MotorMX1508 motorL(PIN_ML_IN1, PIN_ML_IN2);
MotorMX1508 motorR(PIN_MR_IN1, PIN_MR_IN2);

ControlConfig cfg;
ControlState st;

void setup() {
  Serial.begin(115200);

  // LM393 обычно имеет “push-pull” выход, но иногда бывает шум/помехи.
  // С INPUT обычно ок. Если увидишь хаос в Serial — поменяем на INPUT_PULLUP и инвертируем lineIsHigh.
  pinMode(PIN_SENS_L, INPUT);
  pinMode(PIN_SENS_R, INPUT);

  motorL.begin();
  motorR.begin();

  // ===== НАСТРОЙКИ (универсальные) =====
  cfg.lineIsHigh = true;     // если окажется наоборот — поменяешь на false
  cfg.baseSpeed = 140;
  cfg.turnDelta = 90;
  cfg.searchSpeed = 80;
  cfg.filterWindow = 3;
  cfg.searchTicks = 40;

  Serial.println(F("Ready: LM393 + MX1508"));
}

void loop() {
  // Сырой сигнал DO (0/1)
  uint8_t rawL = (uint8_t)digitalRead(PIN_SENS_L);
  uint8_t rawR = (uint8_t)digitalRead(PIN_SENS_R);

  MotorCmd cmd = computeLineFollowerDigital(rawL, rawR, cfg, st);

  motorL.setSpeed(cmd.left);
  motorR.setSpeed(cmd.right);

  static unsigned long t = 0;
  if (millis() - t > 200) {
    t = millis();
    Serial.print(F("rawL/rawR="));
    Serial.print(rawL); Serial.print('/');
    Serial.print(rawR);
    Serial.print(F("  motors="));
    Serial.print(cmd.left); Serial.print('/');
    Serial.println(cmd.right);
  }

  delay(5);
}
