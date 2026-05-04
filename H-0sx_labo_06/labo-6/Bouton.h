#pragma once
#include <OneButton.h>
#include "Systeme.h"

class Bouton {
private:
  OneButton bouton;
  Mode *mode;

  static void onClick(void *instance);

public:
  Bouton(int pin, Mode &m);
  void begin();
  void update();
};