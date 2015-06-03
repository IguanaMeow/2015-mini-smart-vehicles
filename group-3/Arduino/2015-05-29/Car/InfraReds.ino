//averaging out readings
// use this value to determine the size of the readings array.
//get range from infra red sensors
float getIrRange (int pin) {
  int tmp = analogRead(pin);
  //float tmp = averageReadings(pin);
  float range = (2914 / ((float)tmp + 5)) - 1;
  range = constrain(range, irMin, irMax);
  return range;
}

