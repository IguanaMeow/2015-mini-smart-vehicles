void setSpd(int spd) {
  spd = constrain(speedIdle + spd, speedMin, speedMax);
  if (spd < speedIdle){
    esc.writeMicroseconds(speedIdle);
    esc.writeMicroseconds(spd);
  } else {
    esc.writeMicroseconds(spd);
  }  
}

void setAngle(int angle) {
  angle = constrain(angleStraight + angle, angleLeft, angleRight);
  servo.write(angle);
}
