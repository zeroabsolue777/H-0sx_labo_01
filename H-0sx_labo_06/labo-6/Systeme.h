#pragma once

enum Mode {
  MODE_RABAIS,
  MODE_NORMAL,
  MODE_ERREUR,
  MODE_FERMER,
  MODE_URGENCE
};

class Systeme {
private:
  Mode mode;

public:
  Systeme();
  Mode& getMode();
};