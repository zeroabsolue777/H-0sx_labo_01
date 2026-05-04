#include <Arduino.h>
#include "EcranLCD.h"

EcranLCD::EcranLCD() : lcd(0x27, 16, 2) {}

void EcranLCD::begin() {
  lcd.begin();
  lcd.backlight();
}

void EcranLCD::afficher(Mode mode) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MODE:");

  lcd.setCursor(0, 1);

  switch (mode) {
    case MODE_RABAIS: lcd.print("RABAIS"); break;
    case MODE_NORMAL: lcd.print("NORMAL"); break;
    case MODE_ERREUR: lcd.print("ERREUR"); break;
    case MODE_FERMER: lcd.print("FERMER"); break;
    case MODE_URGENCE: lcd.print("!!!URGENCE!!!"); break;
  }
}