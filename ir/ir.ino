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
<<<<<<< HEAD
SoftwareSerial mySerial(A3, 10);
SoftwareSerial myFake(A1, A2);
=======
//SoftwareSerial mySerial(A3,10);
//SoftwareSerial myFake(A1,A2);
>>>>>>> 346719b8df09a539b35f2d9b1d6d59587c86709b

const int spanSizeThreshold = 3;
const int sensorRead = 20;				// Times to read the sensor data
const int sensorReadThreshold = analogMax * sensorRead * 0.64;	// Upperbound for ir 0.64

bool sensor = false;

// Directions
enum Direction {
  LEFT, RIGHT, STRAIGHT, BACK, UNKNOWN
};

void setup() {
<<<<<<< HEAD
  Serial.begin(9600);
  // Servo Setup
  //pinMode(sensorPin, INPUT_PULLUP);
  mySerial.begin(2400);
  myFake.begin(2400);
  servo.attach(servoPin);
  myFake.listen();
=======
	Serial.begin(9600);
	// Servo Setup
	//pinMode(sensorPin, INPUT_PULLUP);
  //mySerial.begin(2400);
  //myFake.begin(2400);
	servo.attach(servoPin);
  //myFake.listen();
  Wire.begin(1);
  Wire.onReceive(recieveEvent);
>>>>>>> 346719b8df09a539b35f2d9b1d6d59587c86709b
}

void loop() {
  debugPrintServoScan();
}

/*~~~~~~~~~~ Servo Functions ~~~~~~~~~~*/
// Debug Servo
void debugPrintServoScan() {
  // Serial.print("Angle (Original Method): ");
  // Serial.println(servoScan());
  Serial.print("Angle (Average Method): ");
  Serial.println(averageServoScan());
}

// Scans 0-180 degrees in front of the robot, returns angle for which there is an signal
int servoScan() {
  bool clockwiseReadings [maxAngle];
  bool counterClockwiseReadings [maxAngle];
  bool combinedReadings [maxAngle];

  // Sweeps the servo from minAngle to maxAngle while collecting the reading
  for (int i = minAngle; i < maxAngle; i++) {
    servoTurn(i / servoResolution);
    clockwiseReadings[i] = readSensor();
  }
  // does another sweep backwards
  for (int i = maxAngle; i > minAngle; i--) {
    servoTurn(i / servoResolution);
    counterClockwiseReadings[i] = readSensor();
  }

  int spanSize = 0;
  int spanEnd = 0;
  int maxSpanSize = 0;

  // Compares the readings from the two sweeps and finds the largest span
  for (int i = minAngle; i < maxAngle; i++) {
    // Sets combinedReadings to true if both sweeeps show an positive signal at i
    if (clockwiseReadings[i] && counterClockwiseReadings[i])
      combinedReadings[i] = true;

    // Increases the spanSize for each run
    if (combinedReadings[i]) {
      spanSize++;
    } else if (spanSize != 0) {
      if (spanSize > maxSpanSize) {
        maxSpanSize = spanSize;
        spanEnd = i - 1;
      }
      spanSize = 0;
    }
  }
  Serial.print("maxSpanSize: ");
  Serial.println(maxSpanSize);
  Serial.print("spanEnd: ");
  Serial.println(spanEnd);

  // Returns the angle for which there's a signal, returns -1 if nothing found
  if (maxSpanSize < spanSizeThreshold) {
    return -1;
  } else {
    return (spanEnd - maxSpanSize / 2) / servoResolution;
  }
}

// An alternative method using average of positive signals
int averageServoScan() {
  // Stores the average of signal position
  int firstSweepAverage = averageSweep(minAngle, maxAngle);
  //Serial.println(firstSweepAverage);

  // Stores the average pos from second sweep
  int secondSweepAverage = averageSweep(maxAngle, minAngle);
  Serial.println(secondSweepAverage);

  // Returns -1 if difference between the result from first & second sweep are
  // larger than 5 degrees. Else returns the angle at which a signal was found.
  if (abs(secondSweepAverage - firstSweepAverage) > 5 * servoResolution)
    return -1;
  else
    return ((firstSweepAverage + secondSweepAverage) / 2) / servoResolution;
}

// Stupid ethod to chose between incremental & decremental search
int averageSweep(int startAngle, int endAngle) {
  if (startAngle < endAngle)
    return averageIncrementalSweep(startAngle, endAngle);
  else
    return averageDecrementalSweep(startAngle, endAngle);
}

// Average incremental sweep
int averageIncrementalSweep(int startAngle, int endAngle) {
  int spanSize = 0;
  int total = 0;
  int elements = 0;

  // Method only saves elements with a spansize > 2
  while (startAngle < endAngle) {
    servoTurn(startAngle / servoResolution);
    // If a positive signal is recieved
    if (readSensor()) {
      spanSize++;	// Increase the current span
      if (spanSize > 1) {		// If current spanSize > 1
        total += startAngle;	// Add current angle to total
        elements++;						// Increment nmbr of elements
      }
    } else { 	// I no signals are detected
      if (spanSize > 1) {
        total -= startAngle - 1;
        elements--;
      }
      spanSize = 0;
    }
    startAngle++;
  }
  return total / elements;
}

// Average decremental sweep
int averageDecrementalSweep(int startAngle, int endAngle) {
  int spanSize = 0;
  int total = 0;
  int elements = 0;

  while (startAngle > endAngle) {
    servoTurn(startAngle / servoResolution);
    // If a positive signal is recieved
    if (readSensor()) {
      spanSize++;	// Increase the current span
      if (spanSize > 1) {		// If current spanSize > 1
        total += startAngle;	// Add current angle to total
        elements++;						// Increment nmbr of elements
      }
    } else { 	// I no signals are detected
      if (spanSize > 1) {
        total -= startAngle + 1;
        elements--;
      }
      spanSize = 0;
    }
    startAngle--;
  }
  return total / elements;
}

// Turns servo to the given angle
void servoTurn(double angle) {
  servo.write(angle);
}

// Reads sensor data
bool readSensor() {
  int reading = 0;

  double lastT = micros();

  while (!mySerial.isListening() || !mySerial.available() && micros() - lastT < 9000) mySerial.listen();

  if (micros() - lastT < 9000) { //mySerial.available()&&
    reading = mySerial.read();
  }

  while (!myFake.isListening()) myFake.listen();

  return reading > 0;

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

