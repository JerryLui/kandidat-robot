//#include <Event.h>
//#include <Timer.h>
//#include <SoftwareSerial.h>
#include <Wire.h>

#define analogMax 1023
#define pi 3.1416
#define servoBoardAddress 2

const int servoResolution = 1;		// Number of steps per servo degree TODO: returns faulty if > 1
const int minAngle = 0;
const int maxAngle = 180 * servoResolution;

// IR-sensor Constants
const int sensorPin = A3;

// Scanner Thresholds
const int spanSizeThreshold = 3;
const int maxAngleDiff = servoResolution * 15;

// readSensor() constants
const int sensorRead = 30;				// Times to read the sensor data
const int sensorReadThreshold = analogMax * sensorRead * 0.64;	// Upperbound for ir 0.64

bool sensor = false;

// Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT, BACK, UNKNOWN
};

void setup() {
	Serial.begin(9600);

	// Servo Setup
	pinMode(sensorPin, INPUT_PULLUP);
	Wire.begin();

}

void loop() {
	Serial.println("Initiate Scanning.");
	Serial.println(servoScan());
}

/*~~~~~~~~~~ Servo Functions ~~~~~~~~~~*/
// Scans 0-180 degrees in front of the robot, returns angle for which there is a signal
int servoScan() {
	// First sweep scan using max span size method
	int clockwiseRead = sweepScan(minAngle, maxAngle);
	Serial.println(clockwiseRead);
	if (clockwiseRead < 0)
		return -1;

	// Second sweep scan using max span size method
	int counterClockwiseRead = sweepScan(maxAngle, minAngle);
	Serial.println(counterClockwiseRead);
	if (counterClockwiseRead < 0) 
		return -1;

	// If difference between the two readings differ by more than maxAngleDiff
	return abs(clockwiseRead-counterClockwiseRead) > maxAngleDiff ? 
	-1 : (clockwiseRead+counterClockwiseRead)/2;
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
	Serial.println(firstSweepAverage);
	if (firstSweepAverage == -1) 
		return false;

	// Stores the average pos from second sweep
	int secondSweepAverage = averageSweep(maxAngle, minAngle);
	Serial.println(secondSweepAverage);
	if (secondSweepAverage == -1) 
		return false;

	// Returns -1 if difference between the result from first & second sweep are
	// larger than 5 degrees. Else returns the angle at which a signal was found.
	return abs(secondSweepAverage-firstSweepAverage) > maxAngleDiff ? 
	-1 : firstSweepAverage+secondSweepAverage/2;
}

// Stupid ethod to chose between incremental & decremental search
int averageSweep(int startAngle, int endAngle) {
	return startAngle < endAngle ? 
	averageIncrementalSweep(startAngle, endAngle) : averageDecrementalSweep(startAngle, endAngle);
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
	return elements > spanSizeThreshold ? (total/elements)/servoResolution : -1;
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
	return elements > spanSizeThreshold ? (total/elements)/servoResolution : -1;
}

// Help variable for large turns
double currentServoAngle;

// Turns servo to the given angle
void servoTurn(int angle) {
	Wire.beginTransmission(servoBoardAddress);
	Wire.write(angle);
	Wire.endTransmission();
	delay(5);

	// Adds an delay for large turns to complete before another task
	if (abs(angle-currentServoAngle) > 30) { 
		delay((abs(angle-currentServoAngle)/180.0)*800);
	}
	currentServoAngle = angle;
}

// Reads sensor data sensorRead times
bool readSensor() {
	int reading = 0;
	int data;
	for (int i = 0; i < sensorRead; i++) {
		/*data = analogRead(sensorPin);
		reading += data;
		Serial.println(data);
		delay(50);*/
		reading += analogRead(sensorPin);	// Data from IR is HIGH when no signal
	}

	// If reading is less than sensorReadThreshold return true
	return reading < sensorReadThreshold;
}
