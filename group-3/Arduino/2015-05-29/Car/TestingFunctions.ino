//prints sensor readings
void printSensorReadings (ArduinoToOdroid message) {
  Serial.println(message.usCentre);
  Serial.println(message.usRight);
  Serial.println(message.irRight_one);
  Serial.println(message.irRight_two);
  Serial.println(message.irRear);
  Serial.println(message.wheelTicks);
}

void printCarControls (OdroidToArduino message) {
  Serial.println(message.spd);
  Serial.println(message.angle);
}
