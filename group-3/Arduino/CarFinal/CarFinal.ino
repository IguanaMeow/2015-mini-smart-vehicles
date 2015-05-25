/*
AUTHORS:
 Anna Graduleva - IR and US 
 Saimonas Sileikis - Connection to Odroid
 Giorgi Dundua - Everything else, as well as contribution to above
 
 
 */
#include <CarVariables.h>
//NanoPB related
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <message.pb.h>

int streamInSize = 6; 
int streamOutSize = 27; //streamOut.bytes_written

//Servo and ESC related
#include <Servo.h>
Servo esc;
Servo servo;
//Ultra sonic rage finder related
#include <Wire.h>
#include <SonarSRF08.h>

SonarSRF08 sonarCentre;
SonarSRF08 sonarRight;

/////this have to go in CarVariables.h if there is time////////
float sonarCentreTemp = -1;
float sonarRightTemp = -1;
//////////////
//making signal stop if wheels traight and speed idle
bool rcSpeedReset = false;
bool rcAngleReset = false;

void setup() {
  esc.attach(escPin);
  servo.attach(servoPin);
  setSpd(spd);
  setAngle(angle);

  rcGetPulseLengths(); ////
  rcControling(); //
  rcInput();  //

  setupSonars();
  
  attachInterrupt(0, wheelEncoder, CHANGE);

  Serial.begin(9600);
  //Serial.setTimeout(200); ///200 sets the maximum milliseconds to wait for serial data when using
}

void loop() {
  //rcGetPulseLengths(); //
  //rcControling(); //
  //rcInput(); //
  getSonarReadings();

  //takes sensor readings, sets message fields to Odroid, encodes and writes to serial
  ArduinoToOdroid messageOut = prepareMessage();

  uint8_t bufferOut[64];
  // Create a stream from an empty 64 byte array
  pb_ostream_t streamOut = pb_ostream_from_buffer((uint8_t*)bufferOut, sizeof(bufferOut));
  bool sendStatus = pb_encode(&streamOut, ArduinoToOdroid_fields, &messageOut);

  if (sendStatus) {
    //Serial.println(streamOut.bytes_written);
    for (int i = 0; i < streamOutSize; i++) {
      Serial.write(bufferOut[i]);
    }
  }
  /*if (!sendStatus) {
    Serial.println("fail");
  }*/
  //////////////////////////////
  //test with same message in
  /*ArduinoToOdroid messageIn;


     // Creates input stream from bufferIn
     pb_istream_t streamIn = pb_istream_from_buffer(bufferOut, sizeof(bufferOut));

     bool receiveStatus = pb_decode(&streamIn, OdroidToArduino_fields, &messageIn);

     // If decoding was succesfull then message is parsed
     if (receiveStatus) {
       int spd = messageIn.usCentre;
       int angle = messageIn.usCentre;
     }*/
  //////////////////////////////
  //for testing
  /*Serial.println(messageOut.usCentre);
  Serial.println(messageOut.usRight);
  Serial.println(messageOut.irRight_one);
  Serial.println(messageOut.irRight_two);
  Serial.println(messageOut.irRear);
  Serial.println(messageOut.wheelTicks);*/
  delay(1000);////
}

//checking serial for new messages from Odroid
void serialEvent() {
  while (Serial.available()) {
    //insert while loop for when not rc control maybe
    //while(!rcControl){}
    OdroidToArduino messageIn;
    uint8_t bufferIn[64];

    // Loops through the data from serial and writes into bufferIn
    for (int i = 0; i < streamInSize; i++) {
      bufferIn[i] = Serial.read();
    }
    // Creates input stream from bufferIn
    pb_istream_t streamIn = pb_istream_from_buffer(bufferIn, sizeof(bufferIn));

    bool receiveStatus = pb_decode(&streamIn, OdroidToArduino_fields, &messageIn);

    // If decoding was succesfull then message is parsed
    /* if (receiveStatus) {
       int spd = messageIn.spd;
       int angle = messageIn.angle;
     }
     // else {
     //    Serial.write("Error decoding input: ");
     //    Serial.write(Serial.read());
     //    Serial.println();
     //  }*/
  }
}

//helper functions
//prepare message to Odroid
ArduinoToOdroid prepareMessage() {
  ArduinoToOdroid message;
  message.usCentre = sonarCentreReading;
  message.usRight = sonarRightReading;
  message.irRight_one = getIrRange(ir1);
  message.irRight_two = getIrRange(ir2);
  message.irRear = getIrRange(ir3);
  message.wheelTicks = wheelTicks;
  return message;
}

//setup ultra sonic rangefinders
void setupSonars() {
  sonarCentre.connect(sonarCentreAddress, GAIN_REGISTER, LOCATION_REGISTER);
  sonarRight.connect(sonarRightAddress, GAIN_REGISTER, LOCATION_REGISTER);
}

//get readings from ultra sonic
void getSonarReadings() {
  sonarCentreReading = sonarCentre.getRange();
  if (sonarCentreReading > 0) {
    sonarCentreReading = constrain(sonarCentreReading, sonarMin, sonarMax);
    sonarCentreTemp = sonarCentreReading;
  } else {
    sonarCentreReading = sonarCentreTemp;
  }
  sonarRightReading = sonarRight.getRange();
  if (sonarRightReading > 0) {
    sonarRightReading = constrain(sonarRightReading, sonarMin, sonarMax);
    sonarRightTemp = sonarRightReading;
  } else {
    sonarRightReading = sonarRightTemp;
  }
}

//get range from infra red sensors
float getIrRange (int pin) {
  int tmp = analogRead(pin);
  //float tmp = averageReadings(pin);
  float range = (2914 / ((float)tmp + 5)) - 1;
  range = constrain(range, irMin, irMax);
  return range;
}

//counting wheel revolutions - 8 ticks = 1 revolution. 1 revolution = 21 cm.
void wheelEncoder()
{
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    wheelTicks++;
  }
  else
    wheelTicks--;
}

//check if rc receiver is getting any signal from rc handset
void rcControling() {
  if (rcPulseLength > rcPulseLengthMin) {
    rcControl = true;
  }
}

//checks if there is rc input, if not car is controlled by HLB
void rcInput() {
  if (!rcControl) {
    //no RC control not detected
    //rcControl = false;
    //setAngle(0);
    //setSpd(0);
    //getting updated values fro Odroid where here
  } else {
    //when RC control is on
    if (rcServoPulseLength) {
      remainderA = abs(rcPulseIdle - rcServoPulseLength);
      if (remainderA < angleVariance) {
        if (!rcAngleReset) {
          servo.write(rcPulseIdle);
          rcAngleReset = true;
        }
      } else {
        int tmp = map(rcServoPulseLength, rcServoMin, rcServoMax, angleLeft, angleRight);
        servo.write(tmp);
        rcAngleReset = true;
      }
    }
    if (rcEscPulseLength) {
      remainderS = abs(rcPulseIdle - rcEscPulseLength);
      if (remainderS < spdVariance) {
        if (!rcSpeedReset) {
          esc.writeMicroseconds(rcPulseIdle);
          rcSpeedReset = true;
        }
      } else {
        int tmp = map(rcEscPulseLength, rcEscMin, rcEscMax, rcSpeedMin, rcSpeedMax);
        esc.writeMicroseconds(tmp);
        rcSpeedReset = false;
      }
    }
  }
}

void rcGetPulseLengths() {
  rcPulseLength = pulseIn(rcPin, HIGH);
  rcServoPulseLength = pulseIn(rcServoPin, HIGH);
  rcEscPulseLength = pulseIn(rcEscPin, HIGH);
}

void setupRcpins() {
  pinMode(rcPin, INPUT);
  pinMode(rcServoPin, INPUT);
  pinMode(rcEscPin, INPUT);
}

void setSpd(int spd) {
  spd = constrain(speedIdle + spd, speedMin, speedMax);
  esc.write(spd);
}
void setAngle(int angle) {
  angle = constrain(angleStraight + angle, angleLeft, angleRight);
  servo.write(angle);
}










