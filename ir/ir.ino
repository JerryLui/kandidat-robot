#include <Event.h>
#include <Timer.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define analogMax 1023
#define pi 3.1416
// Servo Constants
Servo servo;
#define servoPin 13

const int servoThreshold  = 80;
const int servoResolution = 2;		// Number of steps per servo degree
const int minAngle = 0;
const int maxAngle = 180 * servoResolution;

// IR-sensor Constants
//const int sensorPin = A3;
SoftwareSerial mySerial(A3, 10);
SoftwareSerial myFake(A1, A2);

//SoftwareSerial mySerial(A3,10);
//SoftwareSerial myFake(A1,A2);

const int spanSizeThreshold = 3;
const int sensorRead = 20;				// Times to read the sensor data
const int sensorReadThreshold = analogMax * sensorRead * 0.64;	// Upperbound for ir 0.64

bool sensor = false;

// Directions
enum Direction {
  LEFT, RIGHT, STRAIGHT, BACK, UNKNOWN
};

void setup() {
	Serial.begin(9600);
	// Servo Setup
	//pinMode(sensorPin, INPUT_PULLUP);
  //mySerial.begin(2400);
  //myFake.begin(2400);
	servo.attach(servoPin);
  //myFake.listen();
  Wire.begin(1);
  Wire.onReceive(recieveEvent);
}

void loop() {
	Serial.println(servoScan());
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
	if (abs(clockwiseRead-counterClockwiseRead) > 12)
		return -1;
	else
		return (clockwiseRead+counterClockwiseRead)/2;	// Return the average of two readings

}

// Performs a incremental- or decremental sweep depending on input angles
int sweepScan(int startAngle, int endAngle) {
	if (startAngle < endAngle)
		return incrementalSweepScan(startAngle, endAngle);
	else
		return decrementalSweepScan(startAngle, endAngle);
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
	if (abs(secondSweepAverage-firstSweepAverage) > 5*servoResolution)
		return -1;
	else
		return ((firstSweepAverage + secondSweepAverage)/2)/servoResolution;
}

// Stupid ethod to chose between incremental & decremental search
int averageSweep(int startAngle, int endAngle) {
	if (startAngle < endAngle)
		return averageIncrementalSweep(startAngle, endAngle);
	else
		return averageDecrementalSweep(startAngle, endAngle);
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
	return total/elements;
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
	return total/elements;
}

// Help variable for large turns
double currentServoAngle;

// Turns servo to the given angle
void servoTurn(double angle) {
	servo.write(angle);
	// Adds an delay for large turns to complete before another task
	if (abs(angle-currentServoAngle) > 30) {
		delay((abs(angle-currentServoAngle)/180)*800);
	}
	currentServoAngle = angle;
}

// Reads sensor data
bool readSensor() {
  /*int reading = 0;

  double lastT = micros();
*/
  /*while (!mySerial.isListening() || !mySerial.available() && micros() - lastT < 9000) mySerial.listen();

  if (micros() - lastT < 9000) { //mySerial.available()&&
    reading = mySerial.read();
  }

  while (!myFake.isListening()) myFake.listen();

  return reading > 0;
*/
  // Reads sensor data sensorRead times
  /*for (int i = 0; i < sensorRead; i++) {
  	reading += analogRead(sensorPin);	// Data from IR is HIGH when no signal
    }

    // If reading is less than sensorReadThreshold return true
    if (reading < sensorReadThreshold) {
  	return true;
    }
    else {
  	return false;
    }	*/
  sensor = false;
	Wire.beginTransmission(2);
  Wire.write('U');
  Wire.endTransmission(2);
  delayMicroseconds(10000);
  return sensor;
	// Reads sensor data sensorRead times
	/*for (int i = 0; i < sensorRead; i++) {
		reading += analogRead(sensorPin);	// Data from IR is HIGH when no signal
	}

	// If reading is less than sensorReadThreshold return true
	if (reading < sensorReadThreshold) {
		return true;
	}
	else {
		return false;
	}	*/
}
void recieveEvent(int howMany){
  sensor = Wire.read();
}

