#include <Wire.h>

#define REAR_IR 0
#define FRONT_RIGHT_IR 2
#define REAR_RIGHT_IR 1
#define VoltsToCentimeters(volts) ((2914 / (volts + 5)) - 1)

int IR_front_right, IR_rear_right, IR_rear;

// this constant won't change.  It's the pin number
// of the sensor's output:
const int US1 = 7;
const int US2 = 6;
int sonarFront;
int sonarFrontRight;
int duration1;
int duration2;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(100);
}

void loop()
{
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

   String format = "";
   
     // IRT, IRB & IRR
  IR_front_right = IR_Distance(FRONT_RIGHT_IR);
  IR_rear_right = IR_Distance(REAR_RIGHT_IR);
  IR_rear = IR_Distance(REAR_IR);


    
      US_getRange(US1);
      US_getRange(US2); 

  duration1 = pulseIn(US1, HIGH);
  duration2 = pulseIn(US2, HIGH);

  // convert the time into a distance
  sonarFront = microsecondsToCentimeters(duration1);
  sonarFrontRight = microsecondsToCentimeters(duration2);
  
  // Creating temporary concatenation variables for each sensor
     String FUS = String(sonarFront);
     String FRUS = String(sonarFrontRight);
     String IRT = String(IR_front_right);
     String IRB = String(IR_rear_right);
     String IRR = String(IR_rear);

       
       format = FUS + " " + FRUS + " " + IRT + " " + IRB + " " + IRR + " ";
       
     
     Serial.println(format);  
  
  delay(10);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

// Auxiliary function to acquire IR distance: 
int IR_Distance(int IR_sensor){
  float sensorValue = 0;
  sensorValue = analogRead(IR_sensor);
  int distance = VoltsToCentimeters(sensorValue);
  
  if(distance < 40){
  return(distance);
  }
  return(-1);
}

// Auxiliary function to get US range:
int US_getRange(int pingPin) {

pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
    pinMode(pingPin, INPUT);
    
}
  
