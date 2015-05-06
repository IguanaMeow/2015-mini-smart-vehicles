#include <Servo.h>
Servo esc, servo;
int spd = 1500;
int str = 1500;
String input;

void setup() { 
  esc.attach(9);
  servo.attach(8);
  Serial.begin(9600); 
  
 Serial.println(input.toInt()); 
}

void loop() {
   esc.writeMicroseconds(spd); 
   servo.writeMicroseconds(str);
   Serial.println("Speed= "+String(spd)+", steering= "+String(str));
   delay(10);
  
}

void serialEvent() {
  while (Serial.available()) {
    input = Serial.readStringUntil(',');
    Serial.println(input);
    if(input.length()==11){
       spd = (input.substring(2, 6)).toInt();
       str = (input.substring(7, 11)).toInt();
    }
  }
}
