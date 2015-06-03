//check if rc receiver is getting any signal from rc handset
void rcControling() {
  if (rcPulseLength > rcPulseLengthMin) {
    rcControl = true;
  }
}

//checks if there is rc input, if not car is controlled by HLB
void rcInput() {
  rcGetPulseLengths();
  rcControling();
  if (!rcControl) {    
    if (Serial.available()) {
      receiveMessage ();
      setSpd(spd);
      setAngle(angle);      
    }
  } else {
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
        rcAngleReset = false;
      }
    }
    if (rcEscPulseLength) {
      remainderS = abs(rcPulseIdle - rcEscPulseLength);
      if (remainderS < spdVariance) {
        if (!rcSpeedReset) {
          esc.writeMicroseconds(rcPulseIdle);
          rcSpeedReset = true;
        }
        //esc.writeMicroseconds(rcPulseIdle);
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
  //Serial.println(rcPulseLength);    //uncomment for testing
  //Serial.println(rcServoPulseLength);    //uncomment for testing
  //Serial.println(rcEscPulseLength);    //uncomment for testing
}
