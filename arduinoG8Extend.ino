/*----Libraries---*/

#include <Wire.h>            //For US sensors
#include <SonarSRF08.h>      //For US sensors
#include <Servo.h>           //For Driving
#include "I2Cdev.h"          //For MPU6050 Gyro/Accel
#include "MPU6050_6Axis_MotionApps20.h" //For MPU6050 Gyro/Accel - Yaw calc
#include <PPMrcIn.h>         //For RC Override
#include <Statistic.h>       //For RC Override

/*----Special Bytes----*/

const byte endByte = 0xFF;
const byte startByte = 0xAA;
int driving = 0;

/*----Incoming Data----*/

byte recSignal[7];         // a byte array to hold incoming data
unsigned int speedIn = 1500;               //incoming speed data
unsigned int steeringIn = 90;            //incoming steering data
//byte lightsIn = 0;
boolean recComplete = false; //Check if valid packet

/*----Outgoing Data----*/

byte sendSignal[17];         // a byte array to hold outgoing data
unsigned int distanceOut = 105;              //outgoing speed data from Wheel encoder
unsigned int directionOut = 101;           //outgoing steering data (current setting)
unsigned int irFrontOut = 0;            //IR sensor right front outgoing
unsigned int irMiddleOut = 0;           //IR sensor right back outgoing
unsigned int irBackOut = 0;             //IR sensor back outgoing
unsigned int usFrontOut = 0;                //US sensor 1 outgoing
unsigned int usRightOut = 0;                //US sensor 2 outgoing

/*---- Servo and ESC ----*/

Servo esc;
Servo servo;
int throttle = 1500; //Nuetral
int angle = 90;      //Straight
int speedNow = 0;


/*----Wheel encoder and speed----*/

volatile unsigned long mmAbsDistance = 0;
unsigned int cmOldDistance = 0;
int cmPerSecond = 2;
unsigned long oldMillis;
unsigned int cmAbsDistance = 0;
unsigned int avSpeed[5];
int elementCounter = 0;


/*----Accelerometer + Gyro Fusion for Abs direction----*/

MPU6050 mpu;

bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

//value to send out
unsigned int absDirection = 0;


/*----Defining Sensor Data----*/

#define irFrontPin  0      //IR sensor right front in analog pin 0
#define irMiddlePin 1      //IR sensor right back in analog pin 1
#define irBackPin   2      //IR sensor back in analog pin 2
#define wheelEncoderPin 2  //Wheel Encoder digital pin
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
#define RANGE_REGISTER 0x46

/*---------RC Override ------*/
/*-****NOTE - All RC Override code is integration tested but not perfected and so remains experimental and commented out******-*/ 
//Channel channel1;
//Channel channel2;
//int rcThrottle;
//int rcAngle;



void setup() {
  // initialize serial:
  Serial.begin(115200);
  
  //RC Override setup
//  pinMode (10, INPUT);
//  channel1.init(1,10);
//  
//  pinMode (11, INPUT);
//  channel2.init(1,11);
  
  //  delay(5000);
  
//-----setup Gyro/Accel ADAPTED FROM EXAMPLE CODE-----
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize MPU6050
    mpu.initialize();

    // load and configure the DMP
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity    
    mpu.setXAccelOffset(-2439);
    mpu.setYAccelOffset(-117);
    mpu.setZAccelOffset(1790);
    mpu.setXGyroOffset(86);
    mpu.setYGyroOffset(-21);
    mpu.setZGyroOffset(-41);

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);
        // enable Arduino interrupt detection
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        dmpReady = true;
        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
  
//---Connect the US front and front-right using i2c addresses
  usFront.connect(FRONT_08_ADDRESS, GAIN_REGISTER, RANGE_REGISTER);
  usRight.connect(RIGHT_08_ADDRESS, GAIN_REGISTER, RANGE_REGISTER);
  
//---Attach the two main servos, steering and ESC
  esc.attach(9);
  servo.attach(8);
  
//---Set neutral and straight 
  esc.writeMicroseconds(throttle); 
  servo.write(angle + 2);//Correct for slight steering offset
  
//---Setup wheel Encoder
  attachInterrupt(1, updateDistance, RISING);

}

void loop() {
  cmAbsDistance = (int)(mmAbsDistance / 10);
  
//---experimental but unstable speed based ESC adjustment
//  if (millis() - oldMillis > 200){
//    oldMillis = millis();
//    calculateSpeed();
//    checkSpeed();
//  }

  if (recComplete) {
    distIncoming();
    
//    if((rcThrottle<2000 && rcThrottle>1000) || (rcAngle<2000 && rcAngle>1000)){
//      rcControl();
//    }else{  
  
      setTheSpeed();
      setTheSteering();
      
//    }   

    recComplete = false;
  }

  //Gather sensor Data
  
  if(dmpReady){
    getAbsDirection();
  }

  getDistance();
  getDirection();
  getIRFront();
  getIRMiddle();
  getIRBack();
  getUSFront();
  getUSRight();
  sendData();

}

//---Distribute incoming instructions to arduino structures
void distIncoming(){
  speedIn = recSignal[1] + ((int)recSignal[2] << 8);
  steeringIn = recSignal[3] + ((int)recSignal[4] << 8);
}

//---Build buffer for serial out, send it over the wire
void sendData(){
  sendSignal[16] = 0;
  byte i = 0;
  sendSignal[0] = startByte;
  sendSignal[1] = distanceOut & 0xFF;
  sendSignal[2] = (distanceOut >> 8) & 0xFF;
  sendSignal[3] = directionOut & 0xFF;
  sendSignal[4] = (directionOut >> 8) & 0xFF;
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
}

//---Adjust the throttle based on current speed - Experimental (not called)
void checkSpeed(){
  if(throttle > 1520 && 30 < cmPerSecond){
    speedNow++;
    esc.writeMicroseconds(speedNow);
  }else if(throttle < 1500 && 30 < cmPerSecond){
    speedNow--;
    esc.writeMicroseconds(speedNow);
  }
    
}

//Get Absolute Direction
void getAbsDirection(){
  if(mpuInterrupt || fifoCount >= packetSize) {
      // reset interrupt flag and get INT_STATUS byte
      mpuInterrupt = false;
      mpuIntStatus = mpu.getIntStatus();
      // get current FIFO count
      fifoCount = mpu.getFIFOCount();
  
      // check for overflow (this should never happen unless our code is too inefficient)
      if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
          // reset so we can continue cleanly
          mpu.resetFIFO();
  
      // otherwise, check for DMP data ready interrupt (this should happen frequently)
      } else if (mpuIntStatus & 0x02) {
          // wait for correct available data length, should be a VERY short wait
          while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
          // read a packet from FIFO
          mpu.getFIFOBytes(fifoBuffer, packetSize);
          
          // track FIFO count here in case there is > 1 packet available
          // (this lets us immediately read more without waiting for an interrupt)
          fifoCount -= packetSize;
          
          mpu.dmpGetQuaternion(&q, fifoBuffer);
          mpu.dmpGetGravity(&gravity, &q);
          mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
          absDirection = ((int)round(ypr[0] * 180/M_PI))+180;        
      }
  }
}

//---RC Overide Commands

//void rcControl(){
//    esc.writeMicroseconds(1500);
//    delay(5);
//    esc.writeMicroseconds(rcThrottle);
//    
//    servo.write(1500);  
//    delay(5);
//    servo.write(rcAngle); 
//}

//---Set the speed controller
void setTheSpeed(){
  driving = 1; 
  throttle = speedIn;
  if(throttle > 1520){  
    esc.writeMicroseconds(throttle);
  }else if (throttle < 1500){
    esc.writeMicroseconds(throttle);
  }else{
    driving = 0;
    esc.writeMicroseconds(throttle);
  }
  speedNow = throttle;
}

//---Set the steering servo angle
void setTheSteering(){
  servo.write(steeringIn + 2);
  
}

//Get distance from wheel encoder
void getDistance(){
  distanceOut = cmAbsDistance;
}

//---Check current heading data (uneccessary maybe?)
void getDirection(){
  directionOut = absDirection;
}

//---get right front IR Sensor by reading the analog pin
void getIRFront(){
  irFrontOut = analogRead (irFrontPin);
}

//---get right back IR Sensor by reading the analog pin
void getIRMiddle(){
  irMiddleOut = analogRead (irMiddlePin);
}

//---get back IR Sensor by reading the analog pin
void getIRBack(){
  irBackOut = analogRead (irBackPin);
}

//---get first Ultra Sonic Sensor
void getUSFront(){
  int usFrontOutTemp = usFront.getRange(unit);
  if(usFrontOutTemp != 255 && usFrontOutTemp != 0){
    usFrontOut = usFrontOutTemp;
  }
}

//---get second Ultra Sonic Sensor
void getUSRight(){
  int usRightOutTemp = usRight.getRange(unit);
  if(usRightOutTemp != 255 && usRightOutTemp != 0){
    usRightOut = usRightOutTemp;
  }
}


//---Interrupt fucntion for distance
void dmpDataReady() {
    mpuInterrupt = true;
}


//---Interrupt fucntion for distance
void updateDistance(){  
    mmAbsDistance+= 30;
}

//---Calculate speed based on wheel encoder - Experimental, not called
void calculateSpeed(){
  int i;
  int avSpeedCalc = 0;
  if(elementCounter > 3){
    elementCounter = 0;
  }else{
    elementCounter++;
  }
  avSpeed[elementCounter] = (cmAbsDistance - cmOldDistance);
  for(i = 0; i< 4 ; i++){
    avSpeedCalc += avSpeed[i];
  }
  cmPerSecond = avSpeedCalc;
  cmOldDistance = cmAbsDistance;
}


//---Serial incoming, when serial is avaliable runs after loop
void serialEvent(){
  byte i;
  byte badread;
  byte check = 0;
  while(Serial.available() > 14){
    Serial.read();
  }
  if(Serial.peek() == startByte){
    Serial.readBytes((char*)recSignal, 7);
    for(i=0;i<7;i++){
      check ^= recSignal[i];
    }
    if(i==7 && recSignal[0]== startByte && recSignal[5] == endByte && check == 0){
      recComplete=true;
    }
  }else{ 
    while (Serial.available() && badread != endByte){
      badread = Serial.read();
    }
  Serial.read();
  }
  if(recComplete == false && Serial.peek() == startByte){
    Serial.readBytes((char*)recSignal, 7);
    for(i=0;i<7;i++){
        check ^= recSignal[i];
    }
    if(i==7 && recSignal[0]== startByte && recSignal[5] == endByte && check == 0){
        recComplete=true;
    }
  }
}

// Wheel encoder wheel = 21.05cm 7 increments, 30mm per increment
  
