#include <math.h>

enum EtatAppli {ETAT_DEPART, ETAT_VARIATION, ETAT_FIN};

EtatAppli etatActuelle = ETAT_DEPART;

int led = 0; // Broche PWM pour variation


void setup() {
  led = 13;
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void etatDepart() {
  int nombreClignotements = ceil(6.0 / 2.0);
  int delayclig = 350;

  for (int i = 0; i < nombreClignotements; i++) {
    digitalWrite(led, LOW);
    delay(delayclig);
    digitalWrite(led, HIGH);
    delay(delayclig);
  }
}

void etatX() {
   int variationMax = 255;
   int delayx = 2048 / 255;

   for (int i = 0; i <= variationMax; i++) {
      analogWrite(led, i); 
      delay(delayx);
    }

}

void etatY() {
  int delayY1 = 2000;
  int delayY2 = 1000;
  int delayY3 = 300;

  digitalWrite(led, LOW);
  delay(delayY3);

  digitalWrite(led, HIGH);
  delay(delayY1);

  digitalWrite(led, LOW);
  delay(delayY2);
}

void loop() {
  switch (etatActuelle) {
    case ETAT_DEPART:
      etatDepart();
      Serial.println("Etat : Clignotement- 2411160");
      etatActuelle = ETAT_VARIATION;    
    break;

    case ETAT_VARIATION:
      etatX();
      Serial.println("Etat : Varie  - 2411160");
      etatActuelle = ETAT_FIN;
      break;

    case ETAT_FIN:
      etatY();
      Serial.println("Etat : Allume - 2411160");
      etatActuelle = ETAT_DEPART;
      break;
  }
}
