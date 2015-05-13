/*
This code receives an encoded Netstring message and 
decodes it acording to Netsring protocols. Then the 
decoded data is transferred to two servos, one to steer 
the car and another to move the car at a given speed. 

Author: Neda Ashrafi Amiri
*/
#include <Servo.h>
Servo esc; //initialize esc type Servo
Servo servo; //initialize servo type Servo
String c; //initialize encoded Netsrting received via Serial COM
String decodedS; // initialize decoded String
int throttle = 0;
int steer=60; 
int control =512;

void setup() {
  esc.attach(9);//attach esc to pin 9 on Mega
  servo.attach(10);// attach servo to pin 10 Mega
  Serial.begin(9600);
}

void loop() {
  throttle = map(control, 0, 1023, 1000, 2000); 
  Serial.println(control);
  esc.write(throttle); 
  servo.write(steer);
  delay(10);
}

void serialEvent() {
  if(Serial.available()) {
    c = Serial.readStringUntil(',');
    decodedS=decodedNetstring(c);
    delay(2);  //slow looping to allow buffer to fill with next character


    if (decodedS.length() >0) {
      Serial.println(decodedS);  //so you can see the captured string 
      control = decodedS.substring(0, 3).toInt();  //convert a substring readString into a number
      steer= decodedS.substring(3, 6).toInt(); //convert a substring readString into a number
    }
    decodedS="";
    c="";
  }
}



String decodedNetstring(String netstring){
  if (netstring.length() < 3) return "error"; //if the Netstring is less than 3 characters, it's either an invalid one or contains an empty string
  int semicolonIndex = netstring.indexOf(':');
  if (semicolonIndex < 0) return "error"; // if there's no semicolon, then it's an invalid Netstring
  String parsedDigits = netstring.substring(0, semicolonIndex); //parse until the semicolon. Those should be the control digits
  int controlDigits = parsedDigits.toInt();
  if (controlDigits < 1) return "error"; //if the control digit is smaller than 1, then it's either not a digit or the Netstring is invalid
  String command = netstring.substring(semicolonIndex+1); //parse after the semicolon until the end of the string
  if (!command.length()) return "error"; // if it's an empty string, return "error"
  //if (command.substring(command.length() -1) == ",") command.remove(command.length()-1); //if last character is a comma, remove it
  if (command.length() != controlDigits) return "error"; //if string's length isn't equal with the control digits, it's an invalid Netstring
  return command;
}
