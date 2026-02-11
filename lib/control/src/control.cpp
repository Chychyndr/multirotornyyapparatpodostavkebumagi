// lib/control/src/control.cpp
#include "control.h"

static int clampi(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static uint8_t toLine(uint8_t raw, bool lineIsHigh) {
  // Приводим "сырое" чтение (0/1) к смыслу "на линии" (0/1)
  // Если lineIsHigh=true  : raw==1 -> на линии
  // Если lineIsHigh=false : raw==0 -> на линии
  raw = raw ? 1 : 0;
  return lineIsHigh ? raw : (uint8_t)(1 - raw);
}

static uint8_t majority(const uint8_t* buf, uint8_t n) {
  // n = 3 или 5
  uint8_t ones = 0;
  for (uint8_t i = 0; i < n; i++) ones += (buf[i] ? 1 : 0);
  // большинство
  return (ones > (n / 2)) ? 1 : 0;
}

MotorCmd computeLineFollowerDigital(uint8_t rawL, uint8_t rawR,
                                    const ControlConfig& cfg,
                                    ControlState& st) {
  MotorCmd out;

  // Ограничим окно фильтра (разрешаем 1/3/5)
  uint8_t w = cfg.filterWindow;
  if (w != 1 && w != 3 && w != 5) w = 3;

  // Сохраняем историю
  st.histL[st.idx] = rawL ? 1 : 0;
  st.histR[st.idx] = rawR ? 1 : 0;
  st.idx = (uint8_t)((st.idx + 1) % w);

  // Фильтруем
  uint8_t fL = (w == 1) ? st.histL[0] : majority(st.histL, w);
  uint8_t fR = (w == 1) ? st.histR[0] : majority(st.histR, w);

  // Приводим к "на линии"
  uint8_t onL = toLine(fL, cfg.lineIsHigh);
  uint8_t onR = toLine(fR, cfg.lineIsHigh);

  // Потеря линии
  if (!onL && !onR) {
    if (st.searching <= 0) st.searching = (int16_t)cfg.searchTicks;

    st.searching--;

    int dir = (st.lastTurnDir == 0) ? 1 : st.lastTurnDir; // если не знаем — крутим вправо
    out.left  = -cfg.searchSpeed * dir;
    out.right =  cfg.searchSpeed * dir;
    return out;
  }

  // Если мы нашли линию — выходим из поиска
  st.searching = 0;

  // Управление:
  // L=1 R=0 -> поворот влево
  // L=0 R=1 -> поворот вправо
  // L=1 R=1 -> едем прямо (перекрёсток/широкая линия)
  // L=0 R=0 -> уже обработали (поиск)
  int leftSpeed = cfg.baseSpeed;
  int rightSpeed = cfg.baseSpeed;

  if (onL && !onR) {
    // влево
    leftSpeed  = cfg.baseSpeed - cfg.turnDelta;
    rightSpeed = cfg.baseSpeed + cfg.turnDelta;
    st.lastTurnDir = -1;
  } else if (!onL && onR) {
    // вправо
    leftSpeed  = cfg.baseSpeed + cfg.turnDelta;
    rightSpeed = cfg.baseSpeed - cfg.turnDelta;
    st.lastTurnDir = +1;
  } else {
    // onL && onR
    st.lastTurnDir = 0;
  }

  leftSpeed  = clampi(leftSpeed, 0, 255);
  rightSpeed = clampi(rightSpeed, 0, 255);

  out.left = leftSpeed;
  out.right = rightSpeed;
  return out;
}
