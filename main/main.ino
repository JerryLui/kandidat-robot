#include <Wire.h>

// Global Constants
# define analogMax 1023
# define pi 3.1416
# define dockingPathLength 28 		// Distance between station and start of docking pattern

// Servo Constants
#define servoPin 13
#define servoBoardAddress 2

const int servoResolution = 1;		// Number of steps per servo degree
const int minAngle = 0;
const int maxAngle = 180*servoResolution;

// IR-sensor Constants
const int sensorPin = A3;

const int spanSizeThreshold = 3;
const int maxAngleDiff = 15 * servoResolution;
const int sensorRead = 30;				// Times to read the sensor data
const int sensorReadThreshold = analogMax*sensorRead*0.64;	// Upperbound for ir 0.64

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

// Global Variables
unsigned int globalMotorDelay = motorDelayLowerBound;

// Ultrasound Constants
const int trigPin = 10;		// Respective pins for ultrasound
const int echoPin = 11;

const int maxDistance = 100;		// Maximum distance in cm
const int minDistance = 10;

// Line Sensor Constants
#define lineThreshold	10

const int leftLinePin = A2;
const int rightLinePin = A1;

// Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT, BACK, UNKNOWN
};

// State
enum State {
	NAVIGATION, DOCKING, STANDBY
};

State state;

// Converts Direction to String
String directionToString(Direction dir) {
	switch (dir) {
		case LEFT:
			return "Left";
		case RIGHT:
			return "Right";
		case STRAIGHT:
			return "Straight";
		case BACK:
			return "Back";
		case UNKNOWN:
			return "Unknown";
	}
}

// Converts State to String
String stateToString(State inputState) {
	switch (inputState) {
		case NAVIGATION:
			return "Navigation";
		case DOCKING:
			return "Docking";
		case STANDBY:
			return "Standby";
	}
}


// Setup
void setup() {
	// Initiate Serial Contact (for debug purposes)
	Serial.begin(9600);

	// Motor Setup
	pinMode(lOutput, OUTPUT);pinMode(lStep, OUTPUT);pinMode(lDirection, OUTPUT);
	pinMode(rOutput, OUTPUT);pinMode(rStep, OUTPUT);pinMode(rDirection, OUTPUT);
	digitalWrite(lOutput, LOW);digitalWrite(lDirection, HIGH);
	digitalWrite(rOutput, LOW);digitalWrite(rDirection, LOW);

	// Servo Setup
	pinMode(sensorPin, INPUT_PULLUP);

	Wire.begin();
	servoTurn(0);

	// Ultrasound Setup
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	// Line Sensor Setup
	pinMode(leftLinePin, INPUT);
	pinMode(rightLinePin, INPUT);

	// Sets Global Variables
	state = NAVIGATION;
}

int test = 1;
// Main Loop
void loop() {
	if (test == 1 && state != STANDBY) {
		navigate();
	}
}

/*~~~~~~~~~~ Robot Logic ~~~~~~~~~~*/
void navigate() {
	// Robo Logic
	// Scan for signal in 180 degrees in front of robot
	if (state == NAVIGATION && !inDockingRange()) {
		int angle = servoScan();
		// Serial.println(angle);
		// Check if signal out of bounds
		if (angle < 0 || angle > 180) {
			turn(LEFT, 120);
		}
		else if (angle > 95) {
			turn(LEFT, abs(90-angle));	// Rotate robot by given angular difference
			navigateHelper();
		}
		else if (angle < 85) {
			turn(RIGHT, abs(90-angle)); // Rotate robot by given anlgular difference
			navigateHelper();
		}
		else {
			navigateHelper();
		}
	}
	else if (state == DOCKING) {
		dockingNavigation(getLineDirection());
	}
}

void navigateHelper() {
	int distance = getDistance();
	if (!inDockingRange(distance)) {
		distanceNavigation(90, distance);
	} else {
		state = DOCKING;
	}
}

// Navigation for long distance
void distanceNavigation(int angle, int distance) {
	// Turns IR-sensor towoards signal
	servoTurn(angle);

	// While there's a signal and no lines are detected
	while (readSensor() && !inDockingRange(distance)) {
		// Retrieve distance from ultrasound sensor
		// If distance longer than 1m: walk towoards signal with small steps
		if (distance > 100 || distance < 10) {
			longWalk(STRAIGHT, 20);
		}
		// If distance longer than ultrasound lower bound walk all the way towoards it
		else if (distance > 30) {
			longWalk(STRAIGHT, distance-dockingPathLength);
		} else {
			longWalk(STRAIGHT, 5);
		}
		distance = getDistance();
	}
}

// Navigation for docking
void dockingNavigation(Direction dir) {
	while (!inDock(dir)) {
		dockWalk(dir, 1);
		dir = getLineDirection();
	}
}

bool inDockingRange() {
	return inDockingRange(getDistance());
}

// Checks if in range of station
bool inDockingRange(int distance) {
	if (distance < 30 && distance > 10 && readSensor()) {
		state = DOCKING;
		return true;
	} else {
		return false;
	}
}

// Checks if in dock
bool inDock(Direction dir) {
	if (dir == UNKNOWN) {
		state = STANDBY;
		return true;
	} else {
		return false;
	}
}

/*~~~~~~~~~~ Debug Robot Functions ~~~~~~~~~~*/
// Tests robot navigation functions
void debugRobotNavigation() {
	Serial.println("Testing Motor Functions: ");
	debugMotorFunctions();
	Serial.println("Testing Ultrasound Functions: ");
	debugPrintUltrasound();
	Serial.println("Testing Servo Functions: ");
	debugPrintServoScan();
	Serial.print("Testing Line Sensor: ");
	Serial.println(directionToString(getLineDirection()));
}

// Debug Servo
void debugPrintServoScan() {
	Serial.print("Angle (Original Method): ");
	Serial.println(servoScan());
	delay(1000);
	// Serial.print("Angle (Average Method): ");
	// Serial.println(averageServoScan());
}

// Print distance data from ultrasound
void debugPrintUltrasound() {
	Serial.print("Distance: ");
	Serial.println(getDistance());
	delay(1000);
}

// Tests motor functions
void debugMotorFunctions() {
	int debugDelayMillis = 800;

	// Long Walk Functions
	Serial.println("Testing longWalk()");
	longWalk(LEFT, 5);
	delay(debugDelayMillis);
	longWalk(RIGHT, 5);
	delay(debugDelayMillis);
	longWalk(STRAIGHT, 30);
	delay(debugDelayMillis);
	longWalk(STRAIGHT, 8);
	delay(debugDelayMillis);
	longWalk(BACK, 10);
	delay(debugDelayMillis);

	// Short Walk Functions
	Serial.println("Testing dockWalk()");
	dockWalk(LEFT, 20);
	delay(debugDelayMillis);
	dockWalk(RIGHT, 20);
	delay(debugDelayMillis);
	dockWalk(STRAIGHT, 20);
	delay(debugDelayMillis);
	dockWalk(BACK, 20);
	delay(debugDelayMillis);

	// Turn Functions
	Serial.println("Testing turn()");
	turn(LEFT, 45);
	delay(debugDelayMillis);
	turn(LEFT, 135);
	delay(debugDelayMillis);
	turn(RIGHT, 45);
	delay(debugDelayMillis);
	turn(RIGHT, 135);
	delay(debugDelayMillis);
	turn(STRAIGHT, 45);
	delay(debugDelayMillis);
}

/*~~~~~~~~~~ Line Sensor Functions ~~~~~~~~~~*/
// Reads data from line sensors, returns direction to go towoards
Direction getLineDirection() {
	bool leftData = readLeftLineSensor();
	bool rightData = readRightLineSensor();

	if (!leftData && !rightData)
		return STRAIGHT; // When no line found
	else if (leftData && !rightData)
		return RIGHT; 	// When line detected on left sensor
	else if (!leftData && rightData)
		return LEFT;		// When line detected on right sensor
	else
		return UNKNOWN;	// When line found
}

// Reads data from left line sensor, returns true if on a dark area
bool readLeftLineSensor() {
	// Serial.println(analogRead(leftLinePin));
	return analogRead(leftLinePin) < lineThreshold;
}

// Reads data from right line sensor, returns true if on a dark area
bool readRightLineSensor() {
	// Serial.println(analogRead(rightLinePin));
	return analogRead(rightLinePin) < lineThreshold;
}

/*~~~~~~~~~~ Servo Functions ~~~~~~~~~~*/
// Scans 0-180 degrees in front of the robot, returns angle for which there is a signal
int servoScan() {
	// First sweep scan using max span size method
	int clockwiseRead = sweepScan(minAngle, maxAngle);
	// Serial.println(clockwiseRead);
	if (clockwiseRead < 0)
		return -1;

	// Second sweep scan using max span size method
	int counterClockwiseRead = sweepScan(maxAngle, minAngle);
	// Serial.println(counterClockwiseRead);
	if (counterClockwiseRead < 0)
		return -1;

	// If difference between the two readings differ by more than 12 degrees
	return abs(clockwiseRead-counterClockwiseRead) > maxAngleDiff ?
		-1 : (clockwiseRead+counterClockwiseRead)/2;	// Return the average of two readings

}

// Performs a incremental- or decremental sweep depending on input angles
int sweepScan(int startAngle, int endAngle) {
	return startAngle < endAngle ?
		incrementalSweepScan(startAngle, endAngle) : decrementalSweepScan(startAngle, endAngle);
}

// Incremental sweep method by finding the largest span
int incrementalSweepScan(int startAngle, int endAngle) {
	// Initiate variables
	int maxSpanSize = 0;
	int spanSize = 0;
	int spanEnd = 0;

	while (startAngle < endAngle) {
		// Turns reciever towoards the angle
		servoTurn(startAngle/servoResolution);

		// If a signal is found
		if (readSensor()) {
			spanSize++;					// Increase the span size
		} else if (spanSize != 0) {			// Otherwise
			if (maxSpanSize < spanSize) {	// If current span is largest
				maxSpanSize = spanSize;			// Save it as largest
				spanEnd = startAngle-1;							// Save the last element of span
			}
			spanSize = 0;				// Reset span size
		}

		startAngle++;
	}

	// Returns the center of the measured span if maxSpanSize is larger than spanSizeThresh
	return (maxSpanSize > spanSizeThreshold) ? (spanEnd-maxSpanSize/2)/servoResolution : -1;
}

// Decremental sweep method by finding the largest span
int decrementalSweepScan(int startAngle, int endAngle) {
	// Initiate variables
	int maxSpanSize = 0;
	int spanSize = 0;
	int spanEnd = 0;

	while (startAngle > endAngle) {
		// Turns reciever towoards the angle
		servoTurn(startAngle/servoResolution);

		// If a signal is found
		if (readSensor()) {
			spanSize++;					// Increase the span size
		} else if (spanSize != 0) {			// Otherwise
			if (maxSpanSize < spanSize) {	// If current span is largest
				maxSpanSize = spanSize;			// Save it as largest
				spanEnd = startAngle+1;							// Save the last element of span
			}
			spanSize = 0;				// Reset span size
		}

		startAngle--;
	}

	// Returns the center of the measured span if maxSpanSize is larger than spanSizeThresh
	return (maxSpanSize > spanSizeThreshold) ? (spanEnd+maxSpanSize/2)/servoResolution : -1;
}

// An alternative method using average of positive signals
int averageServoScan() {
	// Stores the average of signal position
	int firstSweepAverage = averageSweep(minAngle, maxAngle);
	//Serial.println(firstSweepAverage);

	// Stores the average pos from second sweep
	int secondSweepAverage = averageSweep(maxAngle, minAngle);

	// Returns -1 if difference between the result from first & second sweep are
	// larger than 5 degrees. Else returns the angle at which a signal was found.
	return abs(secondSweepAverage-firstSweepAverage) > maxAngleDiff ?
		-1 : (firstSweepAverage + secondSweepAverage)/2;
}

// Stupid ethod to chose between incremental & decremental search
int averageSweep(int startAngle, int endAngle) {
	return startAngle < endAngle ?
		averageDecrementalSweep(startAngle, endAngle) : averageDecrementalSweep(startAngle, endAngle);
}

// Average incremental sweep used in averageScan() method.
int averageIncrementalSweep(int startAngle, int endAngle) {
	int spanSize = 0;
	int total = 0;
	int elements = 0;

	// Method only saves elements with a spansize > 2
	while (startAngle < endAngle) {
		servoTurn(startAngle/servoResolution);
		// If a positive signal is recieved
		if (readSensor()) {
			spanSize++;	// Increase the current span
			if (spanSize > 1) {		// If current spanSize > 1
				total += startAngle;	// Add current angle to total
				elements++;						// Increment nmbr of elements
			}
		} else { 	// I no signals are detected
			if (spanSize > 1) {
				total -= startAngle-1;
				elements--;
			}
			spanSize = 0;
		}
		startAngle++;
	}
	return (total/elements)/servoResolution;
}

// Average decremental sweep
int averageDecrementalSweep(int startAngle, int endAngle) {
	int spanSize = 0;
	int total = 0;
	int elements = 0;

	while (startAngle > endAngle) {
		servoTurn(startAngle/servoResolution);
		// If a positive signal is recieved
		if (readSensor()) {
			spanSize++;	// Increase the current span
			if (spanSize > 1) {		// If current spanSize > 1
				total += startAngle;	// Add current angle to total
				elements++;						// Increment nmbr of elements
			}
		} else { 	// I no signals are detected
			if (spanSize > 1) {
				total -= startAngle+1;
				elements--;
			}
			spanSize = 0;
		}
		startAngle--;
	}
	return (total/elements)/servoResolution;
}

// Help variable for large turns
double currentServoAngle;

// OLD: Turns servo to the given angle
void servoTurn(int angle) {
	// Begin transmission to second board
	Wire.beginTransmission(servoBoardAddress);
	Wire.write(angle);	// Send angle
	Wire.endTransmission();
	delay(5);

	// Adds an delay for large turns to complete before another task
	if (abs(angle-currentServoAngle) > 15) {
		delay((abs(angle-currentServoAngle)/180.0)*900);
	}

	// Re asigns currentServoAngle
	currentServoAngle = angle;
}

// Reads sensor data
bool readSensor() {
	int reading = 0;

	// Reads sensor data sensorRead times
	for (int i = 0; i < sensorRead; i++) {
		reading += analogRead(sensorPin);	// Data from IR is HIGH when no signal
	}
	// If reading is less than sensorReadThreshold return true
	return reading < sensorReadThreshold;
}

/*~~~~~~~~~~ Ultrasound Functions ~~~~~~~~~~*/
// Gets the distance to object in front of robot
int getDistance() {
	sendEcho();
	return recieveEcho();
}

// Sends an ultrasound pulse
void sendEcho() {
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);			// Send an exiting pulse
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
}

// Recieves an ultrasound pulse and returns the distance
int recieveEcho() {
	int distance = pulseIn(echoPin, HIGH)/29/2;

	pulseIn(echoPin, LOW);
	if (distance < maxDistance && distance > minDistance) {
		return distance;
	} else {
		return 0;
	}
}

/*~~~~~~~~~~ Motor Functions ~~~~~~~~~~~~*/
// Walks towoards direction in given length in centimeters
void longWalk(Direction dir, int length) {
	direction(dir);
	globalMotorDelay = motorDelayUpperBound;

	int steps = lengthToSteps(length);
	// Walks given steps, if nmbr of steps is longer than 2*accelerationLenght
	// skip constant run.
	if ((steps-2*accelerationLength) < 0) {
		accelerate(steps/2);
		deaccelerate(steps/2);
	} else {
		accelerate(accelerationLength);
		runMotor(steps-2*accelerationLength);
		deaccelerate(accelerationLength);
	}
}

// Short walk without acceleration and deacceleration
void dockWalk(Direction dir, int steps) {
	globalMotorDelay = motorDelayUpperBound-1000;
	direction(dir);
	if (steps > 1) {
		for (int i = 0; i < steps; i++) {
			step();
		}
	}
	else step();
}

/*	Converts length into motor steps

		The wheels have a radius of 3.4 cm, which results in a circumference
		of 6.8*pi cm. To convert from length to steps we use the constant steps-
		PerTurn seen in the return formula below. 															*/
int lengthToSteps(int length) {
	return length/(6.8*pi) * stepsPerTurn;
}

// Turns towoards direction by given angle
void turn(Direction dir, double angle) {
	if (dir != LEFT && dir != RIGHT) {}
	else {
		direction(dir);
		globalMotorDelay = motorDelayLowerBound+1000;
		runMotor((angle/180.0)*stepsPerTurn);
	}
}

// Runs motor for given amount of steps
void runMotor(int steps) {
	for (; steps > 0; steps--) {
		step();
	}
}

// Accelerates in given number of steps, returns the current motor delay
void accelerate(int steps) {
	for (; steps > 0; steps--) {
		step();
		decrementDelay();
	}
}

// Deaccelerates in given number of steps, returns the current motor delay
void deaccelerate(int steps) {
	for (; steps > 0; steps--) {
		step();
		incrementDelay();
	}
}

// Increments motor delay time by delayIncrement
int incrementDelay() {
	if (globalMotorDelay < motorDelayUpperBound)
		globalMotorDelay += delayIncrement;
}

// Decrements motor delay time by delayIncrement
int decrementDelay() {
	if (globalMotorDelay > motorDelayLowerBound)
		globalMotorDelay -= delayIncrement;
}

// Turns around 180 degrees anti clockwise
void turnAround() {
	turnAround(LEFT);
}

// Turns around 180 degree in the given direction
void turnAround(Direction dir) {
	turn(dir, 180);
}

// Moves one step by sending out a square wave with the input motorDelay
void step() {
	delayMicroseconds(globalMotorDelay);
	digitalWrite(lStep, HIGH);digitalWrite(rStep, HIGH);
	delayMicroseconds(globalMotorDelay);
	digitalWrite(lStep, LOW);digitalWrite(rStep, LOW);
}

// Changes the direction of motors
void direction(Direction dir) {
	switch (dir) {
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
