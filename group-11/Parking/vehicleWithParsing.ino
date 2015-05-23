#include <Servo.h>

Servo myservo;
Servo esc;

String input;

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

void setSpeed(char value)
{
  switch(value)
  {
    
    case 's':
    
      control = 512;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10); 
      break;
      
    case 'r':
      
      control = 280;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10); 
      break;
    
    case 'f':
    
      control = 580;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10);  
      break;
    
    default:
      
      control = 512;
      throttle = map(control, 0, 1023, 1000, 2000); 
      esc.writeMicroseconds(throttle);
      delay(10);    
  }
}

void setAngle(char value)
{
  switch(value)
  {
    
    case 'l':
    
      degree = 35;
      myservo.write(degree);
      Serial.println(value);
      delay(10);
      break;
      
    case 'r':
      
      degree = 100;
      myservo.write(degree);
      Serial.println(value);
      delay(10);
      break;
    
    case 'f':
    
      degree = 70;
      myservo.write(degree); 
      Serial.println(value);
      delay(10);
      break;
    
    default:

      degree = 70;
      myservo.write(degree);
      delay(10);    
  }
}

void checkIndex(String input)
{
  int index = input.indexOf(',');
  
  switch(index)
  {
    case -1:
    
      break;
      
    case 0:
      
      setAngle(input.charAt(1));
      setSpeed(input.charAt(3));
      break;
    
    case 1:

      setAngle(input.charAt(2));
      setSpeed(input.charAt(0));
      break;
  
    case 2:

      setAngle(input.charAt(3));
      setSpeed(input.charAt(1));
      break; 
   
    case 3:
    
      setAngle(input.charAt(0));
      setSpeed(input.charAt(2));
      break;     
  }
}

void loop() {
  
  while(Serial.available() > 0)
  {
      char inByte = Serial.read();
      input += inByte;
      
      if(input.length() > 4)
      {
        input = "";
      }
      
      else if(input.length() == 4)
      {
        checkIndex(input);
        input = "";
      }
      
      delay(2);
  }
}
