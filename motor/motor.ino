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

#define motorDelayUpperBound 6000		// Delaytime for motor
#define motorDelayLowerBound 2000
#define delayIncrement 40
#define accelerationLength 100			// The number of steps required for accelration/deacceleration

unsigned int testVar;

// Motor Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT
};

// Setup
void setup() {
	pinMode(lOutput, OUTPUT);pinMode(lStep, OUTPUT);pinMode(lDirection, OUTPUT);
	pinMode(rOutput, OUTPUT);pinMode(rStep, OUTPUT);pinMode(rDirection, OUTPUT);
	digitalWrite(lOutput, LOW);digitalWrite(lDirection, HIGH);
	digitalWrite(rOutput, LOW);digitalWrite(rDirection, LOW);
	testVar = 1;
}

// Main Loop
void loop() {
	if (testVar == 1) {
		walk(STRAIGHT, 600);
		testVar = 2;
	}
}

// Motor Functions

// Configures motor for the given direction dir walks for given steps
void walk(Direction dir, int steps) {
	direction(dir);
	deaccelerate(accelerationLength, accelerate(steps));
}

// Runs motor for given amount of steps
void runMotor(int steps, int currentDelay) {
	for (int i = 0; i < steps; i++) {
		step(currentDelay);
	}
}

// Accelerates in given number of steps, returns the current motor delay
int accelerate(int steps) {
	int currentDelay = motorDelayUpperBound;
	for (int i = 0; i < steps; i++) {
		step(currentDelay);
		currentDelay = decrementDelay(currentDelay);
	}
	return currentDelay;
}

// Deaccelerates in given number of steps, returns the current motor delay
int deaccelerate(int steps, int currentDelay) {
	for (int i = 0; i < steps; i++) {
		step(currentDelay);
		currentDelay = incrementDelay(currentDelay);
	}
	return currentDelay;
}

// Increments motor delay time by delayIncrement
int incrementDelay(int delayTime) {
	if (delayTime < motorDelayUpperBound) {
		return delayTime + delayIncrement;
	} else {
		return delayTime;
	}
}

// Decrements motor delay time by delayIncrement
int decrementDelay(int delayTime) {
	if (delayTime > motorDelayLowerBound) {
		return delayTime - delayIncrement;
	} else {
		return delayTime;
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
			digitalWrite(lDirection, HIGH);
			digitalWrite(rDirection, LOW);
			break;
	}
}
