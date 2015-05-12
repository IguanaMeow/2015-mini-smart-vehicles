/*
Adapted from the following code:

Receiver signal reading
https://github.com/kiuz/PPM-Signal-Reader-ARDUINO/

Servo and ESC control
http://arduino.cc/it/Tutorial/Sweep

*/

// setup radio receiver interpretation
#include <PPMrcIn.h>
#include <Statistic.h>
Channel channel1;
Channel channel2;

// include libraries & initialize classes for controlling car
#include <Servo.h> 
Servo esc;
Servo servo;

int throttle = 1500;
int angle = 90;
int rcThrottle;
int rcAngle;

void setup() {
  Serial.begin(115200);
 
  // prepare reading of receiver
  Serial.println("Starting up...");
  pinMode (10, INPUT);
  channel1.init(1,10);
  
  pinMode (11, INPUT);
  channel2.init(1,11);
  
  // prepare writing of speed control and steering servo
  esc.attach(9);
  servo.attach(8);

  // wait five seconds to bypass throttle neutral protection
  delay(5000); 
}

void loop() {
  // read the receiver signal
  channel1.readSignal();
  rcThrottle = channel1.getSignal();
  
  channel2.readSignal();
  rcAngle = channel2.getSignal();
  
  
  // if no signal detected from RC, then let the car drive itself
  if(rcThrottle>2000 || rcThrottle<1000 || rcAngle>2000 || rcAngle<1000){
    
    //Serial.println("No RC signal detected");
    
    // send direction and speed to servo and esc
    esc.writeMicroseconds(1500);
    delay(5);
    esc.writeMicroseconds(throttle);
    
    servo.write(90);  
    delay(5);
    servo.write(angle);    
  }
    
  
  // if RC sends a signal, then let the driver drive the car
  else{
    // pass esc and servo inputs to esc and servo.
    // Without spending some time at neutral the esc
    // would not reverse.  The minimum time required
    // was found to be 5 ms by trial & error
    
    //Serial.print("Channel 1: ");
    //Serial.print(rcThrottle);
    //Serial.print("Channel 2: ");
    //Serial.print(rcAngle);
    //Serial.println("");
    
    esc.writeMicroseconds(1500);
    delay(5);
    esc.writeMicroseconds(rcThrottle);
    
    servo.write(1500);  
    delay(5);
    servo.write(rcAngle);    
    
    delay(10);
  }
}
