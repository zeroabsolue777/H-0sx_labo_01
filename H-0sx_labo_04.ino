#include <Servo.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2);

Servo porte;
int angle = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 10;

unsigned long pauseStart = 0;
const unsigned long pauseDuration = 10000;

int etat = 2;

const int boutonOuverture = 2;
const int boutonUrgence   = 3;

bool urgenceActive = false;

unsigned long lastDebounceUrg  = 0;
unsigned long lastDebounceOpen = 0;
const unsigned long DEBOUNCE_MS = 50;

int prevReadingUrg  = HIGH;
int prevReadingOpen = HIGH;
int stableUrg  = HIGH;
int stableOpen = HIGH;

// -----------------------------
// HC-SR04
// -----------------------------
const int TRIG = 9;
const int ECHO = 10;
const int distanceSeuil = 30;

enum EtatCapteur { CAPTEUR_IDLE, CAPTEUR_TRIG, CAPTEUR_ATTENTE_ECHO, CAPTEUR_LECTURE };
EtatCapteur etatCapteur = CAPTEUR_IDLE;

unsigned long trigStart       = 0;
unsigned long echoStart       = 0;
long distanceMesuree          = 999;
const unsigned long CAPTEUR_INTERVAL = 60;
unsigned long lastCapteurTime = 0;


// ======================================================
//  CAPTEUR
// ======================================================

void capteurIdle(unsigned long now) {
  if (now - lastCapteurTime >= CAPTEUR_INTERVAL) {
    lastCapteurTime = now;
    digitalWrite(TRIG, LOW);
    etatCapteur = CAPTEUR_TRIG;
  }
}

void capteurTrig() {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  echoStart = micros();
  etatCapteur = CAPTEUR_ATTENTE_ECHO;
}

void capteurAttenteEcho(unsigned long now) {
  if (digitalRead(ECHO) == HIGH) {
    echoStart = micros();
    etatCapteur = CAPTEUR_LECTURE;
  }
  if (now - lastCapteurTime > 25) {
    distanceMesuree = 999;
    etatCapteur = CAPTEUR_IDLE;
  }
}

void capteurLecture(unsigned long now) {
  if (digitalRead(ECHO) == LOW) {
    long duree = micros() - echoStart;
    distanceMesuree = duree * 0.034 / 2;
    etatCapteur = CAPTEUR_IDLE;
  }
  if (now - lastCapteurTime > 25) {
    distanceMesuree = 999;
    etatCapteur = CAPTEUR_IDLE;
  }
}

void updateCapteur() {
  unsigned long now = millis();
  switch (etatCapteur) {
    case CAPTEUR_IDLE:          capteurIdle(now);        break;
    case CAPTEUR_TRIG:          capteurTrig();           break;
    case CAPTEUR_ATTENTE_ECHO:  capteurAttenteEcho(now); break;
    case CAPTEUR_LECTURE:       capteurLecture(now);     break;
  }
}


// ======================================================
//  LCD
// ======================================================

int dernierAffichage = -1;

void afficherEtat(int e) {
  if (e == dernierAffichage) return;
  lcd.clear();
  switch (e) {
    case 0:
      lcd.print("Porte :");
      lcd.setCursor(0, 1);
      lcd.print("OUVERTURE");
      break;
    case 1:
      lcd.print("Porte :");
      lcd.setCursor(0, 1);
      lcd.print("OUVERTE");
      break;
    case 2:
      lcd.print("Porte :");
      lcd.setCursor(0, 1);
      lcd.print("FERMETURE");
      break;
    case 3:
      lcd.print("!!! URGENCE !!!");
      lcd.setCursor(0, 1);
      lcd.print("Appuyer RESET");
      break;
  }
  dernierAffichage = e;
}


// ======================================================
//  BOUTONS
// ======================================================

void gererBoutonUrgence(unsigned long now) {
  int readUrg = digitalRead(boutonUrgence);
  if (readUrg != prevReadingUrg) lastDebounceUrg = now;

  if (now - lastDebounceUrg > DEBOUNCE_MS && readUrg != stableUrg) {
    stableUrg = readUrg;
    if (stableUrg == LOW) {
      if (!urgenceActive) {
        urgenceActive = true;
        etat = 3;
        porte.detach();
        afficherEtat(3);
      } else {
        urgenceActive = false;
        porte.attach(6);
        angle = 0;
        porte.write(angle);
        previousMillis = now;
        etat = 2;
        afficherEtat(etat);
      }
    }
  }
  prevReadingUrg = readUrg;
}

void gererBoutonOuverture(unsigned long now) {
  int readOpen = digitalRead(boutonOuverture);
  if (readOpen != prevReadingOpen) lastDebounceOpen = now;

  if (now - lastDebounceOpen > DEBOUNCE_MS && readOpen != stableOpen) {
    stableOpen = readOpen;
    if (stableOpen == LOW && etat == 2) {
      etat = 0;
      previousMillis = now;
      afficherEtat(etat);
    }
  }
  prevReadingOpen = readOpen;
}


// ======================================================
//  DETECTION AUTOMATIQUE
// ======================================================

void gererDetectionDistance(unsigned long now) {
  if (distanceMesuree > 0 && distanceMesuree < distanceSeuil && etat == 2 && angle == 0) {
    etat = 0;
    previousMillis = now;
    afficherEtat(etat);
  }
}


// ======================================================
//  MACHINE À ÉTATS — PORTE
// ======================================================

void etatOuverture(unsigned long now) {
  if (now - previousMillis >= interval) {
    previousMillis = now;
    angle++;
    if (angle >= 180) {
      angle = 180;
      pauseStart = now;
      etat = 1;
      afficherEtat(etat);
    }
    porte.write(angle);
  }
}

void etatPauseOuverte(unsigned long now) {
  if (now - pauseStart >= pauseDuration) {
    etat = 2;
    previousMillis = now;
    afficherEtat(etat);
  }
}

void etatFermeture(unsigned long now) {
  if (now - previousMillis >= interval) {
    previousMillis = now;
    angle--;
    if (angle <= 0) {
      angle = 0;
      afficherEtat(etat);
    }
    porte.write(angle);
  }
}

void updatePorte(unsigned long now) {
  switch (etat) {
    case 0: etatOuverture(now);    break;
    case 1: etatPauseOuverte(now); break;
    case 2: etatFermeture(now);    break;
  }
}


// ======================================================
//  SETUP & LOOP
// ======================================================

void setup() {
  porte.attach(6);

  pinMode(boutonOuverture, INPUT_PULLUP);
  pinMode(boutonUrgence,   INPUT_PULLUP);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("2411160 - Labo 4");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation...");
  delay(2000);
  lcd.clear();

}

void loop() {
  unsigned long now = millis();

  updateCapteur();
  gererBoutonUrgence(now);

  if (urgenceActive) return;

  gererDetectionDistance(now);
  gererBoutonOuverture(now);
  updatePorte(now);
}