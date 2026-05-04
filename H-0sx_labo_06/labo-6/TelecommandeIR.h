#pragma once
#include <Arduino.h>
#include "Systeme.h"

class TelecommandeIR {
private:
  int pin;
  Mode *mode;

public:
  TelecommandeIR(int p, Mode &m);
  void begin();
  void update();
};