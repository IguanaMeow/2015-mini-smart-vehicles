/*
AUTHORS:
 Giorgi Dundua
 Saimonas Sileikis
 Anna Graduleva
 */
#include <CarVariables.h>
//NanoPB related
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <message.pb.h>

//Servo and ESC related
#include <Servo.h>
Servo esc;
Servo servo;

void setup() {
  esc.attach(escPin);
  servo.attach(servoPin);
  setSpd(spd);
  setAngle(angle);

  setupSonars();

  attachInterrupt(0, wheelEncoder, CHANGE);

  Serial.begin(19200); //9600
  //Serial.setTimeout(200); ///200 sets the maximum milliseconds to wait for serial data when using
}

void loop() {
  rcInput();
  getSonarReadings();
  sendMessage ();
  delay(1000);
}

//checking serial for new messages from Odroid
void serialEvent() {
  while (Serial.available()) {
    if (Serial.available()) {
      receiveMessage();
    }
  }
}












