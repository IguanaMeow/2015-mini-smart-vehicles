
// Using Arduinos Servo library:
#include <Servo.h>

int pin = 8;
unsigned long duration;
boolean flag = false;

/* Variables for regulating servo and ESC */
Servo myservo;
Servo esc;

String inputString;

int throttle = 0; 
int control = 512;
int degree = 70;  




void setup()
{
  pinMode(pin, INPUT);

  // Attach ESC signal wire to pin 9:
  esc.attach(9);
  myservo.attach(10);

  // Initialize Serial-communication with 115200 baud: 
  Serial.begin(115200);

  // Initialize ESC value to neutral mode:
  control = 512;
  throttle = map(control, 0, 1023, 1000, 2000); 
   
  // Write in microseconds (1500 = neutral mode) to ESC: 
  esc.writeMicroseconds(throttle);

  delay(10);
   
}
 
void loop(){
  
  override();
  if(flag == false){
  serialRead();
  }
  else {
    writeToEsc(512);
    Serial.println("test");
    }
}

void writeToEsc(int value){
  throttle = map(value, 0, 1023, 1000, 2000); 
  esc.writeMicroseconds(throttle); 
}

void serialRead(){
  while(Serial.available() > 0)
  {
         char inByte = Serial.read();
         if (inByte == ' ') break;
         inputString += inByte; //makes the string readString
         delay(2);
  }
  
  if(inputString.length() > 1)
  {
        degree = inputString.toInt();
        myservo.write(degree); 
	//("Turning Right");
	control = 585;
	throttle = map(control, 0, 1023, 1000, 2000); 
	esc.writeMicroseconds(throttle);  
	delay(10);
  }
        inputString = "";
        
	// Flush the buffer:
        Serial.flush();
        delay(2);
}
void override(){
  
    duration = pulseIn(pin, HIGH, 10000);

  if(duration <= 1400){
    flag = true;
    //Serial.println("Brake");
  }
  else if(duration >= 1600){
    flag = true;
    //Serial.println("Forward");
  }
  
  else flag = false;
}


