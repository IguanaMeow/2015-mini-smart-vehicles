/*
AUTHOR/S:
 Giorgi Dundua
 */
#ifndef CarVariables_h
#define CarVariables_h
//baud rate and serial delay
//pins
//remote control receiver pins
const int rcPin = 7;
const int rcEscPin = 6;
const int rcServoPin = 5;

//steering servo and esc pins
const int escPin = 8;
const int servoPin = 9;
// infrared sensors pins
const int ir1 = 0;
const int ir2 = 1;
const int ir3 = 2;

//steering and speed variables
const int speedIdle = 1500;
const int speedMax = 1800;
const int speedMin = 1200;
const int angleStraight = 90;
const int angleLeft = 64;
const int angleRight = 115;
int spd = 0;
int angle = 0;
//wheel encoder
const int encoderPinA = 2;
const int encoderPinB = 3;

//wheel encoder variables
volatile int wheelTicks = 0; //8 = 1 = 21 cm

//steering and speed variables in rc mode
const int rcEscMin = 1050; //1058
const int rcEscMax = 2040; //2039
const int rcSpeedMax = 1630; //limit speeds in rc mode
const int rcSpeedMin = 1250; //limit speeds in rc mode

//rc receiver pulse lenths and their variance
const int rcPulseIdle = 1500;
const int rcServoMin = 990; //992
const int rcServoMax = 2010; //2009
const int rcPulseLengthMin = 50; //arbitrary non 0 small number max is 995
const int angleVariance = 70; //
const int spdVariance = 70; ///
int rcPulseLength;
int rcServoPulseLength;
int rcEscPulseLength;
int remainderA;
int remainderS;
bool rcControl = false;

//Ultrasonc sensor addresses
#define sonarCentreAddress (0xE0 >> 1)//(0xE0 >> 1)
#define sonarRightAddress (0xE6 >> 1)//(0xE6 >> 1)
// Setup Analogue Gain & range
// http://www.robot-electronics.co.uk/htm/srf08tech.html section "Analogue Gain" & section "Changing the Range"
#define GAIN_REGISTER 0x09 //0x09
#define LOCATION_REGISTER 0x18//140 (0x8C) is 6 metres. 24 (0x18) gives a range of 1 metre.

//ultrasonic variables
float sonarCentreReading = 0;
float sonarRightReading = 0;
float sonarMax = 90; // OCATION_REGISTER set to 100 cm
float sonarMin = 4;

//infrared variables
float irMax = 30;
float irMin = 4;

#endif