unsigned long overrideRelease = 0;
const unsigned short OVERRIDE_TIMEOUT = 1000;

//check if rc receiver is getting any signal from rc handset
void rcControling() {
  if (rcPulseLength > rcPulseLengthMin) {
    overrideRelease = millis() + OVERRIDE_TIMEOUT;
    rcControl = true;
  }
}

//checks if there is rc input, if not car is controlled by HLB
void rcInput() {
  rcGetPulseLengths();
  rcControling();
  if (!rcControl || (millis() > overrideRelease)) {
    if (rcControl) {
      rcControl = false;
      setSpd(0);
      setAngle(0);
    }
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
        //servo.write(rcPulseIdle);
      } else {
        int tmp = map(rcServoPulseLength, rcServoMin, rcServoMax, angleLeft, angleRight);
        //int tmp = constrain(rcServoPulseLength, rcAngleLeft, rcAngleRight);
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
        //int tmp = constrain(rcEscPulseLength, rcSpeedMin, rcSpeedMax);
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
