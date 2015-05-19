
#include <Wire.h>
#include <Servo.h>

#define ADDRESS (0x70) //Ultrasonic_1 address
#define ADDRESS2 (0xE4 >> 1) //Ultrasonic_2 address
#define IR_1 7 //pin for front right 
#define IR_2 0 //pin for rear right IR
#define IR_3 5 // pin for the back IR


//Global Variables
int IR_rightFront, IR_rightRear, IR_rear, val1, val2, val3 = 0;
int sensorReading = 0;
int sensorReading2 = 0;
const int encoderPinA = 19;
const int encoderPinB = 25;
volatile long encoderTicks = 0;
unsigned int integerVal = 0;
int US1, US2;
int throttle = 1500;
int angle = 90;
int reading = 0;
bool dataReceived = true;

String inputString;

Servo esc;
Servo servo;

void setup()
{
  inputString.reserve(10); // Instatiate Objects
  Wire.begin();
  esc.attach(9);
  servo.attach(10);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  attachInterrupt(4, WheelEncoderInterrupt, CHANGE);
  Serial.begin(115200);          // start serial communication at 9600bps
}



void loop()
{
  if (dataReceived == true) {
    //IR_rightFront = 3;
    //IR_rightRear = -1;
    //IR_rear = 2;
    // US1 = 20;
    // US2 = 10;
    esc.writeMicroseconds(throttle);
    servo.write(angle);
    val1 = analogRead(IR_1);  // reads the value of the sharp sensor
    val2 = analogRead(IR_2);
    val3 = analogRead(IR_3);

    //read infrared sensors
    IR_rightFront = ((2914 / (val1 + 5) - 1));
    IR_rightRear =  ((2914 / (val2 + 5) - 1)); //converts to cm
    IR_rear = ((2914 / (val3 + 5) - 1));
    US1 = range(ADDRESS);
    US2 = range(ADDRESS2);

    //convert int to strings
    String U1 = String(US1);
    String U2 = String(US2);
    String IR1 = String( IR_rightFront);
    String IR2 = String(IR_rightRear);
    String IR3 = String( IR_rear);
    String WE = String(encoderTicks);
    String speedValue = String(integerVal);

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

    //prints on serial monitor
    Serial.println(encodeNetstring(sensorData));
    dataReceived = false;

  }
  //delay(100);               // wait for this much time before printing next value

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

void serialEvent() {
  char inChar;
  while (Serial.available() > 0) {  // something came across serial

    String value;
    int integerVal = 0;
    static char buffer[34];
    if (readline(Serial.read(), buffer, 34) > 0) {
      value = decodeNetstring(buffer);
    }
    if (value.startsWith("WA")) {
      int equalsIndex = value.indexOf('=');
      String getData = value.substring(equalsIndex + 1);
      integerVal = getData.toInt();
      throttle = 1600;
      angle = integerVal + 90;

      equalsIndex = getData.indexOf('=');
      String speedData = getData.substring(equalsIndex + 1);
      

    }

    inputString = "";
  }
  dataReceived = true;
}


//Jani
String encodeNetstring(String value) {

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

int readline(int readch, char *buffer, int len) {
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case ',': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}





