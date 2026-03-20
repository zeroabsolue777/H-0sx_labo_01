#include <Wire.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27,16,2);

// Pins
const int LED_PIN = 8;
const int BUTTON_PIN = 2;
const int X_PIN = A1;
const int Y_PIN = A2;
const int SENSOR_PIN = A0;

// Variables capteur
float R1 = 10000; // Résistance
float logR2, R2, T, Tc;
float c1 = 1.129148e-03, c2 = 2.34125e-04, c3 = 8.76741e-08;

// État système
int systemState = 0; // 0 = OFF, 1 = ON
char DAmasquer[9];   // ****XX
int altitude = 0;

// Machine d'état
enum State {START_SCREEN, PAGE_JOYSTICK, PAGE_SENSOR};
State currentState = START_SCREEN;

// Bouton
bool buttonState = HIGH, lastButtonState = HIGH;
unsigned long lastButtonPress = 0;

// LCD
unsigned long lastLcdUpdate = 0;

// Serial
unsigned long lastSerialUpdate = 0;

// Custom char
byte numero[8] = {
  0B11100,
  0B10000,
  0B11100,
  0B10100,
  0B11111,
  0B00101,
  0B00101,
  0B00111
};

// Temps pour vitesse
unsigned long lastAltitudeUpdate = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lcd.begin(16,2);
  lcd.backlight();
  lcd.createChar(0, numero);

  // Masquer le code étudiant
  sprintf(DAmasquer,"******%02d", 60);
}

void loop() {
  taskButton();
  taskSerial();

  switch(currentState){
    case START_SCREEN: taskStart(); break;
    case PAGE_JOYSTICK: taskJoystick(); break;
    case PAGE_SENSOR: taskTemperature(); break;
  }
}

// ======================== Tâches ==========================
void taskStart(){
  static unsigned long startTime = millis();

  lcd.setCursor(0,0);
  lcd.print("Leon perez   "); // espaces pour effacer reste

  lcd.setCursor(0,1);
  lcd.print(char(0));
  lcd.setCursor(8,1);
  lcd.print(DAmasquer);

  if(millis() - startTime > 3000){
    lcd.clear();
    currentState = PAGE_JOYSTICK;
  }
}

void taskButton(){
  static unsigned long wait = 200;

  buttonState = digitalRead(BUTTON_PIN);

  if(buttonState == LOW && lastButtonState == HIGH){
    if(millis() - lastButtonPress >= wait){
      lastButtonPress = millis();
      
      // bascule d'écran
      if(currentState == PAGE_JOYSTICK) currentState = PAGE_SENSOR;
      else if(currentState == PAGE_SENSOR) currentState = PAGE_JOYSTICK;
      lcd.clear();
    }
  }
  lastButtonState = buttonState;
}

void taskJoystick(){
  unsigned long wait = 200;
  int valX = analogRead(X_PIN);
  int valY = analogRead(Y_PIN);

  int angle = map(valX,0,1023,-90,90);
  String directionX = (angle<0)?"(G)":"(D)";

  String mouvement = "STABLE";

  if(valY>600){
    mouvement = "UP";
    if(millis()-lastAltitudeUpdate>1000 && altitude<200){
      altitude++;
      lastAltitudeUpdate = millis();
    }
  } else if(valY<400){
    mouvement = "DOWN";
    if(millis()-lastAltitudeUpdate>1000 && altitude>0){
      altitude--;
      lastAltitudeUpdate = millis();
    }
  }

  // Affichage LCD
  lcd.setCursor(0,0);
  lcd.print(mouvement+" ALT:");
  lcd.print(altitude);
  lcd.print("m   ");

  lcd.setCursor(0,1);
  lcd.print("DIR:");
  lcd.print(abs(angle));
  lcd.print((char)223);
  lcd.print(" ");
  lcd.print(directionX);
  lcd.print("   ");
}

void taskTemperature(){
  unsigned long wait = 500;
  if(millis() - lastLcdUpdate >= wait){
    lastLcdUpdate = millis();

    int sensorValue = analogRead(SENSOR_PIN);
    // Conversion thermistance
    R2 = R1 * (1023.0/sensorValue -1.0);
    logR2 = log(R2);
    T = 1.0/(c1 + c2*logR2 + c3*logR2*logR2*logR2);
    Tc = T - 273.15;

    lcd.setCursor(0,0);
    lcd.print("TEMP:");
    lcd.print(Tc,1);
    lcd.print("C   ");

    lcd.setCursor(0,1);

    if(Tc>35){
      digitalWrite(LED_PIN,HIGH);
      lcd.print("COOL: ON ");
      systemState = 1;
    } else if(Tc<30){
      digitalWrite(LED_PIN,LOW);
      lcd.print("COOL: OFF");
      systemState = 0;
    }
  }
}

void taskSerial(){
  unsigned long wait = 100;
  if(millis() - lastSerialUpdate >= wait){
    lastSerialUpdate = millis();
    int valX = analogRead(X_PIN);
    int valY = analogRead(Y_PIN);

    Serial.print("etd:2411160,x:");
    Serial.print(valX);
    Serial.print(",y:");
    Serial.print(valY);
    Serial.print(",sys:");
    Serial.println(systemState);
  }
}
