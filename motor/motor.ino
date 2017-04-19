// Direction Variables
#define lDirection 4
#define lStep 3
#define lOutput 5

#define rDirection 7
#define rStep 6
#define rOutput 2

// Motor Constants
const int stepsPerTurn = 391;							// Nmbr of steps per turn __NOT PRECISE__

// Motor Variables
const int motorDelayUpperBound = 6000;		// Delaytime for motor
const int motorDelayLowerBound = 2000;
const int delayIncrement = 40;
const int accelerationLength = (motorDelayUpperBound-motorDelayLowerBound)/delayIncrement;

// Global Constants
const float pi = 3.1416;

// Global Variables
int globalDelay = motorDelayLowerBound;

int testVar = 1;

// Motor Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT, BACK
};

// Setup
void setup() {
	pinMode(lOutput, OUTPUT);pinMode(lStep, OUTPUT);pinMode(lDirection, OUTPUT);
	pinMode(rOutput, OUTPUT);pinMode(rStep, OUTPUT);pinMode(rDirection, OUTPUT);
	digitalWrite(lOutput, LOW);digitalWrite(lDirection, HIGH);
	digitalWrite(rOutput, LOW);digitalWrite(rDirection, LOW);
	testVar = 1;
}

void loop() {
	if (testVar == 1) {
		walk(STRAIGHT, 6.8*pi);
		testVar++;
	} else if (testVar == 2) {
		turnAround();
		testVar++;
	}
}

/*~~~~~~~~~~ Motor Functions ~~~~~~~~~~~~*/
/* 	The wheels have a radius of 3.4 cm, which results in a circumference
		of 6.8*pi cm. To convert from length to steps we use the constant steps-
		PerTurn. 																																*/
// Walks towoards direction in given length in centimeters
void walk(Direction dir, int length) {
	direction(dir);
	
	// Converts length into steps for motor
	int steps = length/(6.8*pi) * stepsPerTurn; // Temporary workaround

	// Walks given steps
	if ((steps-2*accelerationLength) < 0) {
		accelerate(steps/2);
		deaccelerate(steps/2);
	} else {
		accelerate(accelerationLength);
		runMotor(steps-2*accelerationLength);
		deaccelerate(accelerationLength);
	}
}

// Turns towoards direction by given angle
void turn(Direction dir, int angle) {
	direction(dir);
	globalDelay = motorDelayLowerBound;
	runMotor(angle/180*stepsPerTurn);
}

// Runs motor for given amount of steps
void runMotor(int steps) {
	for (int i = 0; i < steps; i++) {
		step(globalDelay);
	}
}

// Accelerates in given number of steps, returns the current motor delay
void accelerate(int steps) {
	int currentDelay = motorDelayUpperBound;
	for (int i = 0; i < steps; i++) {
		step(currentDelay);
		currentDelay = decrementDelay(currentDelay);
	}
	globalDelay = currentDelay;
}

// Deaccelerates in given number of steps, returns the current motor delay
void deaccelerate(int steps) {
	int currentDelay = globalDelay;
	for (int i = 0; i < steps; i++) {
		step(currentDelay);
		currentDelay = incrementDelay(currentDelay);
	}
	globalDelay = currentDelay;
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

// Turns around 180 degrees anti clockwise 
void turnAround() {
	turnAround(LEFT);
}

void turnAround(Direction dir) {
	turn(dir, 180);
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
		case BACK:
			digitalWrite(lDirection, LOW);
			digitalWrite(rDirection, HIGH);
			break;
	}
}
