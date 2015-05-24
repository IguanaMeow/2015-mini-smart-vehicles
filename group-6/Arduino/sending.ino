#include <NewPing.h>

// DIGITAL
#define ECHO_SIDE 12
#define TRIGGER_SIDE 13
#define ECHO_FRONT 9
#define TRIGGER_FRONT 10

// ANALOG
#define IR_FRONT 0
#define IR_SIDE 1
#define IR_BACK 2

#define MAX_DISTANCE 200 // in cm

NewPing sonarFront(TRIGGER_FRONT, ECHO_FRONT, MAX_DISTANCE);
NewPing sonarSide(TRIGGER_SIDE, ECHO_SIDE, MAX_DISTANCE);

char memory[20];

void setup() {
  Serial.begin(9600);
  pinMode(TRIGGER_FRONT, OUTPUT);
  pinMode(TRIGGER_SIDE, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(ECHO_SIDE, INPUT);
}

void loop() {
 int checksum = 0;
 sprintf(memory, "s%04d%04d%03d%03d%03d",
   sonarFront.ping(),
   sonarSide.ping(),
   analogRead(IR_FRONT),
   analogRead(IR_SIDE),
   analogRead(IR_BACK)
 );
 for(int i = 17; i > 0; i--) checksum += (memory[i] - '0');
 memory[18] = String(checksum % 10)[0];
 Serial.write(memory);
 delay(50);
}
