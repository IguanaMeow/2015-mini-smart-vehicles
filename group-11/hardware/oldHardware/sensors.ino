#include <Wire.h>
#define FRONT_SONAR 0x72
#define FRONT_RIGHT_SONAR 0x70

#define REAR_IR 2
#define FRONT_RIGHT_IR 1
#define REAR_RIGHT_IR 0
#define VoltsToCentimeters(volts) ((2914 / (volts + 5)) - 1)

#define cmdByte 0x00                              // Command byte
#define lightByte 0x01                            // Byte to read light sensor
#define rangeByte 0x02

int IR_front_right, IR_rear_right, IR_rear;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(100);
}

void loop() {
  
  IR_rear_right = IR_Distance(REAR_RIGHT_IR);
  IR_rear = IR_Distance(REAR_IR);
  IR_front_right= IR_Distance(FRONT_RIGHT_IR);
  
  Serial.print("IR rear right distance: ");
  Serial.print(IR_rear_right);
  Serial.print(" cm");
  Serial.print("\t");
  
  Serial.print("IR front right distance: ");
  Serial.print(IR_front_right);
  Serial.print(" cm");
  Serial.print("\t");
  
  Serial.print("IR rear distance: ");
  Serial.print(IR_rear);
  Serial.print(" cm");
  Serial.print("\t");
  
  Serial.println();
  //delay(50);
  startSonars();
  int sonarFront = getRange(FRONT_SONAR);                      // Calls a function to get the range data 
  Serial.print("Front sonar distance: ");
  Serial.print(sonarFront);
  Serial.print(" cm");
  Serial.print("\t");
  
  int sonarFrontRight = getRange(FRONT_RIGHT_SONAR);                      // Calls a function to get the range data 
  Serial.print("Front right sonar distance: ");
  Serial.print(sonarFrontRight);
  Serial.print(" cm");
  
  Serial.println();
  
  delay(700);

}

int IR_Distance(int IR_sensor){
  float sensorValue = 0;
  sensorValue = analogRead(IR_sensor);
  int distance = VoltsToCentimeters(sensorValue);
  
  if(distance < 40){
  return(distance);
  }
  return(-1);
}

void startSonars()
{
  Wire.beginTransmission(0x00);             // Start communicating with SRF08
  Wire.write((byte)cmdByte);                             // Send Command Byte
  Wire.write(0x51);                                // Send 0x51 to start a ranging in cm
  Wire.endTransmission();
 
  delay(80);
}

int getRange(unsigned char srfAddress){                                   // This function gets a ranging from the SRF08
  int range = 0; 
 
                                     // Wait for ranging to be complete
 
  Wire.beginTransmission(srfAddress);             // start communicating with SRFmodule
  Wire.write(rangeByte);                           // Call the register for start of ranging data
  Wire.endTransmission();
 
  Wire.requestFrom(srfAddress, 2);                // Request 2 bytes from SRF module
  while(Wire.available() < 2);                    // Wait for data to arrive
  byte highByte = Wire.read();                 // Get high byte
  byte lowByte = Wire.read();                  // Get low byte
 
  range = (highByte << 8) + lowByte;              // Put them together
 
  return(range);                                  // Returns Range
}
