#ifndef MOTEUR_H
#define MOTEUR_H

#include <Arduino.h>

enum EtatConvoyeur { ARRET, AVANCE, RECULE, URGENCE };

class Moteur {
private:
    int in1;
    int in2;

public:
    Moteur(int pin1, int pin2);

    void begin();
    void update(EtatConvoyeur etat, int vitesse);
};

#endif