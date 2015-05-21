
#include <Wire.h>
#include <Servo.h>

#define USRF1 (0x70)
#define USRF2 (0xE4 >> 1)
#define IR_1 7
#define IR_2 0
#define IR_3 5


//Global Variables
const int encoderPinA = 19;
const int encoderPinB = 25;
int IR_rightFront, IR_rightRear, IR_rear;
int US1, US2;
int throttle = 1500;
int angle = 90;
String inputString;
bool dataReceived = true;
volatile unsigned long encoderTicks = 0;

Servo esc;
Servo servo;

void setup()
{
  inputString.reserve(10); // Instantiate Objects
  Wire.begin();
  Serial.begin(9600);

  esc.attach(9);
  servo.attach(10);

  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(4, WheelEncoderInterrupt, CHANGE);
  
  throttle = 1500; 
}

void loop()
{
  if (dataReceived == true) {

    esc.writeMicroseconds(throttle);
    servo.write(angle);

    // Converts infrared readings to cm
    IR_rightFront = convert(IR_1);
    IR_rightRear =  convert(IR_2);
    IR_rear = convert(IR_3);

    US1 = range(USRF1);
    US2 = range(USRF2);

    //Convert int to String
    String U1 = String(US1);
    String U2 = String(US2);
    String IR1 = String( IR_rightFront);
    String IR2 = String(IR_rightRear);
    String IR3 = String( IR_rear);
    String WE = String(encoderTicks);

    String sensorData = "";
    sensorData += "IR1=";
    sensorData += IR1;
    sensorData += "IR2=";
    sensorData += IR2;
    sensorData += "IR3=";
    sensorData += IR3;
    sensorData += "US1=";
    sensorData += U1;
    sensorData += "US2=";
    sensorData += U2;
    sensorData += "WE=";
    sensorData += WE;


    
   Serial.println(encodeNetstring(sensorData));
    dataReceived = false; //Boolean to synchronize serial
  }
}
// Function Converts IR to cm
int convert(int infrared) {
  int val = analogRead(infrared);
  // Converts infrared readings to cm
  infrared = ((2914 / (val + 5) - 1));
  if (infrared > 30) {
    return infrared = -1;
  }
  return infrared;
}


// Ultrasonic Setup
int range(int ADDRESS_) {
  int range = 0;
  Wire.beginTransmission(ADDRESS_);

  Wire.write(byte(0x00));     //command register (0x00)
  Wire.write(byte(0x51));     //set cm
  Wire.endTransmission();

  delay(70);

  Wire.beginTransmission(ADDRESS_);
  Wire.write(byte(0x02));      //echo #1 register (0x02)
  Wire.endTransmission();

  Wire.requestFrom(ADDRESS_, 2);

  if (2 <= Wire.available())
  {
    range = Wire.read();
    range = range << 8;
    range |= Wire.read();
    if(range > 100) {
      return -1; 
    }
  }
  return range;
  delay(250);
}

void WheelEncoderInterrupt() {
  if (digitalRead(encoderPinA)) {
    !(digitalRead(encoderPinB)) ? encoderTicks -- : encoderTicks ++;
  }
  else {
    !(digitalRead(encoderPinB)) ? encoderTicks ++ : encoderTicks --;
  }
}

void serialEvent() {

  while (Serial.available() > 0) {

    String value;
    int intVal = 0;
    static char buffer[34];

    if (readline(Serial.read(), buffer, 34) > 0) {
      value = decodeNetstring(buffer);
    }
    if (value.startsWith("WA")) {
      int equalsIndex = value.indexOf('=');
      String getData = value.substring(equalsIndex + 1);
      intVal = getData.toInt();
      angle = intVal + 90;

      equalsIndex = getData.indexOf('=');
      String speedData = getData.substring(equalsIndex + 1); 
      float acceleration = speedData.toFloat();

      if (acceleration == 0) throttle = 1500;
      if (acceleration > 0) throttle = 1600;
      if (acceleration < 0) throttle = 1100;
    }
    inputString = "";
  }
  dataReceived = true;
}

String encodeNetstring(String value) {

  int len = value.length();
  if (!len) return "ERROR:_EMPTY_STRING";
  String netstring = len + String(":" + value + ",");

  return netstring;
}

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

int readline(int readch, char *buffer, int len) {

  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n':
        break;
      case ',':
        rpos = pos;
        pos = 0;
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  return -1;
}
