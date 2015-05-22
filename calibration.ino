//Code for calibrating the ESC
//Upload code, open serial and follow the instructions
#include <Servo.h>
#include <ctype.h>
Servo esc;
int neutral = 1500;
int maximum = 2000;
int minimum = 1000;
void setup() {
  esc.attach(9);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Hold in ESC until it blinks red and beeps. Let go and enter 1");
  
}

void loop() { 
  delay(10);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    switch(inChar){
      case '1': esc.writeMicroseconds(neutral); Serial.println("Press ESC once and enter 2");break;
      case '2': esc.writeMicroseconds(maximum); Serial.println("Press ESC once and enter 3");break;
      case '3': esc.writeMicroseconds(minimum); Serial.println("Press ESC once and enter 4");break;
      case '4': esc.writeMicroseconds(neutral); Serial.println("ESC calibrated! Well done, you get many potatoes!");break;
    } 
  }
}
