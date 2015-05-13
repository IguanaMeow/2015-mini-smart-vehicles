#include <Servo.h>
int pin = 8;
unsigned long duration;
boolean flag = false;

Servo esc;

String inputString;


int throttle = 0;
int control =512;

void setup() {
  pinMode(pin, INPUT);
  esc.attach(9);
  Serial.begin(9600);
  throttle = map(control, 0, 1023, 1000, 2000); 
  esc.writeMicroseconds(throttle); 
}

void loop() {
  
  
  override();
  if(flag == false){
  writeToEsc(control);
  }
  else writeToEsc(512);

  
  
  //Serial.println(duration);
  delay(20);
}

void writeToEsc (int value){
  throttle = map(value, 0, 1023, 1000, 2000); 
  esc.writeMicroseconds(throttle); 
}

void override(){
  
    duration = pulseIn(pin, HIGH);

  if(duration <= 1500){
    flag = true;
    Serial.println("Brake");
  }
  else if(duration >= 1600){
    flag = true;
    //Serial.println("Forward");
  }
  
  else flag = false;
}

void serialEvent() {
  
     while (Serial.available()) {
       
    char c = Serial.read();  //gets one byte from serial buffer
    inputString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }
  
  if (inputString.length() >0) {
    Serial.println(inputString);  //so you can see the captured string 
    control = inputString.toInt();  //convert readString into a number      
  }
  
    inputString="";

}
