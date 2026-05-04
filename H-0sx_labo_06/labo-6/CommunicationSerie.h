#pragma once
#include "Systeme.h"

class CommunicationSerie {
private:
  Mode *mode;

public:
  CommunicationSerie(Mode &m);
  void begin();
  void update();
}; 