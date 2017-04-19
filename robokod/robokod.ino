#include <Event.h>
#include <Timer.h>
#include <Servo.h>
#include <SoftwareSerial.h>

//searching var
Servo myservo;
SoftwareSerial mySerial(8,10);
SoftwareSerial myFake(9,10);
#define nrOfReadings 3           //20 works fine
#define sensorLimit 80               //limit for what should be considered a high and low value for the IR-sensor
#define sense 2                      //10 work with nrofr 20
#define resulotion 2
#define minAngle 0                    //The angle at which the servo strats its search sweep
#define maxAngle 180*resulotion       //The angle at which the servo stops,
#define accuracy 180*resulotion

//Moving var
#define dir1 4
#define steg1 3
#define enable1 5

#define dir2 7
#define steg2 6
#define enable2 2

#define startDelay 2800   //sets the speed or acctually the delay between steps when the robot begins to move
#define speedDelay 650    //sets the speed or acctually the delay between steps
#define inc 7             //to achive smooth acceleration and breaking the delay between each step gradually increases or decreases near the beginning and end of each move
#define brake 130         //sets the distance from the end of a move at which the delay starts incrementing with "inc" microseconds each time

unsigned int delayT;
unsigned int Adelay;

void setup() {
	Serial.begin(57600);      // open the serial port at 9600 bps:
	mySerial.begin(2400);
	myFake.begin(2400);

	pinMode(enable1,OUTPUT);pinMode(steg1,OUTPUT);pinMode(dir1,OUTPUT);
	pinMode(enable2,OUTPUT);pinMode(steg2,OUTPUT);pinMode(dir2,OUTPUT);

	digitalWrite(enable1,LOW);digitalWrite(dir1,HIGH);
	digitalWrite(enable2,LOW);digitalWrite(dir2,LOW);

	pinMode(A5, INPUT_PULLUP);    // Set 'A0' as input from the sensor
	myservo.attach(13);           //defines that pin 11 is connected to the controll pin of a "servo"

	Adelay = micros();            //To get the delay between each step to be approximally equal the time it takes to run the code must be accounted for
}

void loop() {
	int x = scan();
	if ( x > 95) {
		walk(1,abs(x-90)*2);
	}
	else if (x < 85 && x>0) {
		walk(2,abs(90-x)*2);
	}
	else {
		walk(3,(10-abs(x-90))*80);
	}
}
/*~~~~~~~~~~ Move Functions ~~~~~~~~~~~*/
//This function uses varibales defined in the begin to perform searchsweep with the servo, it returns a int representing the middle of a brigthspot
//in other words the returned int represents the angle to the "Lighthouse" in relation to the robot
int scan() {
	bool readings [accuracy];       //To avoid false readings the sweep is done both ways
	int maxSpan = 0;                //first counterclockwise and then clockwise
	int startSpan = 0;
	bool readings2 [accuracy];
	int maxSpan2 = 0;
	int startSpan2 = 0;
	int j;

	for (int i = 0; i<accuracy; i++) { //This loop makes sure there are no lingering values in the arrays
		readings[i] = false;
		readings2[i] = false;
	}

	for ( int i = minAngle; i < maxAngle; i++ ) {  //Counterclockwise searchsweep
		stepServo(i);                               //Moves the servo to the next position
		readings[i] = readSensor();                 //saves the current value in its position in the array
	}
	for ( int i = maxAngle; i > minAngle; i-- ) {  //Clockwise sweep
		stepServo(i);
		readings2[i] = readSensor();
	}

	for ( int i = minAngle; i < maxAngle; i++ ) {  //Search for span of "true" values
		if (readings[i] && readings[i+1]) {           //Counterclockwise, if two adjacent readings are both "true" then
			for ( j = i; readings[j]; j++) {}         //this loop starts counting and only stops when it reaches a "false" value in the array
			if (maxSpan < j-i) {                        //if the longest span of "true" values are shorter then the spann just scanned the values are updated
				maxSpan = j-i;                          //the length of the span
				startSpan = i;                          //the beginning of the span
			}
		}

		if (readings2[i] && readings2[i+1]) {         //same as above but for the clockwise sweep
			for ( j = i; readings2[j]; j++) {}
			if (maxSpan2 < j-i) {
				maxSpan2 = j-i;
				startSpan2 = i;
			}
		}
		//if (startSpan == i) i = i + maxSpan;
		//If a new span of "true" values has been found this line makes the search resume at the end of the span
	}
	//Serial.println((((startSpan +(maxSpan/2))/2)+((startSpan2 +(maxSpan2/2))/2))/(540/180));
	if (maxSpan+maxSpan2 < 3) {
		return -1;
	} else {
		return(((startSpan +(maxSpan/2))/2)+((startSpan2 +(maxSpan2/2))/2))/(540/180);
	}
}

void stepServo(int stepAngle) {
	myservo.write((double)stepAngle/(accuracy/180));
	delay(2);
}

bool readSensor(){
	double time = 0;
	bool signal = false;
	mySerial.listen();
	time = micros();
	while(mySerial.available() == 0 && (micros()-time)<9000) {
		if(mySerial.available()){
			signal = true;
		}
		myFake.listen();
	}
	return signal;
}

/*~~~~~~~~~~ Move Function ~~~~~~~~~~*/
void walk(int dir, int steps) {
	gears(dir);
	for (int i = 0; i<steps; i++) {
		delayT = soft(i,steps,delayT);
		step(delayT);
	}
	delay(50);
}

// Tells motors to turn one step, direction given by the function gears
void step(double delayT) {
	delayMicroseconds(delayT-(micros()-Adelay));
	digitalWrite(steg1,HIGH);digitalWrite(steg2,HIGH);
	Adelay = micros();

	delayMicroseconds(delayT-(micros()-Adelay));
	digitalWrite(steg1,LOW);digitalWrite(steg2,LOW);
	Adelay = micros();
}

double soft(int steps, int limit, double current) {
	if (steps == 0) {
		current = startDelay;
	}
	else if (steps <=((startDelay-speedDelay)/inc) && current > speedDelay) {
		current = current - inc;
	}
	else if (steps>(limit-brake)) {
		current = current + inc;
	}
	return current;
}

void gears(int dir) {
	switch(dir) {
		case 1:
			left();
			break;
		case 2:
			straight();
			break;
		case 3:
			right();
			break;
	}
}
void left() {
	digitalWrite(dir1,LOW);
	digitalWrite(dir2,LOW);
}
void straight() {
	digitalWrite(dir1,HIGH);
	digitalWrite(dir2,LOW);
}
void right() {
	digitalWrite(dir1,HIGH);
	digitalWrite(dir2,HIGH);
}
