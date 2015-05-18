#include <Servo.h>

Servo myservo;
Servo esc;

int throttle = 0; 
int control = 512;
int degree = 70;

void setup() {
  
  Serial.begin(9600);
  esc.attach(9);
  myservo.attach(10);
  control = 512;
  throttle = map(control, 0, 1023, 1000, 2000);
  esc.writeMicroseconds(throttle);
  delay(100);
}

void loop() {
  
  if(Serial.available() > 0) 
  {
    
   char inByte = Serial.read();

   
   /* Determine angle and speed in relation to receiving bytes from Proxy */ 
   
           if (inByte == 48) 
           {
      	     degree = 100;
      	     myservo.write(degree); 
      	     //("Turning Right");
      	     control = 585;
      	     throttle = map(control, 0, 1023, 1000, 2000); 
      	     esc.writeMicroseconds(throttle);  
      	     delay(10);
           } 
           
           else if (inByte == 49) 
           {
      	     degree = 35;
      	     myservo.write(degree);
      	     //("Turning Left");
      	     control = 585;
      	     throttle = map(control, 0, 1023, 1000, 2000); 
      	     esc.writeMicroseconds(throttle);  
      	     delay(10); 
          
            }  
            
            else if (inByte == 50) 
            {
    	     degree = 70;
    	     myservo.write(degree); 
    	     //("Forwards");
    	     control = 585;
    	     throttle = map(control, 0, 1023, 1000, 2000); 
    	     esc.writeMicroseconds(throttle);  
    	     delay(10);
          
            } 
            
            else 
            {
             control = 585;
            }
  }
}
