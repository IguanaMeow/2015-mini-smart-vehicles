
#include <Servo.h>
Servo esc;
Servo servo;

int throttle = 1500; //
int angle = 90;
void setup() { 
  esc.attach(9);
  servo.attach(8);
  Serial.begin(9600);  
}

void loop() {
   esc.writeMicroseconds(throttle); 
   servo.write(angle);
   delay(10);
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    switch(inChar){
      case 'f': throttle= 1700; break;
      case 'b': throttle= 1200;break;
      case 'n': throttle= 1500;break;
      case 'l': angle = 65;break;
      case 'r': angle = 115;break;
      case 'm': angle = 90;break;
      case 's': throttle= 1500;angle = 90;break;
    } 
  }   
}
