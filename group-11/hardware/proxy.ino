
// Using Arduinos Servo library:
#include <Servo.h>

/* Variables for regulating servo and ESC */
Servo myservo;
Servo esc;

String inputString;

int throttle = 0; 
int control = 512;
int degree = 70;  




void setup()
{

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
 
void loop()

{

   

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



