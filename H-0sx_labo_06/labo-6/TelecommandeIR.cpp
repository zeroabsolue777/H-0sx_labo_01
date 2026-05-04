#include <Arduino.h> 
#include <IRremote.h>
#include "TelecommandeIR.h"

TelecommandeIR::TelecommandeIR(int p, Mode &m) : pin(p), mode(&m) {}

void TelecommandeIR::begin() {
  IrReceiver.begin(pin);
}

void TelecommandeIR::update() {
  if (IrReceiver.decode()) {

    uint32_t cmd = IrReceiver.decodedIRData.decodedRawData;

    if (*mode != MODE_URGENCE) {
      switch (cmd) {
        case 0xF30CFF00: *mode = MODE_RABAIS; break;
        case 0xE718FF00: *mode = MODE_NORMAL; break;
        case 0xA15EFF00: *mode = MODE_ERREUR; break;
        case 0xF708FF00: *mode = MODE_FERMER; break;
      }
    }

    IrReceiver.resume();
  }
}