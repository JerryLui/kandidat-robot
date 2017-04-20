// Ultrasound Constants
const int maxDistance = 100;		// Maximum distance in cm
const int minDistance = 10;

const int trigPin = 2;					// Respective pins for ultrasound
const int echoPin = 3;

void setup() {
	Serial.begin(9600);
	// Ultrasound setup
	pinMode(trigPin, OUTPUT);pinMode(echoPin, INPUT);
}

void loop() {
	delay(1000);
	Serial.print("Distance: ");
	Serial.println(getDistance());
}

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

