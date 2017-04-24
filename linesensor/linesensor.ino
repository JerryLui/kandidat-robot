#include <Servo.h>

#define analogMax 1023
#define pi 3.1416
// Servo Constants
Servo servo;
#define servoPin 13

const int servoThreshold  = 80;
const int servoResolution = 2;		// Number of steps per servo degree
const int minAngle = 0;
const int maxAngle = 180*servoResolution;

// IR-sensor Constants
const int sensorPin = A3;

const int spanSizeThreshold = 3;
const int sensorRead = 20;				// Times to read the sensor data
const int sensorReadThreshold = analogMax*sensorRead*0.8;	// Upperbound for ir 0.64

// Directions
enum Direction {
	LEFT, RIGHT, STRAIGHT, BACK, UNKNOWN
};

void setup() {
	Serial.begin(9600);
	// Servo Setup
	pinMode(sensorPin, INPUT_PULLUP);
	servo.attach(servoPin);
}

void loop() {
	debugPrintServoScan();
}

/*~~~~~~~~~~ Servo Functions ~~~~~~~~~~*/
// Debug Servo
void debugPrintServoScan() {
	Serial.print("Angle (Original Method): ");
	Serial.println(servoScan());
	Serial.print("Angle (Average Method): ");
	Serial.println(averageServoScan());
}

// Scans 0-180 degrees in front of the robot, returns angle for which there is an signal
int servoScan() {
	bool clockwiseReadings [maxAngle];
	bool counterClockwiseReadings [maxAngle];
	bool combinedReadings [maxAngle];

	// Sweeps the servo from minAngle to maxAngle while collecting the reading
	for (int i = minAngle; i<maxAngle; i++) {
		servoTurn(i/servoResolution);
		clockwiseReadings[i] = readSensor();
	}
	// does another sweep backwards
	for (int i = maxAngle; i>minAngle; i--) {
		servoTurn(i/servoResolution);
		counterClockwiseReadings[i] = readSensor();
	}

	int spanSize = 0;
	int spanEnd = 0;
	int maxSpanSize = 0;

	// Compares the readings from the two sweeps and finds the largest span
	for (int i = minAngle; i<maxAngle; i++) {
		// Sets combinedReadings to true if both sweeeps show an positive signal at i
		if (clockwiseReadings[i] && counterClockwiseReadings[i])
			combinedReadings[i] = true;

		// Increases the spanSize for each run
		if (combinedReadings[i]) {
			spanSize++;
		} else if (spanSize != 0) {
			if (spanSize > maxSpanSize) {
				maxSpanSize = spanSize;
				spanEnd = i-1;
			}
			spanSize = 0;
		}
	}

	// Returns the angle for which there's a signal, returns -1 if nothing found
	if (maxSpanSize < spanSizeThreshold) {
		return -1;
	} else {
		return (spanEnd-maxSpanSize/2)/servoResolution;
	}
}

// An alternative method using average of positive signals
int averageServoScan() {
	// Variables to hold the average
	int total = 0; 
	int elements = 0;
	int spanSize = 0;

	// Sweeps the servo from minAngle to maxAngle while collecting the reading
	for (int i = minAngle; i<maxAngle; i++) {
		servoTurn(i/servoResolution);
		if (readSensor() && spanSize++ > 1) {
			total += i;
			elements++;
		} else if (spanSize > 1) {
			total -= i-1;
			elements--;
			spanSize = 0;
		} else {
			spanSize = 0;
		}
	}

	// Stores the average of signal position
	int firstSweepAverage = total/elements;

	// Resets variables
	total = 0; elements = 0; spanSize = 0;

	// does another sweep backwards
	for (int i = maxAngle; i>minAngle; i--) {
		servoTurn(i/servoResolution);
		if (readSensor() && spanSize++ > 1) {
			total += i;
			elements++;
		} else if (spanSize > 1) {
			total -= i-1;
			elements--;
			spanSize = 0;
		} else {
			spanSize = 0;
		}
	}

	// Stores the average pos from second sweep
	int secondSweepAverage = total/elements;

	// Returns -1 if difference between the result from first & second sweep are
	// larger than 5 degrees. Else returns the angle at which a signal was found.
	if ((secondSweepAverage-firstSweepAverage) > 5) // ADD CONDITION
		return -1;
	else
		return ((firstSweepAverage + secondSweepAverage)/2)/servoResolution;
}

// Turns servo to the given angle
void servoTurn(double angle) {
	servo.write(angle);
}

// Reads sensor data
bool readSensor() {
	int reading = 0;

	// Reads sensor data sensorRead times
	for (int i = 0; i < sensorRead; i++) {
		reading += analogRead(sensorPin);	// Data from IR is HIGH when no signal
	}
	// If reading is less than sensorReadThreshold return true
	if (reading < sensorReadThreshold)
		return true;
	else
		return false;
}


