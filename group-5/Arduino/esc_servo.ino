#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#include <Servo.h>

#define EncoderPinA 2   // Encoder Pin A pin 2 and pin 3 are inturrpt pins
#define EncoderPinB 5
Servo esc, servo;
int spd = 1500;
int str = 1500;

//Initialize Variables
long counts = 0; //counts the encoder counts. The encoder has ~233counts/rev

void setup() { 
  pinMode(EncoderPinB, INPUT);  
  digitalWrite(EncoderPinA, LOW); //initialize Pin States
  digitalWrite(EncoderPinB, LOW);
  attachInterrupt(0, readEncoder, CHANGE); //attach interrupt to PIN 2 
  
  esc.attach(9);
  servo.attach(8);
  Serial.begin(9600);  
}

void loop() {
   esc.writeMicroseconds(spd); 
   servo.writeMicroseconds(str);
   Serial.println("Speed= "+String(spd)+", steering= "+String(str));
   Serial.println(counts);
   delay(10);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    switch(inChar){
      case 'f':     // fast drive
        spd = 1650; 
        break;
      case 'd':     // drive
        spd = 1560;
        break;
      case 's':    // slow drive
        spd = 1550;
        break; 
      case 'n':     // nutral
        spd = 1500;
        break;
      case 'b':   // brake
        spd = 1000;
        break;
      case 'l':    // left turn
        str = 2000;
        break;
      case 'm':   // right turn
        str = 1500;
        break; 
      case 'r':   // right turn
        str = 1000;
        break;
    }
  }
}

void readEncoder() //this function is triggered by the encoder CHANGE, and increments the encoder counter
{ 
  if(digitalRead(EncoderPinB) == digitalRead(EncoderPinA) )
  {
    counts = counts-1; //you may need to redefine positive and negative directions
  }
  else
  {
    counts = counts+1;
  }
}
