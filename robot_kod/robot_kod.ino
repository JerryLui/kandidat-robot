#include <Event.h>
#include <Timer.h>
#include <Servo.h>

//searching var
Servo myservo;
#define nrOfReadings 30    //20 works fine
#define sensorLimit 800
#define sense 17           //10 work with nrofr 20
#define minAngle 0
#define maxAngle 180 
#define accuracy 180

//Moving var
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
unsigned int aDelay;
int FW = 1000;
int TR = 200;//768/4; //768 är typ ett helt varv

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:   

  pinMode(enable1,OUTPUT);pinMode(steg1,OUTPUT);pinMode(dir1,OUTPUT);
  pinMode(enable2,OUTPUT);pinMode(steg2,OUTPUT);pinMode(dir2,OUTPUT);
  
  digitalWrite(enable1,LOW);digitalWrite(dir1,HIGH);
  digitalWrite(enable2,LOW);digitalWrite(dir2,LOW);

  pinMode(A0, INPUT_PULLUP);    // Set 'A0' as input from the sensor
  myservo.attach(11); 

  aDelay = micros();
}

// Main loop
void loop() {
  int x = scan();
/*  Serial.print("Vinkel: ");*/
  /*Serial.println(x);*/
  if (x >= 100) { 
		walk(1,abs(x-85));
	}
  else if (x < 80 && x>0) { 
		walk(3,abs(95-x));
	}
  else if (x<0) {
	}
  else { // Walks straight
		stepServo(x);
		while (readSensor()) {
			walk(2, 200);
		}
	}
  /* 
  walk(2, FW);
  walk(1, TR);
  */
  
  //stepServo(90);
}
/////////////////////////////////////////// Search functions
int scan() {
  bool readings [accuracy];
  bool readings2 [accuracy];
  int maxSpan = 0;
  int startSpan = 0;
  int maxSpan2 = 0;
  int startSpan2 = 0;
  int j;

	// Initial Sweep
  for (int i = minAngle; i < maxAngle; i++) {  //Cc sweep
    stepServo(i);
    readings[i] = readSensor();
    //Serial.println(readings[i]);
  }

  for (int i = maxAngle; i > minAngle; i--) {  //C sweep
    stepServo(i);
    readings2[i] = readSensor();
    //Serial.println(readings[i]);
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
  } else {
  return(((startSpan +(maxSpan/2))/2)+((startSpan2 +(maxSpan2/2))/2))
  }
}

// Turns the servo by given input: stepAngle
void stepServo(double stepAngle) {
  myservo.write(stepAngle);
}

// Reads sensor from analogRead times nrOfReadings
// If reading returns above threshold value (sense) return true, else false
bool readSensor() {
  double sensorData = 0;
  int reading = 0;
	
  // Reads sesonr nrOfReadings times and sets a reading value
  for (int i = 0; i < nrOfReadings; i++) {
    sensorData = analogRead(A0);
      if (sensorData < sensorLimit) {
        reading = reading + 3;
      } else {
        reading--;
      }
  } // end for
  if (reading > sense) {
    return true;
  } else {
    return false;
  }
}

/*~~~~~~~~~~~~ Move functions ~~~~~~~~~~~~*/
// Walks towoards the direction by nmbr of steps 
void walk(int dir, int steps) {
  direction(dir);
  for (int i = 0; i<steps; i++) {
    delayT = soft(i,steps,delayT);
    step(delayT);
  }
  delay(50);
}

// Flyttar båda motorerna 1 steg framåt, riktning bestämms av funktionen direction
void step(double delayT) { delayMicroseconds(delayT-(micros()-aDelay));
  digitalWrite(steg1,HIGH);
	digitalWrite(steg2,HIGH);
  aDelay = micros();

  delayMicroseconds(delayT-(micros()-aDelay));
  digitalWrite(steg1,LOW);
	digitalWrite(steg2,LOW);
  aDelay = micros();
}

double soft(int steps, int limit, double current) {
  if (steps == 0) current = startDelay;
  if (steps <=((startDelay-speedDelay)/inc) && current > speedDelay) current = current - inc;
  if (steps>(limit-brake)) current = current + inc;
  return current;
}

void direction(int dir) {
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

