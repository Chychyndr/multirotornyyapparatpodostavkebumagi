// lib/motor_mx1508/src/motor_mx1508.cpp
#include "motor_mx1508.h"

#ifdef ARDUINO
  #include <Arduino.h>
#else
  // Для native сборки (симулятор) — заглушки, чтобы проект собирался.
  // В симуляторе реальные PWM-пины не нужны.
  static inline void pinMode(uint8_t, uint8_t) {}
  static inline void analogWrite(uint8_t, int) {}
  static inline int constrain(int x, int a, int b) { return (x < a) ? a : (x > b) ? b : x; }
  #define OUTPUT 1
#endif

MotorMX1508::MotorMX1508(uint8_t in1, uint8_t in2) : _in1(in1), _in2(in2) {}

void MotorMX1508::begin() {
  pinMode(_in1, OUTPUT);
  pinMode(_in2, OUTPUT);
  stop();
}

void MotorMX1508::setSpeed(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    analogWrite(_in1, speed);
    analogWrite(_in2, 0);
  } else if (speed < 0) {
    analogWrite(_in1, 0);
    analogWrite(_in2, -speed);
  } else {
    stop();
  }
}

void MotorMX1508::stop() {
  analogWrite(_in1, 0);
  analogWrite(_in2, 0);
}
