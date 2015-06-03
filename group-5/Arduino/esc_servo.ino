#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#include <Servo.h>


Servo esc, servo;
int spd = 1500;
int str = 1500;


void setup() { 

  
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

