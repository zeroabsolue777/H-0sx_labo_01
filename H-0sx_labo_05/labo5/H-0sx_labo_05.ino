#include <LCD_I2C.h>
#include "Moteur.h"

// ======================================================
// CONSTANTES DU SYSTEME
// ======================================================

// Pins moteur (via driver ou pont en H)
const int MOTOR_IN1 = 44;
const int MOTOR_IN2 = 45;

// LEDs
const int LED_MOUVEMENT = 12;
const int LED_URGENCE   = 6;

// Bouton d’urgence
const int PIN_URGENCE = 2;

// Joystick
const int JOY_X = A0;   // vitesse
const int JOY_Y = A1;   // direction

// Zone morte du joystick
const int JOY_MIN = 450;
const int JOY_MAX = 570;

// Vitesse
const int VITESSE_STEP = 3;

// Délais
const unsigned long DELAI_LCD   = 1500;
const unsigned long DEBOUNCE_MS = 50;


// ======================================================
// CLASSE JOYSTICK
// ======================================================
class Joystick {
private:
  int pinX, pinY;
  unsigned long lastUpdate;
  const int interval = 50;

public:
  Joystick(int x, int y) : pinX(x), pinY(y), lastUpdate(0) {}

  EtatConvoyeur getDirection() {
    int y = analogRead(pinY);

    if (y > JOY_MAX) return AVANCE;
    if (y < JOY_MIN) return RECULE;
    return ARRET;
  }

  void updateVitesse(int &vitesse) {
    if (millis() - lastUpdate < interval) return;
    lastUpdate = millis();

    int x = analogRead(pinX);

    if (x > JOY_MAX) vitesse = min(255, vitesse + VITESSE_STEP);
    else if (x < JOY_MIN) vitesse = max(0, vitesse - VITESSE_STEP);
  }
};


// ======================================================
// CLASSE URGENCE
// ======================================================
class Urgence {
private:
  int pin;
  bool lastState;
  unsigned long lastTime;

public:
  Urgence(int p) : pin(p), lastState(HIGH), lastTime(0) {}

  void begin() {
    pinMode(pin, INPUT_PULLUP);
  }

  void update(EtatConvoyeur &etat) {
    bool current = digitalRead(pin);

    if (current == LOW && lastState == HIGH) {
      if (millis() - lastTime > DEBOUNCE_MS) {
        lastTime = millis();
        etat = (etat == URGENCE ? ARRET : URGENCE);
      }
    }

    lastState = current;
  }
};


// ======================================================
// CLASSE LEDs
// ======================================================
class LEDs {
private:
  int ledMove, ledUrg;

public:
  LEDs(int m, int u) : ledMove(m), ledUrg(u) {}

  void begin() {
    pinMode(ledMove, OUTPUT);
    pinMode(ledUrg, OUTPUT);
  }

  void update(EtatConvoyeur etat) {
    digitalWrite(ledMove, (etat == AVANCE || etat == RECULE));
    digitalWrite(ledUrg,  (etat == URGENCE));
  }
};


// ======================================================
// CLASSE AFFICHAGE LCD
// ======================================================
class Affichage {
private:
  LCD_I2C lcd;
  int page;
  unsigned long lastUpdate;

public:
  Affichage() : lcd(0x27, 16, 2), page(0), lastUpdate(0) {}

  void begin() {
    lcd.begin();
    lcd.backlight();
  }

  void update(EtatConvoyeur etat, int vitesse) {
    if (millis() - lastUpdate < DELAI_LCD) return;
    lastUpdate = millis();

    lcd.clear();

    if (etat == URGENCE) {
      lcd.setCursor(0, 0);
      lcd.print("!!! URGENCE !!!");
      lcd.setCursor(0, 1);
      lcd.print("ARRET TOTAL");
      return;
    }

    switch (page) {
      case 0:
        lcd.print("Convoyeur:");
        lcd.setCursor(0, 1);
        lcd.print(etat == ARRET ? "NON" : "OUI");
        break;

      case 1:
        lcd.print("Sens:");
        lcd.setCursor(0, 1);
        if (etat == AVANCE) lcd.print("AVANT");
        else if (etat == RECULE) lcd.print("ARRIERE");
        else lcd.print("STOP");
        break;

      case 2:
        lcd.print("Vitesse:");
        lcd.setCursor(0, 1);
        lcd.print(vitesse);
        break;
    }

    page = (page + 1) % 3;
  }
};


// ======================================================
// OBJETS
// ======================================================
Moteur moteur(MOTOR_IN1, MOTOR_IN2);
Joystick joystick(JOY_X, JOY_Y);
Urgence urgence(PIN_URGENCE);
LEDs leds(LED_MOUVEMENT, LED_URGENCE);
Affichage affichage;

EtatConvoyeur etat = ARRET;
int vitesse = 0;


// ======================================================
// SETUP
// ======================================================
void setup() {
  moteur.begin();
  urgence.begin();
  leds.begin();
  affichage.begin();
}


// ======================================================
// LOOP
// ======================================================
void loop() {

  // Gestion urgence
  urgence.update(etat);

  // Si urgence → ignorer joystick
  if (etat != URGENCE) {
    etat = joystick.getDirection();
    joystick.updateVitesse(vitesse);
  }

  // Mise à jour des modules
  moteur.update(etat, vitesse);
  leds.update(etat);
  affichage.update(etat, vitesse);
}

