//----Libraries---

#include <Wire.h>            //For US sensors
#include <SonarSRF08.h>      //For US sensors

//----Special Bytes----
const byte endByte = 0xFF;
const byte startByte = 0xAA;

//----Incoming Data ----
byte recSignal[7];         // a byte array to hold incoming data
int speedIn = 100;               //incoming speed data
int steeringIn = 101;            //incoming steering data
//byte lightsIn = 0;
boolean recComplete = false; //Check if valid packet

//----Old Incoming Data ----
int speedInOld;               //Last changed speed data
int steeringInOld;            //Last changed steering data
//byte lightsInOld;

//----Outgoing Data ----
byte sendSignal[17];         // a byte array to hold outgoing data
int speedOut = 100;              //outgoing speed data from Wheel encoder
int steeringOut = 101;           //outgoing steering data (current setting)
int irFrontOut = 102;            //IR sensor right front outgoing
int irMiddleOut = 103;           //IR sensor right back outgoing
int irBackOut = 104;             //IR sensor back outgoing
int usFrontOut = 105;                //US sensor 1 outgoing
int usRightOut = 106;                //US sensor 2 outgoing

//----Defining Sensor Data----
#define irFrontPin  0      //IR sensor right front in analog pin 0
#define irMiddlePin 1      //IR sensor right back in analog pin 1
#define irBackPin   2      //IR sensor back in analog pin 2
#define FRONT_08_ADDRESS (0xE6 >> 1) //Address for the Front US sensor
#define RIGHT_08_ADDRESS (0xE0 >> 1) // adress for the Right US sensor
SonarSRF08 usFront;
SonarSRF08 usRight;
char unit = 'c'; // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds used to define distance with US sensors

// Setup Analogue Gain
// http://www.robot-electronics.co.uk/htm/srf08tech.html section "Analogue Gain"
#define GAIN_REGISTER 0x09
// Setup Range Location
// http://www.robot-electronics.co.uk/htm/srf08tech.html section "Changing the Range"
#define LOCATION_REGISTER 0x8C

void setup() {
  // initialize serial:
  Serial.begin(57600);
  usFront.connect(FRONT_08_ADDRESS, GAIN_REGISTER, LOCATION_REGISTER);//connect to the front US sensor
  usRight.connect(RIGHT_08_ADDRESS, GAIN_REGISTER, LOCATION_REGISTER);//connect yo the right US sensor
  // TODO - A blocking function that waits for a predefined signal from the Odroid here or in loop
  // Possibly use the handshake example?
}

void loop() {
  // print the string when a newline arrives:
  if (recComplete) {
    distIncoming();
    //Set Speed data if different from current
    if(speedIn != speedInOld){
      //setTheSpeed();
      speedInOld = speedIn;
    }
    //Set Speed data if different from current - currently using old value, maybe use wheel encoder?
    if(steeringIn != steeringInOld){
      //setTheSteering();
      steeringInOld = steeringIn;
    }    
    recComplete = false;
  }
  //Gather sensor Data
  getSpeed();
  getSteering();
  getIRFront();
  getIRMiddle();
  getIRBack();
  getUSFront();
  getUSRight();
  sendData();
}

//Distribute incoming instructions to arduino structures
void distIncoming(){
  speedIn = recSignal[1] + ((int)recSignal[2] << 8);
  steeringIn = recSignal[3] + ((int)recSignal[4] << 8);
  //lightsIn = recSignal[5]
}

//Build buffer for serial out, send it over the wire
void sendData(){
  sendSignal[16] = 0;
  byte i = 0;
  sendSignal[0] = startByte;
  sendSignal[1] = speedOut & 0xFF;
  sendSignal[2] = (speedOut >> 8) & 0xFF;
  sendSignal[3] = steeringOut & 0xFF;
  sendSignal[4] = (steeringOut >> 8) & 0xFF;
  sendSignal[5] = irFrontOut & 0xFF;
  sendSignal[6] = (irFrontOut >> 8) & 0xFF;
  sendSignal[7] = irMiddleOut & 0xFF;
  sendSignal[8] = (irMiddleOut >> 8) & 0xFF;
  sendSignal[9] = irBackOut & 0xFF;
  sendSignal[10] = (irBackOut >> 8) & 0xFF;
  sendSignal[11] = usFrontOut & 0xFF;
  sendSignal[12] = (usFrontOut >> 8) & 0xFF;
  sendSignal[13] = usRightOut & 0xFF;
  sendSignal[14] = (usRightOut >> 8) & 0xFF;
  sendSignal[15] = endByte;
  for(i = 0; i < 16; i++){
    sendSignal[16] ^= sendSignal[i];
  } 
  Serial.write(sendSignal, 17);
//  Serial.print("IRFront");
//  Serial.print(irFrontOut);
//  Serial.print("\n");
//  Serial.print("IRMiddle");
//  Serial.print(irMiddleOut);
//  Serial.print("\n");
//  Serial.print("IRBack");
//  Serial.print(irBackOut);
//  Serial.print("\n");
//  Serial.print("USFRONT");
//  Serial.print(usFrontOut);
//  Serial.print("\n");
//  Serial.print("USRIGHT");
//  Serial.print(usRightOut);
//  Serial.print("\n");
//  Serial.print("\n");
//  Serial.print("\n");
}

//Set the speed controller
void setTheSpeed(){
  //TODO
}

//Set the steering servo angle
void setTheSteering(){
  //TODO
}

//void setTheLights(){
//  //TODO
//}

//Get speed from wheel encoder
void getSpeed(){
  //TODO
  speedOut = speedIn;
}

//Check current steering data (uneccessary maybe?)
void getSteering(){
  //TODO
  steeringOut = steeringIn;
}

//get right front IR Sensor by reading the analog pin
void getIRFront(){
  irFrontOut = analogRead (irFrontPin);
  //Serial.println (irFrontOut);
}

//get right back IR Sensor by reading the analog pin
void getIRMiddle(){
  irMiddleOut = analogRead (irMiddlePin);
  //Serial.println (irMiddleOut);
}

//get back IR Sensor by reading the analog pin
void getIRBack(){
  irBackOut = analogRead (irBackPin);
  //Serial.println (irBackOut);
}

//get first Ultra Sonic Sensor
void getUSFront(){
  usFrontOut = usFront.getRange(unit);
  //Serial.println (usFrontOut);
}

//get second Ultra Sonic Sensor
void getUSRight(){
  usRightOut = usRight.getRange(unit);
  //Serial.println (usRightOut);
}

void serialEvent(){
  byte i;
  byte badread;
  byte check = 0;
  while(Serial.available() && Serial.peek() != startByte){
    Serial.read();
  }
  Serial.readBytes((char*)recSignal, 7);
  for(i=0;i<7;i++){
    check ^= recSignal[i];
  }
  if(i==7 && recSignal[0]== startByte && recSignal[5] == endByte && check == 0){
    recComplete=true;
  }else{ 
    while (Serial.available() && badread != endByte){
      badread = Serial.read();
    }
    
  }
}
