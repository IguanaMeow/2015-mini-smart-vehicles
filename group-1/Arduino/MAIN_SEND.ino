
#include <Wire.h>

#define srfAddress1 0x70                           // Address of the SRF08
#define srfAddress2 0x73                           // Address of the SRF08

#define cmdByte 0x00                              // Command byte
#define lightByte 0x01                            // Byte to read light sensor
#define rangeByte 0x02                            // Byte for start of ranging data

const int pingPin = 7;

const int trigPinM = 42;
const int echoPinM= 43;

const int irPin1 = 15;
const int irPin2 = 14;
const int irPin3 = 13;

long ir1, ir2, ir3;
long us1, us2, us3; 

String input, encodedS;
String inf1;
String inf2;
String inf3;
String ult1;
String ult2;
String ult3;

void setup() {
  Wire.begin();     
  // initialize serial communication:
  pinMode(irPin1,OUTPUT);
  pinMode(irPin2,OUTPUT);
  pinMode(irPin3,OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  ir1 = infraredSense(irPin1);
  ir2 = infraredSense(irPin2);
  ir3 = infraredSense(irPin3);

  us1= ultrasonicSense(trigPinM, echoPinM);
  us2= getRange(srfAddress1);                      // Calls a function to get the range data
  us3= getRange(srfAddress2);  

  //Serial.print("Range1: ");
  //Serial.print(us1);
  //Serial.println("cm");

  //Serial.print("Range2: ");
  //Serial.print(us2);
  //Serial.println("cm");
  inf1=checkLength(String(ir1));
  inf2=checkLength(String(ir2));
  inf3=checkLength(String(ir3));
  ult1=checkLength(String(us1));
  ult2=checkLength(String(us2));
  ult3=checkLength(String(us3));

  input+= inf1+ inf2+ inf3+ ult1+ ult2+ ult3;
  encodedS=encodedNetstring(input);
  Serial.println(encodedS);
  encodedS="";
  input="";
  //delay(100);
}



String encodedNetstring(String plainInput){
  unsigned int len = plainInput.length();
  if (!len) return "error"; //if the input string is empty, return "error"
  return len + String(":" + plainInput + ","); //return a Netstring in the form of [len]":"[string]","
}
long infraredSense(const int pin){
  // A variable to store the values from sensor.
  // This is initially zero.
  int val;

  // A variable to store the calculated cm value
  int cm;

  // read the value of the sharp sensor on A3
  val = analogRead(pin);

  // Apply Linearisation
  cm = (2914 / (val +5))-1;

  // prints the value of the sensor to serial
  // Serial.print("Infrared cm: ");
  //Serial.print(cm);
  //Serial.println();

  return cm;
}

long ultrasonicSense(int trigPin, int echoPin){
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration,  cm;

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);

  //Serial.print(cm);
  //Serial.print("cm for US1");
  //Serial.println();
  return cm;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
} 

int getRange(int srfAddress){                                   // This function gets a ranging from the SRF08
  int range = 0; 

  Wire.beginTransmission(srfAddress);             // Start communicating with SRF08
  Wire.write((byte)cmdByte);                             // Send Command Byte
  Wire.write(0x51);                                // Send 0x51 to start a ranging in cm
  Wire.endTransmission();

  delay(100);                                     // Wait for ranging to be complete

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

String checkLength(String value){
  if(value.length() < 3){
    if(value.length()==2){
      value= "0"+ value;

    }
    if(value.length()==1){
      value="00"+ value;
    }
    return value;
  }
}

