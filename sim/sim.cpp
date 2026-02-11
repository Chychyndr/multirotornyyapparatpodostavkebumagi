// sim/sim.cpp
#include <iostream>
#include <vector>
#include <utility>
#include <control.h>

int main() {
  ControlConfig cfg;
  ControlState st;

  // Универсально:
  // true  -> линия = 1
  // false -> линия = 0
  cfg.lineIsHigh = true;

  cfg.baseSpeed = 140;
  cfg.turnDelta = 90;
  cfg.searchSpeed = 80;
  cfg.filterWindow = 3;
  cfg.searchTicks = 10;

  // Сценарий: пары (rawL, rawR) как будто это digitalRead() (0/1)
  // Тут линия чаще справа, потом слева, потом перекресток, потом потеря
  std::vector<std::pair<int,int>> scenario = {
    {0,0}, {0,0},               // потеря
    {0,1}, {0,1}, {0,1},        // линия справа -> вправо
    {0,1}, {0,0}, {0,1},        // шум: кратковременная потеря
    {1,0}, {1,0},               // линия слева -> влево
    {1,1}, {1,1},               // перекрёсток/широкая линия -> прямо
    {0,0}, {0,0}                // потеря
  };

  for (size_t i = 0; i < scenario.size(); i++) {
    uint8_t rawL = (uint8_t)scenario[i].first;
    uint8_t rawR = (uint8_t)scenario[i].second;

    MotorCmd cmd = computeLineFollowerDigital(rawL, rawR, cfg, st);

    std::cout << "Step " << i
              << "  rawL/rawR=" << int(rawL) << "/" << int(rawR)
              << "  motors L/R=" << cmd.left << "/" << cmd.right
              << "  lastDir=" << int(st.lastTurnDir)
              << "  searching=" << int(st.searching)
              << "\n";
  }

  // Быстрый тест универсальности: переключаем инверсию
  std::cout << "\n--- Switch: lineIsHigh=false (inverted sensors) ---\n";
  cfg.lineIsHigh = false;
  ControlState st2;

  for (size_t i = 0; i < scenario.size(); i++) {
    // Инвертируем "сырой" сигнал, как будто датчик выдаёт противоположное
    uint8_t rawL = (uint8_t)(1 - scenario[i].first);
    uint8_t rawR = (uint8_t)(1 - scenario[i].second);

    MotorCmd cmd = computeLineFollowerDigital(rawL, rawR, cfg, st2);

    std::cout << "Step " << i
              << "  rawL/rawR=" << int(rawL) << "/" << int(rawR)
              << "  motors L/R=" << cmd.left << "/" << cmd.right
              << "  lastDir=" << int(st2.lastTurnDir)
              << "  searching=" << int(st2.searching)
              << "\n";
  }

  return 0;
}
