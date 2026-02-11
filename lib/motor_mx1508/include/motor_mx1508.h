// lib/motor_mx1508/include/motor_mx1508.h
#pragma once
#include <stdint.h>

class MotorMX1508 {
public:
  // in1/in2 должны быть PWM-пинами (на Uno: 3,5,6,9,10,11)
  MotorMX1508(uint8_t in1, uint8_t in2);

  void begin();

  // speed: -255..255 (отрицательное = назад)
  void setSpeed(int speed);

  void stop();

private:
  uint8_t _in1, _in2;
};
