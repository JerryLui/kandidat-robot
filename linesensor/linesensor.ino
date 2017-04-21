#define lineThreshold 30
const int lineLeftPin = A1;
const int lineRightPin = A2;

enum Direction {
	LEFT, RIGHT, STRAIGHT, BACK, UNKNOWN
};

void setup() {
  Serial.begin(9600);
 
 	// Line Sensor Setup
  pinMode(lineLeftPin, INPUT);
  pinMode(lineRightPin, INPUT); 
}

void loop() {
	printData(getLineDirection());	
	delay(1000);
}

/*~~~~~~~~~~ Line Sensor Functions ~~~~~~~~~~*/
// Line Sensor Test Function
void printLineData(Direction dir) {
	switch (dir) {
		case LEFT:
			Serial.println("Left");
			break;
		case RIGHT:
			Serial.println("Right");
			break;
		case STRAIGHT:
			Serial.println("Straight");
			break;
		case BACK:
			Serial.println("Back");
			break;
		case UNKNOWN:
			Serial.println("Unknown");
			break;
	}
}

// Reads data from line sensors, returns direction to go towoards
Direction getLineDirection() {
	bool leftData = readLeftLineSensor();
	bool rightData = readRightLineSensor();

	if (!leftData && !rightData)
		return UNKNOWN;
	else if (leftData && !rightData)
		return RIGHT;
	else if (!leftData && rightData)
		return LEFT;
	else
		return STRAIGHT;
}
// Reads data from left line sensor, returns true if on a line
bool readLeftLineSensor() {
	return analogRead(lineLeftPin) > lineThreshold;
}

// Reads data from right line sensor, returns true if on a line
bool readRightLineSensor() {
	return analogRead(lineRightPin) > lineThreshold;
}





