#include <Wire.h>      
#define cmdByte 0x00              
#define lightByte 0x01  // Byte to read light sensor
#define rangeByte 0x02  // Byte for start of ranging data
#define cm 0x51
#define inch 0x50
 
int US_Front_R = 0x70; // Change address, 0x70 - 0x77 
int US_Front = 0x72;
byte highByte = 0x00;  // Stores high byte from ranging
byte lowByte = 0x00;   // Stored low byte from ranging
 
void setup(){
  Wire.begin();        // Waits to make sure everything is powered up before sending or receiving data
  Serial.begin(38400);
  delay(100);
}
 
void loop(){
  int US_F_Dist = getRange(US_Front_R);
  int US_FR_Dist = getRange(US_Front);
  delay(100);
  
  String US_F_Str = String(US_F_Dist);
  String US_FR_Str = String(US_FR_Dist);
  String to_print = "F: " + US_F_Str + " cm, FR: " + US_FR_Str + " cm";
  Serial.println(to_print);
    
  delay(100);
  
  }
 
int getRange(int address) { 
 
  int range = 0; 
 
  Wire.beginTransmission(address);  // Start communticating with SRF08
  Wire.write((byte)cmdByte);           // Send Command Byte
  Wire.write(cm);                      // Choose between inch and cm
  Wire.endTransmission();
 
  delay(70);                          // Wait for ranging to be complete
 
  Wire.beginTransmission(address);  // start communicating with SRFmodule
  Wire.write(rangeByte);               // Call the register for start of ranging data
  Wire.endTransmission();
 
  Wire.requestFrom(address, 2);     // Request 2 bytes from SRF module
  while (Wire.available() < 2);
  byte highByte = Wire.read();
  byte lowByte = Wire.read();
  
  range = (highByte << 8) + lowByte;
   
  return(range);                      // Returns Range
}

