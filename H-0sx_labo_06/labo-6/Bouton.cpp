#include <Arduino.h>
#include "Bouton.h"

Bouton::Bouton(int pin, Mode &m) : bouton(pin, true), mode(&m) {}

void Bouton::onClick(void *instance) {
  Bouton *self = (Bouton*)instance;

  if (*(self->mode) == MODE_URGENCE)
    *(self->mode) = MODE_NORMAL;
  else
    *(self->mode) = MODE_URGENCE;
}

void Bouton::begin() {
  bouton.attachClick(onClick, this);
}

void Bouton::update() {
  bouton.tick();
}