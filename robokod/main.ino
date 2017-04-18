#include <Event.h>
#include <Timer.h>
#include <Servo.h>

// Searching Variables
Servo myservo;
#define nrOfReadings 30   		// Amount of times the code is read at same position
#define sensorThreshold 800		// Threshold value for input signal
#define sense 17          		// 10 work with nrofr 20
#define minAngle 0						// Minimum angle for servo
#define maxAngle 360					// Maximum angle for servo
#define accuracy 360					// ??

// Moving Variables for Step Motors
#define dir1 4
#define steg1 3
#define enable1 5

#define dir2 7
#define steg2 6
#define enable2 2

#define startDelay 2800
#define speedDelay 650
#define inc 7
#define brake 130


unsigned int delayT;
unsigned int Adelay;
int FW = 1000;
int TR = 200;//768/4; //768 är typ ett helt varv

void setup() {
  Serial.begin(9600);      			// open the serial port at 9600 bps:   

  pinMode(enable1,OUTPUT);pinMode(steg1,OUTPUT);pinMode(dir1,OUTPUT);
  pinMode(enable2,OUTPUT);pinMode(steg2,OUTPUT);pinMode(dir2,OUTPUT);
  
  digitalWrite(enable1,LOW);digitalWrite(dir1,HIGH);
  digitalWrite(enable2,LOW);digitalWrite(dir2,LOW);

  pinMode(A0, INPUT_PULLUP);    // Set 'A0' as input from the sensor
  myservo.attach(13); 

  Adelay = micros();
}

void loop() {
  int x = scan();
  Serial.println(x);
  if (x > 99) { 
		walk(1,abs(x-85));
	}
  else if (x<80 && x>0) { 
		walk(3,abs(95-x));
	}
  else if (x != -1) {
		stepServo(x);
		delay(1000);
		while (readSensor()) { // && ultraljud.read not too far
			walk(2, 200);
		}
	}
}

/*~~~~~~~~~ Search Functions ~~~~~*/
int scan() {
  bool readings [accuracy];
  bool readings2 [accuracy];
  int maxSpan = 0;
  int startSpan = 0;
  int maxSpan2 = 0;
  int startSpan2 = 0;
  int j;
/*
  for (int i = 0; i<accuracy; i++) {
    readings[i] = false;
    readings2[i] = false; 
  }
  */
  for (int i = minAngle; i < maxAngle; i++) {  //Cc sweep
    stepServo(i/(accuracy/180));
    readings[i] = readSensor();
    Serial.println(readings[i]);
  }
  for (int i = maxAngle; i > minAngle; i--) {  //C sweep
    stepServo(i/(accuracy/180));
    readings2[i] = readSensor();
    Serial.println(readings[i]);
  }
  
  for (int i = minAngle; i < maxAngle; i++) { //Search for span 
    if (readings[i] && readings[i+1]) {           //Cc
      for (j = i; readings[j]; j++) {}
      if (maxSpan < j-i) {
        maxSpan = j-i;
        startSpan = i;
      }
    }
    
    if (readings2[i] && readings2[i+1]) { //C
      for (j = i; readings2[j]; j++) {}
      if (maxSpan2 < j-i) {
        maxSpan2 = j-i;
        startSpan2 = i;
      }
    }
  }
  if (maxSpan+maxSpan2 < 10) {
    return -1;
  }else{
  return(((startSpan +(maxSpan/2))/2)+((startSpan2 +(maxSpan2/2))/2))/(accuracy/180);
  }
}

void stepServo(double stepAngle) {
  myservo.write(stepAngle);
  //delay(1);
}

bool readSensor() {
  double sensorData = 0;
  int reading = 0;
  
  for (int i = 0; i < nrOfReadings; i++) {
    sensorData = analogRead(A0);
    //Serial.println(sensorData);
      if (sensorData < sensorThreshold) {
        reading=reading + 3;
      }else{
        reading--;
        //reading = reading - 2;
      }
  } // end for
  if (reading > sense) {
    //Serial.println("true");
    return true;
  }else{
    //Serial.println("false");
    return false;
  }
}

/*~~~~~ Motor Functions ~~~~~~*/
void walk(int gear, int steps) {
  gears(gear);
  for (int i = 0; i<steps; i++) {
    delayT = soft(i,steps,delayT);
    step(delayT);
  }
  delay(50);
}

//Flyttar båda motorerna 1 steg framåt, riktning bestämms av funktionen gears
void step(double delayT) { 
  delayMicroseconds(delayT-(micros()-Adelay));
  digitalWrite(steg1,HIGH);digitalWrite(steg2,HIGH);
  Adelay = micros();

  delayMicroseconds(delayT-(micros()-Adelay));
  digitalWrite(steg1,LOW);digitalWrite(steg2,LOW);
  Adelay = micros();
}

double soft(int step, int limit, double current) {
  if (step == 0) 
		current = startDelay;
  if (step <=((startDelay-speedDelay)/inc) && current > speedDelay)
		current = current - inc;
  if (step>(limit-brake)) // ELIF?
		current = current + inc;
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
  //Serial.println("left");
  digitalWrite(dir1,LOW);
  digitalWrite(dir2,LOW);
}
void straight() {
  //Serial.println("straight");
  digitalWrite(dir1,HIGH);
  digitalWrite(dir2,LOW);
}
void right() {
  //Serial.println("right");
  digitalWrite(dir1,HIGH);
  digitalWrite(dir2,HIGH);
}

