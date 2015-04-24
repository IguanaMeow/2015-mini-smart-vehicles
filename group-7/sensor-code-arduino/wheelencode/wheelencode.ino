/*
  Wheel encoder test. Each tick triggers the ISR
  and modifies the number of ticks depending on 
  direction of travel. Number of ticks is printed 
  to the serial monitor.
*/

const int encoderPinA = 19;
const int encoderPinB = 25;
volatile long encoderTicks = 0; 

void setup()
{
  Serial.begin(9600);
  pinMode(encoderPinA, INPUT);    
  pinMode(encoderPinB, INPUT);     
  attachInterrupt(4, WheelEncoderInterrupt, CHANGE);
}
 
void loop()
{
  Serial.println(encoderTicks);
  delay(100);
}
 
// Interrupt service routines for the wheel encoder
void WheelEncoderInterrupt()
{
   if(digitalRead(encoderPinA)){ 
   !(digitalRead(encoderPinB)) ? encoderTicks ++ : encoderTicks --;
   }
  else{
   !(digitalRead(encoderPinB)) ? encoderTicks -- :encoderTicks ++; 
   } 
}
 
