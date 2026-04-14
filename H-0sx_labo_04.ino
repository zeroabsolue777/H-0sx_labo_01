#include <Servo.h>
#include <LCD_I2C.h>

// ======================================================
//  CONSTANTES DU SYSTEME
// ======================================================

// Pins
const int SERVO_PIN         = 6;
const int BOUTON_OUVERTURE  = 2;
const int BOUTON_URGENCE    = 3;
const int TRIG_PIN          = 9;
const int ECHO_PIN          = 10;

// Servo
const int ANGLE_MIN         = 10;
const int ANGLE_MAX         = 170;
const unsigned long SERVO_INTERVAL = 10;

// Pause
const unsigned long PAUSE_OUVERTE = 10000;

// Anti-rebond
const unsigned long DEBOUNCE_MS = 50;

// Capteur
const int DISTANCE_SEUIL    = 30;
const unsigned long CAPTEUR_INTERVAL = 60;
const unsigned long CAPTEUR_TIMEOUT  = 25;


// ======================================================
//  OBJETS
// ======================================================

LCD_I2C lcd(0x27, 16, 2);
Servo porte;


// ======================================================
//  VARIABLES GLOBALES
// ======================================================

int angle = ANGLE_MIN;
unsigned long previousMillis = 0;

unsigned long pauseStart = 0;

int etat = 2;  // 0=ouvre, 1=pause, 2=ferme, 3=urgence
bool urgenceActive = false;

// Anti-rebond
unsigned long lastDebounceUrg  = 0;
unsigned long lastDebounceOpen = 0;

int prevReadingUrg  = HIGH;
int prevReadingOpen = HIGH;

int stableUrg  = HIGH;
int stableOpen = HIGH;

// Capteur
enum EtatCapteur { CAPTEUR_IDLE, CAPTEUR_TRIG, CAPTEUR_ATTENTE_ECHO, CAPTEUR_LECTURE };
EtatCapteur etatCapteur = CAPTEUR_IDLE;

unsigned long lastCapteurTime = 0;
unsigned long echoStart = 0;
long distanceMesuree = 999;

// LCD
int dernierAffichage = -1;


// ======================================================
//  CAPTEUR HC-SR04 (non bloquant)
// ======================================================

void capteurIdle(unsigned long now) {
  if (now - lastCapteurTime >= CAPTEUR_INTERVAL) {
    lastCapteurTime = now;
    digitalWrite(TRIG_PIN, LOW);
    etatCapteur = CAPTEUR_TRIG;
  }
}

void capteurTrig() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  echoStart = micros();
  etatCapteur = CAPTEUR_ATTENTE_ECHO;
}

void capteurAttenteEcho(unsigned long now) {
  if (digitalRead(ECHO_PIN) == HIGH) {
    echoStart = micros();
    etatCapteur = CAPTEUR_LECTURE;
  }
  if (now - lastCapteurTime > CAPTEUR_TIMEOUT) {
    distanceMesuree = 999;
    etatCapteur = CAPTEUR_IDLE;
  }
}

void capteurLecture(unsigned long now) {
  if (digitalRead(ECHO_PIN) == LOW) {
    long duree = micros() - echoStart;
    distanceMesuree = duree * 0.034 / 2;
    etatCapteur = CAPTEUR_IDLE;
  }
  if (now - lastCapteurTime > CAPTEUR_TIMEOUT) {
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
  int readUrg = digitalRead(BOUTON_URGENCE);
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
        porte.attach(SERVO_PIN);
        angle = ANGLE_MIN;
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
  int readOpen = digitalRead(BOUTON_OUVERTURE);
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
  if (distanceMesuree < DISTANCE_SEUIL && etat == 2 && angle == ANGLE_MIN) {
    etat = 0;
    previousMillis = now;
    afficherEtat(etat);
  }
}


// ======================================================
//  MACHINE À ÉTATS — PORTE
// ======================================================

void etatOuverture(unsigned long now) {
  if (now - previousMillis >= SERVO_INTERVAL) {
    previousMillis = now;
    angle++;
    if (angle >= ANGLE_MAX) {
      angle = ANGLE_MAX;
      pauseStart = now;
      etat = 1;
      afficherEtat(etat);
    }
    porte.write(angle);
  }
}

void etatPauseOuverte(unsigned long now) {
  if (now - pauseStart >= PAUSE_OUVERTE) {
    etat = 2;
    previousMillis = now;
    afficherEtat(etat);
  }
}

void etatFermeture(unsigned long now) {
  if (now - previousMillis >= SERVO_INTERVAL) {
    previousMillis = now;
    angle--;
    if (angle <= ANGLE_MIN) {
      angle = ANGLE_MIN;
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
//  SETUP
// ======================================================

void setup() {
  porte.attach(SERVO_PIN);

  pinMode(BOUTON_OUVERTURE, INPUT_PULLUP);
  pinMode(BOUTON_URGENCE,   INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.print("2411160 - Labo 4");
  lcd.setCursor(0, 1);
  lcd.print("Initialisation...");
  delay(2000);
  lcd.clear();
}


// ======================================================
//  LOOP
// ======================================================

void loop() {
  unsigned long now = millis();

  updateCapteur();
  gererBoutonUrgence(now);

  if (urgenceActive) return;

  gererDetectionDistance(now);
  gererBoutonOuverture(now);
  updatePorte(now);
}
