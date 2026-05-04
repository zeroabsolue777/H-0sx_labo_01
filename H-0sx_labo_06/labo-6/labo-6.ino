#include <Arduino.h>
#include <U8g2lib.h>

#define CLK_PIN  8
#define DIN_PIN  10
#define CS_PIN   9

U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2(
  U8G2_R0,
  CLK_PIN,
  DIN_PIN,
  CS_PIN,
  U8X8_PIN_NONE,
  U8X8_PIN_NONE
);

#include "Systeme.h"
#include "Bouton.h"
#include "EcranLCD.h"
#include "MatriceLED.h"
#include "TelecommandeIR.h"
#include "CommunicationSerie.h"

Systeme systeme;
Bouton bouton(2, systeme.getMode());
EcranLCD lcd;
MatriceLED matrice;
TelecommandeIR ir(11, systeme.getMode());
CommunicationSerie serialCom(systeme.getMode());

void setup() {
  bouton.begin();
  lcd.begin();
  matrice.begin();
  ir.begin();
  serialCom.begin();
}

void loop() {

  bouton.update();

  if (systeme.getMode() == MODE_URGENCE) {
    lcd.afficher(MODE_URGENCE);
    matrice.afficher(MODE_URGENCE);
    return;
  }

  ir.update();
  serialCom.update();

  lcd.afficher(systeme.getMode());
  matrice.afficher(systeme.getMode());
}