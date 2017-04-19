#include <Servo.h>
#include <Event.h>
#include <Timer.h>

// Direction Variables
#define lDirection 4
#define lStep 3
#define lOutput 5

#define rDirection 7
#define rStep 6
#define rOutput 2

#define motorDelayLowerBound 6000		// Delaytime for motor
#define motorDelayUpperBound 2000

// Motor Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT
};

// Setup
void setup() {
	pinMode(lOutput, OUTPUT);pinMode(lStep, OUTPUT);pinMode(lDirection, OUTPUT);
	pinMode(rOutput, OUTPUT);pinMode(rStep, OUTPUT);pinMode(rDirection, OUTPUT);
	digitalWrite(lOutput, LOW);digitalWrite(lDirection, LOW);
	digitalWrite(rOutput, LOW);digitalWrite(rDirection, HIGH);
}

// Main Loop
void loop() {
	step(motorDelayLowerBound);
}

// Motor Functions
void walk(Direction dir, int steps) {
	int currentDelay = motorDelayUpperBound;
	direction(dir);
	for (int i = 0; i < steps; i++) {
	}
}

// Moves one step by sending out a square wave with the input motorDelay
void step(int motorDelay) {
	delayMicroseconds(motorDelay);
	digitalWrite(lStep, HIGH);digitalWrite(rStep, HIGH);
	delayMicroseconds(motorDelay);
	digitalWrite(lStep, LOW);digitalWrite(rStep, LOW);
}

// Changes the direction of motors
void direction(Direction dir) {
	switch(dir) {
		case LEFT:
			digitalWrite(lDirection, LOW);
			digitalWrite(rDirection, LOW);
			break;
		case RIGHT:
			digitalWrite(lDirection, HIGH);
			digitalWrite(rDirection, HIGH);
			break;
		case STRAIGHT:
			digitalWrite(lDirection, LOW);
			digitalWrite(rDirection, HIGH);
			break;
	}
}
