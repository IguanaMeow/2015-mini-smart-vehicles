
#include <Servo.h>
Servo esc;
Servo servo;
String inputString;
String c;
String decodedS;
//pin define
int sensorPin = A0;    // select the input pin for the potentiometer
int throttle = 0; //
int steer=0;
int control =512;
void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(sensorPin, INPUT);
  esc.attach(9);
  servo.attach(10);
  Serial.begin(9600);

}

void loop() {
  // read the value from the sensor:
  // control = analogRead(sensorPin); //uncomment when controling from potentiometer 
  throttle = map(control, 0, 1023, 1000, 2000); 
  //steer= map(control, 0, 1023, 1000, 2000); 
  // display value and send to servo
  Serial.println(throttle);
  esc.writeMicroseconds(throttle); 
  servo.writeMicroseconds(steer);
  delay(10);
}

void serialEvent() {
  if(Serial.available()) {
    c = Serial.readStringUntil(',');
    decodedS=decodedNetstring(c);
    delay(2);  //slow looping to allow buffer to fill with next character


    if (decodedS.length() >0) {
      Serial.println(decodedS);  //so you can see the captured string 
      control = decodedS.substring(0, 2).toInt();  //convert a substring readString into a number
      steer= decodedS.substring(3, 5).toInt(); //convert a substring readString into a number
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

