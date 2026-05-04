#pragma once
#include <U8g2lib.h>
#include "Systeme.h"

extern U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2;

class MatriceLED {
private:
  unsigned long lastUpdate;
  bool blinkState;
  int scrollX;

public:
  MatriceLED();
  void begin();
  void afficher(Mode mode);

private:
  void afficherSmiley();
  void afficherX();
  void afficherErreur();
  void afficherRabais();
};