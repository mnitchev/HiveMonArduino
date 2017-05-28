/*
 Name:    HiveMonArduino.ino
 Author:  Mario Nitchev
*/

#include <Servo.h>

const char LOCK = '0';
const char UNLOCK = '1';
const char GET_DATA = '2';

const int LOCKED = 100;
const int UNLOCKED = 10;
bool locked = false;

const int temperaturePin = A0;
const int lightPin = A1;
const int servoPin = 3;

const int redLEDPin = 11;
const int blueLEDPin = 10;
const int greenLEDPin = 9;

const float baselineTemp = 20.0;
float angle = 0, dir = 1.0;
float light;
int count = 0;

Servo lockServo;

void setup() {
  lockServo.attach(servoPin);
  lockServo.write(UNLOCKED);
  Serial.begin(9600);
  lightGreen();
}

void loop() {
  if (serialHasData()) {
    lightYellow();
    char command = readCommand();
    interpreteCommand(command);
    changeLockLight();
  }
  delay(15);
}

void lightRed() {
  analogWrite(redLEDPin, 255);
  analogWrite(blueLEDPin, 0);
  analogWrite(greenLEDPin, 0);
}

void lightYellow(){
  analogWrite(redLEDPin, 255);
  analogWrite(blueLEDPin, 0);
  analogWrite(greenLEDPin, 255);
}

void lightGreen() {
  analogWrite(redLEDPin, 0);
  analogWrite(blueLEDPin, 0);
  analogWrite(greenLEDPin, 255);
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
  hiveTemperature = 0.0;
  humidity = 0.0;

  return serializeData(piloTemperature, hiveTemperature, humidity, light);
}

void rotateServo(int position) {
  if ((position == LOCKED && !locked) || (position == UNLOCKED && locked)) {
    locked = !locked;
    lockServo.write(position);
  }
}

void changeLockLight(){
 if(locked){ 
    lightRed();
 }else{
    lightGreen();
 }
}
