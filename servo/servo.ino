#include <Wire.h>
#include <Servo.h>

// Global Constants
double angle = 0;
const int mainBoardAddress = 2;

// Servo Constants
Servo servo;

#define servoPin 13

void setup() {
  Wire.begin(mainBoardAddress);
  Wire.onReceive(recieveEvent);
  // Serial.begin(9600);
  servo.attach(servoPin);
  // Serial.println("Slave started");
}

void loop() {
  servo.write(angle);
  // Serial.println(angle);
}

void recieveEvent(int event){
  while(Wire.available()){
     angle = Wire.read()/10;
  }
}

