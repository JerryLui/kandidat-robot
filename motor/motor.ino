#include <Event.h>
#include <Timer.h>

// Direction Variables
#define lDirection 4
#define lStep 3
#define lOutput 5

#define rDirection 7
#define rStep 6
#define rOutput 2

// Motor Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT
};

// Setup
void setup() {
	pinMode(lOutput, OUTPUT);pinMode(lStep, OUTPUT);pinMode(lDirection, OUTPUT);
	digitalWrite(lOutput, LOW);digitalWrite(lOutput, HIGH);

	pinMode(rOutput, OUTPUT);pinMode(rStep, OUTPUT);pinMode(rDirection, OUTPUT);
	digitalWrite(rOutput, LOW);digitalWrite(rOutput, HIGH);
}

// Main Loop
void loop() {
	delayMicroseconds(10);
	step();
}

// Motor Functions
void step() {
	digitalWrite(lStep, HIGH);digitalWrite(rStep, HIGH);
	digitalWrite(lStep, LOW);digitalWrite(rStep, LOW);
}

void directions(Direction dir) {
	switch(dir) {
		case LEFT:
			digitalWrite(lDirection, LOW);
			digitalWrite(rDirection, LOW);
			break;
		case RIGHT:
			digitalWrite(lDirection, LOW);
			digitalWrite(rDirection, HIGH);
			break;
		case STRAIGHT:
			digitalWrite(lDirection, HIGH);
			digitalWrite(rDirection, HIGH);
			break;
	}
}
