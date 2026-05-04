#include <Arduino.h>   
#include "Systeme.h"

Systeme::Systeme() : mode(MODE_NORMAL) {}

Mode& Systeme::getMode() {
  return mode;
}