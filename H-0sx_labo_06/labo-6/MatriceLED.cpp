#include <Arduino.h>
#include "MatriceLED.h"

MatriceLED::MatriceLED() {
  lastUpdate = 0;
  blinkState = false;
  scrollX = 8;
}

void MatriceLED::begin() {
  u8g2.begin();
}

void MatriceLED::afficherSmiley() {
  u8g2.drawCircle(3, 4, 3, U8G2_DRAW_ALL);
  u8g2.drawPixel(2, 3);
  u8g2.drawPixel(4, 3);
  u8g2.drawLine(2, 5, 4, 5);
}

void MatriceLED::afficherX() {
  u8g2.drawLine(0, 0, 7, 7);
  u8g2.drawLine(7, 0, 0, 7);
}

void MatriceLED::afficherErreur() {
  if (millis() - lastUpdate > 300) {
    lastUpdate = millis();
    blinkState = !blinkState;
  }

  if (blinkState) {
    u8g2.drawBox(0, 0, 8, 8);
  }
}

void MatriceLED::afficherRabais() {
  if (millis() - lastUpdate > 100) {
    lastUpdate = millis();
    scrollX--;
    if (scrollX < -40) scrollX = 8;
  }

  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(scrollX, 7, "SPECIAL");
}

void MatriceLED::afficher(Mode mode) {
  u8g2.clearBuffer();

  switch (mode) {
    case MODE_RABAIS: afficherRabais(); break;
    case MODE_NORMAL: afficherSmiley(); break;
    case MODE_ERREUR: afficherErreur(); break;
    case MODE_FERMER: break;
    case MODE_URGENCE: afficherX(); break;
  }

  u8g2.sendBuffer();
}