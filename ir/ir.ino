#include <Servo.h>

// Servo Constants
Servo servo;
const int servoResolution = 2;		// Number of steps per servo degree
const int minAngle = 0;
const int maxAngle = 180*servoResolution;

// IR-sensor Constants
# define analogMax 1023
const int sensorPin = A0;
const int sensorThreshold = 80; 	// Threshold value for a read
const int spanSizeThreshold = 3;
const int sensorRead = 20;				// Times to read the sensor data
const int sensorReadThreshold = analogMax*sensorRead*0.10;	// Upperbound for ir

void setup() {
	Serial.begin(9600);
	pinMode(sensorPin, INPUT_PULLUP);
	servo.attach(13);
}

void loop() {
	int angle = servoScan();
	Serial.print(angle);
}

/*~~~~~~~~~~ Servo Functions ~~~~~~~~~~*/
// Scans 0-180 degrees in front of the robot, returns angle for which there is an signal
int servoScan() {
	bool clockwiseReadings [maxAngle];
	bool counterClockwiseReadings [maxAngle];
	bool combinedReadings [maxAngle];

		// Sweeps the servo from minAngle to maxAngle while collecting the reading
	for (int i = minAngle; i<maxAngle; i++) {
		servoTurnTo(i/servoResolution);
		clockwiseReadings[i] = readSensor();
	}
	// does another sweep backwards
	for (int i = maxAngle; i>minAngle; i--) {
		servoTurnTo(i/servoResolution);
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

// Turns servo to the given angle
void servoTurnTo(double angle) {
	servo.write(angle);
}

// Reads sensor data
bool readSensor() {
	int reading = 0;

	// Reads sensor data sensorRead times
	for (int i = 0; i < sensorRead; i++) {
		reading += analogRead(A5);	// Data from IR is HIGH when no signal
	}

	// If reading is less than sensorReadThreshold return true
	if (reading < sensorReadThreshold)
		return true;
	else
		return false;
}
