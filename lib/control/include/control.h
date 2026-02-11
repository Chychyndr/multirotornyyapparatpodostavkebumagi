// lib/control/include/control.h
#pragma once
#include <stdint.h>

struct ControlConfig {
  // Скорости моторов (0..255)
  int baseSpeed = 140;
  int turnDelta  = 90;   // насколько уменьшаем одну сторону и увеличиваем другую при повороте
  int searchSpeed = 80;  // скорость вращения при поиске линии

  // Универсальность LM393:
  // true  -> датчик выдаёт 1 на линии
  // false -> датчик выдаёт 0 на линии (инверсия)
  bool lineIsHigh = true;

  // Антидребезг: сколько последних чтений учитываем (3 или 5 — норм)
  uint8_t filterWindow = 3;

  // Сколько "тиков" крутиться при потере линии
  uint16_t searchTicks = 40;
};

struct ControlState {
  // -1 = последний поворот влево, +1 = вправо, 0 = прямо
  int8_t lastTurnDir = 0;

  // Сколько тиков ещё крутиться в поиске
  int16_t searching = 0;

  // Фильтр большинства (буфер последних чтений)
  // (Храним до 5, window выбираем 3 или 5)
  uint8_t idx = 0;
  uint8_t histL[5] = {0,0,0,0,0};
  uint8_t histR[5] = {0,0,0,0,0};
};

struct MotorCmd {
  int left = 0;   // -255..255
  int right = 0;  // -255..255
};

// Вход: "сырые" цифровые значения датчиков (0/1) как из digitalRead()
MotorCmd computeLineFollowerDigital(uint8_t rawL, uint8_t rawR,
                                    const ControlConfig& cfg,
                                    ControlState& st);
