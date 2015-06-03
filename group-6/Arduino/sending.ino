#include <NewPing.h>

// DIGITAL
#define ECHO_SIDE 12
#define TRIGGER_SIDE 13
#define ECHO_FRONT 9
#define TRIGGER_FRONT 10

// ANALOG
#define IR_FRONT 1
#define IR_MIDDLE 2
#define IR_BACK 3

#define MAX_DISTANCE 2000


NewPing sonarFront(TRIGGER_FRONT, ECHO_FRONT, MAX_DISTANCE);

NewPing sonarSide(TRIGGER_SIDE, ECHO_SIDE, MAX_DISTANCE);

int measure(int pin) {
  return (2914 / (analogRead(pin) + 5)) - 1;
}

char memory[20];
int v1, v2, v3, v4, v5, iterator = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_FRONT, OUTPUT);
  pinMode(TRIGGER_SIDE, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(ECHO_SIDE, INPUT);
  delay(100);
}

void loop() {
  if(iterator == 0) { v1 = 0; v2 = 0; v3 = 0; v4 = 0; v5 = 0; }
  
  v1 += sonarFront.ping_cm();
  v2 += sonarSide.ping_cm();
  v3 += measure(IR_FRONT);
  v4 += measure(IR_MIDDLE);
  v5 += measure(IR_BACK);
  
  iterator++;
  
  if(iterator < 10) return;
 
 sprintf(memory, "s%04d%04d%03d%03d%03d", v1 / 10, v2 / 10, v3 / 10, v4 / 10, v5 / 10);
 
 int checksum = 0;
 for(int i = 17; i > 0; i--) checksum += (memory[i] - '0');
 memory[18] = String(checksum % 10)[0];
 
 Serial.write(memory);
 
 iterator = 0;
 
 delay(50);
}
