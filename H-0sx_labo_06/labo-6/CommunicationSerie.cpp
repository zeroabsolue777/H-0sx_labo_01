#include <Arduino.h>
#include "CommunicationSerie.h"
#include <Arduino.h>

CommunicationSerie::CommunicationSerie(Mode &m) : mode(&m) {}

void CommunicationSerie::begin() {
  Serial.begin(9600);
}

void CommunicationSerie::update() {
  if (Serial.available()) {

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (*mode != MODE_URGENCE) {
      if (cmd == "rabais") *mode = MODE_RABAIS;
      else if (cmd == "normal") *mode = MODE_NORMAL;
      else if (cmd == "erreur") *mode = MODE_ERREUR;
      else if (cmd == "fermer") *mode = MODE_FERMER;
    }
  }
}