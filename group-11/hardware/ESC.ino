
#include <Servo.h>
#define TIME 500
#define CIRCUMFERENCE 20.4

int counter = 0;
double distanceMeasured = 0;


Servo servoMain; // Define our Servo
Servo esc;
String inputString;
String servoString;

int throttle = 0;
int control =512;

int degree = 0;

void setup() {
  
  esc.attach(9);
  servoMain.attach(10); // servo on digital pin 10
  Serial.begin(9600);
  attachInterrupt(1, distance, CHANGE);
}

void loop() {
  
  Serial.println(counter);
  if(counter >= 8) {
    distanceMeasured += 20.4;
    counter = 0;
   }
  // read the value from the sensor:
 // control = analogRead(sensorPin); //uncomment when controling from potentiometer 
  throttle = map(control, 0, 1023, 1000, 2000); 
  servoMain.write(degree);  // Turn Servo Left to 45 degrees
     
  // display value and send to servo
  //Serial.print(distanceMeasured);
  //Serial.println(" cm");
  
  esc.writeMicroseconds(throttle); 

  delay(10);
}

void serialEvent() {
  while (Serial.available()) {
     while (Serial.available()) {
       
    char c = Serial.read();  //gets one byte from serial buffer
    if(c == 32 ) break;
    inputString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }
    while (Serial.available()){
    char c = Serial.read();
    servoString += c;
    delay(2);
    } 

  if (inputString.length() >0) {
    Serial.println(inputString);  //so you can see the captured string 
    control = inputString.toInt();  //convert readString into a number      
  }
  
  if(servoString.length() > 0){
      degree = servoString.toInt();
    }
  inputString="";
  servoString = "";
  }
}

void distance()
{
  counter++;
}
