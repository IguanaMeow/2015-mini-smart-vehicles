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

/* Function to determine SPEED based on incoming bit-values */
void setSpeed(char value)
{
  switch(value)
  {
    
    // Neutral-mode, halts the vehicle:
    case 'a':
    
      control = 512;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10); 
      break;
     
    // Drive Backwards: 
    case 'b':
      
      control = 280;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10); 
      break;
    
    // Drive Forwards:
    case 'c':
    
      control = 580;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10);  
      break;
    
    // Neutral-mode as default value:
    default:
      
      control = 512;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10);    
  }
}

/* Function to determine ANGLE based on incoming bit-values */ 
void setAngle(char value)
{
  switch(value)
  {
    
    // Go left:
    case 'd':
    
      degree = 35;
      myservo.write(degree);
      Serial.println(value);
      delay(10);
      break;
    
    // Go Right:
    case 'e':
      
      degree = 100;
      myservo.write(degree);
      Serial.println(value);
      delay(10);
      break;
    
    // Go Forwards:
    case 'f':
    
      degree = 70;
      myservo.write(degree); 
      Serial.println(value);
      delay(10);
      break;
    
    // Default--value as forwards: 
    default:

      degree = 70;
      myservo.write(degree);
      delay(10);    
  }
}

void loop() {
  
  /* Reads the buffer continously with each loop-iteration */
  while(Serial.available() > 0)
  {
      // Assigns the current value in the buffer to the variable 'inByte': 
      char inByte = Serial.read();
      
   
   
           // Determine SPEED by use of Auxiliary function 'setSpeed':
           if (inByte == 97)
          {
         	setSpeed(inByte);  
          }

          if (inByte == 98)
          {
         	setSpeed(inByte); 
          }

          if (inByte == 99)
          {
	  	setSpeed(inByte);       
          }
   
   
   
   
   
      // Determine ANGLE by use of Auxiliary function 'setAngle':    
      if (inByte == 100)
      {
         setAngle(inByte);       
      }
      
      if (inByte == 101)
      {
         setAngle(inByte);       
      }
      
      if (inByte == 102)
      {
         setAngle(inByte);       
      }
      
      
  }

  // Flush the buffer: 
  Serial.flush();
}
