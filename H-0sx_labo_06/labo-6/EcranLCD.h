#pragma once
#include <LCD_I2C.h>
#include "Systeme.h"

class EcranLCD {
private:
  LCD_I2C lcd;

public:
  EcranLCD();
  void begin();
  void afficher(Mode mode);
};