#include "Moteur.h"

Moteur::Moteur(int pin1, int pin2)
    : in1(pin1), in2(pin2) {}

void Moteur::begin() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    // Moteur arrêté au démarrage
    analogWrite(in1, 0);
    analogWrite(in2, 0);
}

void Moteur::update(EtatConvoyeur etat, int vitesse) {

    switch (etat) {

        case AVANCE:
            analogWrite(in1, vitesse);
            analogWrite(in2, 0);
            break;

        case RECULE:
            analogWrite(in1, 0);
            analogWrite(in2, vitesse);
            break;

        default: // ARRET ou URGENCE
            analogWrite(in1, 0);
            analogWrite(in2, 0);
            break;
    }
}