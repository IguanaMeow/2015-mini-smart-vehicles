
#include <Servo.h>
Servo esc, servo;
int spd = 1500;
int str = 90;

void setup() { 
  esc.attach(9);
  servo.attach(8);
  Serial.begin(9600);  
}

void loop() {
   esc.writeMicroseconds(spd); 
   servo.write(str);
   Serial.println("Speed= "+String(spd)+", steering= "+String(str));
   delay(10);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    switch(inChar){
      case 'f':     // fast drive
        spd = 1700; 
        break;
      case 'd':     // drive
        spd = 1600;
        break;
      case 's':    // slow drive
        spd = 1580;
        break; 
      case 'n':     // nutral
        spd = 1500;
        break;
      case 'b':   // brake
        spd = 1000;
        break;
      case 'l':    // left turn
        str = 0;
        break; 
      case 'r':   // right turn
        str = 180;
        break;
    }
  }
}
