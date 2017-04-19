#include <Servo.h>

// Servo Constants
Servo servo;
const int servoResolution = 2;		// Number of steps per servo degree
const int minAngle = 0;
const int maxAngle = 180*servoResolution;

// IR-sensor Constants
const int sensorThreshold = 80; 	// Threshold value for a read
const int sensorRead = 3;					// Times to read the sensor data

void setup() {
	Serial.begin(9600);
	pinMode(A5, INPUT_PULLUP);
	servo.attach(13);
}

void loop() {
	int angle = scan();
}

/*~~~~~~~~~~ Servo Functions ~~~~~~~~~~*/
// Scans 0-180 degrees in front of the robot, returns angle for which there is an signal
int scan() {
	int clockwiseReadings [maxAngle];
	int counterClockwiseReadings [maxAngle];

	for (int i = 0; i<maxAngle; i++) {
		servoTurnTo(i/servoResolution);
		clockwiseReadings[i] = readSensor();
	}

	for (int i = maxAngle; i>minAngle; i--) {
		servoTurnTo(i/servoResolution);
		counterClockwiseReadings[i] = readSensor();
	}
	
}

// Turns servo to the given angle
void servoTurnTo(double angle) {
	servo.write(angle);
}

// Reads sensor data
int readSensor() {
	
}
