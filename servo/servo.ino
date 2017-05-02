#include <Wire.h>
#include <Servo.h>

// Global Constants
const int mainBoardAddress = 2;

double currentAngle = 0;
double angle = 0;

// Servo Constants
Servo servo;

#define servoPin 13

void setup() {
	Wire.begin(mainBoardAddress);
	Wire.onReceive(recieveEvent);

	servo.attach(servoPin);
}

void loop() {
}

void recieveEvent(int event){
	// Read and store angle if Wire available
	if (Wire.available()) {
		servoTurn(Wire.read());
	}
}

void servoTurn(double angle) {
	// Check if input angle is different to currentAngle
	if (abs(currentAngle-angle) > 0.1) {
		servo.write(angle);
		currentAngle = angle;
	}
}
