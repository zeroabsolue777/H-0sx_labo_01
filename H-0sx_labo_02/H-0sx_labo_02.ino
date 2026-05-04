const int POTPIN = A1;
const int BUTTONPIN = 2;
const int LEDPINS[] = {7, 8, 9, 10};

bool buttonState = true;
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCEDELAY = 50;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTONPIN, INPUT_PULLUP);

  for (int pin : LEDPINS) {
    pinMode(pin, OUTPUT);
  }
}

void loop() {
  int progress = lireProgressionPot();
  int pourcentage = map(progress, 0, 20, 0, 100);

  gererBouton(pourcentage, progress);
  controlLEDs(pourcentage, progress);
}

//gerer les anti-rebont
void gererBouton(int pourcentage, int progress) {
  bool reading = (digitalRead(BUTTONPIN) == LOW);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > DEBOUNCEDELAY) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState) {
        afficherProgression(pourcentage, progress);
      }
    }
  }

  lastButtonState = reading;
}

int lireProgressionPot() {
  int potValue = analogRead(POTPIN);
  return map(potValue, 0, 1023, 0, 20);
}

void afficherProgression(int pourcentage, int progress) {
  Serial.print("[");
  for (int i = 0; i < 20; i++) {
    Serial.print(i < progress ? '!' : '.');
  }
  Serial.print("] ");
  Serial.print(pourcentage);
  Serial.println("%");
}

//permet de controler les leds en fonctions 
//du pourcentage fourni par la progresion du potentiometre
void controlLEDs(int pourcentage, int progress) {

  int ledActive;

  for (int i = 0; i < 4; i++) {
    int seuil = i * 25;  // 0%, 25%, 50%, 75%
    if (pourcentage > seuil) {
      ledActive = i;  // on garde la dernière LED valide
    }
  }

  for (int i = 0; i < 4; i++) {
    digitalWrite(LEDPINS[i], (i == ledActive) ? HIGH : LOW);
  }
}