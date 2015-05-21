#include <Servo.h> 

#define MAX_SPEED 40
#define MAX_DIRECTION 50

#define SERVO_HIGH 2000
#define SERVO_LOW 700
#define SERVO_PIN 11

#define ESC_HIGH 1900
#define ESC_LOW 900
#define ESC_PIN 12

Servo servo, esc;

char memory[5];  // memory for incoming message

int index = 0, // index of incoming digit in memory
    speed = MAX_SPEED / 2, // start at medium speed
    direction = MAX_DIRECTION / 2; // and center direction

void setup() {
  servo.attach(SERVO_PIN);
  esc.attach(ESC_PIN);
  Serial.begin(9600);
}

void loop() {
  // calculate and send steering values to servo and esc 
  servo.writeMicroseconds(direction * (SERVO_HIGH - SERVO_LOW) / MAX_DIRECTION + SERVO_LOW);
  esc.writeMicroseconds(ESC_HIGH - speed * (ESC_HIGH - ESC_LOW) / MAX_SPEED);

  // loop until we have a value
  if(Serial.available() <= 0) return;
  char value = Serial.read();
  
  // reset if not a digit
  if(!isdigit(value)) {
    index = 0;
    return;
  }
  
  // put digit in memory and increment index
  memory[index] = value;
  index++;
  
  // if full message in memory; decode it
  if(index == 5) decode();

}

void decode(){

  int d0= memory[0] - '0',
      d1 = memory[1] - '0',
      d2 = memory[2] - '0',
      d3 = memory[3] - '0',
      d4 = memory[4] - '0'; // data quality check digit

  // reset index for the next one
  index = 0;

  // verify data is correct using the check digit
  // digit should equal the remainder of the sum of preceeding digits divided by 10
  if((d0 + d1 + d2 + d3) % 10 != d4) return;

  // update speed and direction
  direction = d0 * 10 + d1;
  speed = d2 * 10 + d3;

  // don't exceed max values
  if(direction > MAX_DIRECTION) direction = MAX_DIRECTION;
  if(speed > MAX_SPEED) speed = MAX_SPEED;

}