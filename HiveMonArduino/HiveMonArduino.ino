/*
 Name:    HiveMonArduino.ino
 Author:  Mario Nitchev
*/

#include <Servo.h>
#include <Adafruit_Si7021.h>
#include <LiquidCrystal.h>

const char LOCK = '0';
const char UNLOCK = '1';
const char GET_DATA = '2';

const int LOCKED = 100;
const int UNLOCKED = 10;
bool locked = false;

const int temperaturePin = A0;
const int lightPin = A1;
const int servoPin = 9;

const float baselineTemp = 20.0;
float angle = 0;
float light;
int count = 0;

Servo lockServo;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Adafruit_Si7021 temperatureAndHumiditySensor = Adafruit_Si7021();

void setup() {
  lcd.begin(16, 2);
  lockServo.attach(servoPin);
  lockServo.write(UNLOCKED);
  Serial.begin(9600);
  temperatureAndHumiditySensor.begin();
}

void loop() {
  if (serialHasData()) {
    char command = readCommand();
    interpreteCommand(command);
  }
  delay(15);
}

bool serialHasData() {
  int numberOfBytes = Serial.available();
  return numberOfBytes > 0;
}

char readCommand() {
  return Serial.read();
}

void interpreteCommand(char command) {
  switch (command) {
  case LOCK: rotateServo(LOCKED); break;
  case UNLOCK: rotateServo(UNLOCKED); break;
  case GET_DATA: sendSensorData(); break;
  }
}

float getVoltage(int pin){
  return (analogRead(pin) / 1024.0) * 5.0;
}

void sendSensorData() {
  String jsonData = collectSensorData();
  Serial.print(jsonData);
}

String serializeData(float piloTemperature, float hiveTemperature, float humidity, float light) {
  String status;
  if(locked){
    status = "\"locked\"";
  }else{
    status = "\"unlocked\"";
  }
  String jsonString = String("{ \"piloTemperature\" : " + String(piloTemperature) + 
    " , \"hiveTemperature\" : " + String(hiveTemperature) + " , \"humidity\" : " + String(humidity) +
    " , \"light\" : " + String(light) + " , \"status\" : " + status + " }\0");
  return jsonString;
}

String collectSensorData() {
  float humidity, piloTemperature, hiveTemperature, light, voltage;
  light = analogRead(lightPin);
  voltage = getVoltage(temperaturePin);
  piloTemperature = (voltage - 0.5) * 100.0;
  hiveTemperature = temperatureAndHumiditySensor.readTemperature();
  humidity = temperatureAndHumiditySensor.readHumidity();
  updateLcdScreen(piloTemperature, hiveTemperature, humidity, light);
  return serializeData(piloTemperature, hiveTemperature, humidity, light);
}

void updateLcdScreen(float piloTemperature, float hiveTemperature, float humidity, float light){
  int piloTemperatureRough = (int)piloTemperature;
  int hiveTemperatureRough = (int)hiveTemperature;
  int humidityRough = (int)humidity;
  String status;
  if(locked){
    status = "Locked  ";
  }else{
    status = "Unlocked";
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  String sensorData = "Ht:" + String(hiveTemperatureRough) + "C   Pt:" + String(piloTemperatureRough) + "C";
  lcd.print(sensorData);
  lcd.setCursor(0, 1);
  lcd.print(status + "  Hm" + String(humidityRough) + String("%"));  
}

void rotateServo(int position) {
  if ((position == LOCKED && !locked) || (position == UNLOCKED && locked)) {
    locked = !locked;
    lockServo.write(position);
  }
}



