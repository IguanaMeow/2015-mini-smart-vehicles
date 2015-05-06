//Regine

#include <Wire.h>
#include <Servo.h>
#define ADDRESS (0x70) //address of the sensor
#define ADDRESS2 (0xE4 >> 1) //address of the other sensor
#define IS_1 7 //pin for front right 
#define IS_2 0 //pin for rear right IR
#define IS_3 5 // pin for the back IR   
int IS_frontRight, IS_rearRight, IS_back, val1, val2, val3;
int sensorReading = 0;
int sensorReading2 = 0;
const int encoderPinA = 19;
const int encoderPinB = 25;
volatile long encoderTicks = 0;
unsigned int integerVal = 0;
Servo esc;
Servo servo;
int US1, US2;

int throttle = 1500; //
int angle = 90;

void setup()
{
  Wire.begin();
  esc.attach(9);
  servo.attach(10);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(4, WheelEncoderInterrupt, CHANGE);
  Serial.begin(9600);          // start serial communication at 9600bps
}

int reading = 0;

void loop()
{
  esc.writeMicroseconds(throttle);
  servo.write(angle);
  val1 = analogRead(IS_1);  // reads the value of the sharp sensor
  val2 = analogRead(IS_2);
  val3 = analogRead(IS_3);
  IS_frontRight = ((2914 / (val1 + 5) - 1));
  IS_rearRight =  ((2914 / (val2 + 5) - 1)); //converts to cm
  IS_back = ((2914 / (val3 + 5) - 1));
  US1 = range(ADDRESS);
  US2 = range(ADDRESS2);
  String U1 = String(US1);
  String U2 = String(US2);
  String IS1 = String(IS_frontRight);
  String IS2 = String(IS_rearRight);
  String IS3 = String(IS_back);
  String WE = String(encoderTicks);
  String speedValue = String(integerVal);


  Serial.println(encodeStringNetstring("U1=" + (U1)));
  // prints the value of the sensor to the serial monitor
  Serial.println(encodeStringNetstring("U2=" + (U2)));

  Serial.println(encodeStringNetstring("IS1=" + (IS1)));

  Serial.println(encodeStringNetstring("IS2=" + (IS2)));

  Serial.println(encodeStringNetstring("IS3=" + (IS3)));

  Serial.println(encodeStringNetstring("WE=" + (WE)));

  Serial.println(encodeStringNetstring("SPEED" + (speedValue)));

  delay(100);               // wait for this much time before printing next value

}
//function that gets the range and read the sensor values
int range(int ADDRESS_) {
  int range = 0;
  Wire.beginTransmission(ADDRESS_); // transmit to device

  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)
  Wire.write(byte(0x51));     //for centimeters
  Wire.endTransmission();     // stop transmitting

  // step 2: wait for readings to happen
  delay(70);

  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(ADDRESS_); // transmit to device #112
  Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire.requestFrom(ADDRESS_, 2);    // request 2 bytes from slave device #112

  // step 5: receive reading from sensor
  if (2 <= Wire.available())   // if two bytes were received
  {
    range = Wire.read();  // receive high byte (overwrites previous reading)
    range = range << 8;    // shift high byte to be high 8 bits
    range |= Wire.read(); // receive low byte as lower 8 bits
  }
  return range;
  delay(250);                  // wait a bit since people have to read the output :)
}
void WheelEncoderInterrupt() { //if pinA is high and pinB is low ticks ++ else ticks
  //if pinA is low and pinB is low ticks -- else ticks ++
  if (digitalRead(encoderPinA)) {
    !(digitalRead(encoderPinB)) ? encoderTicks -- : encoderTicks ++;
  }
  else {
    !(digitalRead(encoderPinB)) ? encoderTicks ++ : encoderTicks --;
  }
}

// Functions
//Regine
void serialEvent() {
  unsigned int integerValue = 0; // Max value is 65535
  char inByte;

  if (Serial.available() > 0) {   // something came across serial
    integerVal = 0;         // throw away previous integerValue
    while (1) {           // force into a loop until 'n' is received
      inByte = Serial.read();
      if (inByte == '\n') break;   // exit the while(1), we're done receiving
      if (inByte == -1) continue;  // if no characters are in the buffer read() returns -1
      integerVal *= 10;  // shift left 1 decimal place
      // convert ASCII to integer, add, and shift left 1 decimal place
      integerVal = ((inByte - 48) + integerVal);
      String speedValue = String(integerVal);
      Serial.println(encodeStringNetstring(speedValue));

      if (integerVal >= 1600) {
        throttle = 1600;
      }
      if (integerVal == 1500) {
        throttle = 1500;
      }
      if (integerVal == 1200) {
        throttle = 1200;
      }
      if (integerVal == 65) {
        angle = 65;
      }
      if (integerVal == 115) {
        angle = 115;
      }
      if (integerVal == 90) {
        angle = 90;
      }

    }
  }

}


//Jani
String encodeStringNetstring(String value) {

  int len = value.length();
  if (!len) return "ERROR:_EMPTY_STRING";
  String netstring = len + String(":" + value + ",");

  return netstring;

}


// Needs to be tested to insure that decoding works. // Jani
String decodeNetstring(String netstring) {
  if (netstring.length() < 3) return "NETSTRING_ERROR_TOO_SHORT";

  //if (netstring.length() > ?) return "NETSTRING_ERROR_TOO_LONG";

  int semicolonIndex = netstring.indexOf(':');
  if (semicolonIndex < 0) return "NETSTRING_ERROR_NO_COLON";

  String getLength = netstring.substring(0, semicolonIndex);
  int payloadLength = getLength.toInt();
  if (payloadLength < 1) return "NETSTRING_ERROR_LEADING_ZERO";

  String payload = netstring.substring(semicolonIndex + 1);
  if (!payload.length()) return "NETSTRING_ERROR_NO_LENGTH";

  if (payload.substring(payload.length() - 1) == ",") payload.remove(payload.length() - 1); //remove the comma
  if (payload.length() != payloadLength) return "NETSTRING_ERROR_INVALID_LENGTH";

  return payload;

}





