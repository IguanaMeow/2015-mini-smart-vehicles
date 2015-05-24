
#include <Servo.h> 
#define servoPin 8
#define escPin 9

#define escHigh 2000
#define escLow 1000

Servo servo, esc;  // create servo and esc object 
 
int throttle = 1500;
int angle = 90;

void setup() 
{ 
  Serial.begin(38400);
  servo.attach(servoPin); 
  esc.attach(escPin);  
   
   /*
   // Calibrate
   esc.writeMicroseconds(throttle);
   while (!Serial.available());
   Serial.read();
   // Send neutral
   Serial.println("Wait 2 secs then press any key"); 
   Serial.println("Sending neutral");
   esc.writeMicroseconds(throttle);
   while (!Serial.available());
   Serial.read();
   
   // Send max val
   Serial.println("Wait 2 secs then press any key"); 
   Serial.println("Sending max");
   esc.writeMicroseconds(escHigh);
   // Input
   while (!Serial.available());
   Serial.read();

   // Send low value
   Serial.println("Wait 2 secs then press any key");
   Serial.println("Sending low");
   esc.writeMicroseconds(escLow);
   while (!Serial.available());
   Serial.read();
   
   Serial.println("Wait 2 secs then press any key");
   Serial.println("Sending neutral");
   esc.writeMicroseconds(throttle);
   while (!Serial.available());
   Serial.read();
   */
} 
 
void loop() { 
  
  //throttle = map(512, 0, 1023, 1000, 2000);
  esc.writeMicroseconds(throttle);
  servo.write(angle);
  
  delay(10);

}

void serialEvent() {
 while (Serial.available()) {
   char c = Serial.read();
   
   switch(c) {
    case 'f': throttle = throttle + 10; break;
    case 's': throttle = throttle - 10; break; 
    case 'w': throttle = 1570; break;
    case 'n': throttle = 1500; angle = 90; break;
    case 'r': throttle = 1270; break;
    
    case 'v': angle = 65; break;
    case 'm': angle = 90; break;
    case 'h': angle = 115; break;
  }
 }
}
