ArduinoToOdroid prepareMessage() {
  ArduinoToOdroid message;
  message.usCentre = sonarCentreReading;
  message.usRight = sonarRightReading;
  /*message.irRight_one = average(ir1, readings1, index1, total1);
  message.irRight_two = averageReadings(ir2);
  message.irRear = getIrRange(ir3);*/
  message.irRight_one = getIrRange(ir1);
  message.irRight_two = getIrRange(ir2);
  message.irRear = getIrRange(ir3);
  message.wheelTicks = wheelTicks;
  return message;
}

//encode message and send over serial
void sendMessage () {
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
      //printSensorReadings(messageOut);  //uncomment for testing sensor readings
    }
  }
  /*else {    //uncomment for testing
    Serial.write("Error encoding input: ");
    Serial.write(Serial.read());
    Serial.println();
  }*/
}

//get buffer from serial and decode
void receiveMessage(){
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
  if (receiveStatus) {
    int spd = messageIn.spd;
    int angle = messageIn.angle;
    //printCarControls(messageIn);  //uncomment for testing received controls
  }
  /*else {    //uncomment for testing    //uncomment for testing
    Serial.write("Error decoding input: ");
    Serial.write(Serial.read());
    Serial.println();
  }*/
}




