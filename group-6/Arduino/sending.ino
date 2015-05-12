// DIGITAL
#define ECHO_SIDE 8
#define TRIGGER_SIDE 9
#define ECHO_FRONT 10
#define TRIGGER_FRONT 11

// ANALOG
#define IR_FRONT 0
#define IR_SIDE 1
#define IR_BACK 2

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
 sprintf(memory, "s%04lu%04lu%03d%03d%03d",
   sonar(ECHO_FRONT, TRIGGER_FRONT),
   sonar(ECHO_SIDE, TRIGGER_SIDE),
   analogRead(IR_FRONT),
   analogRead(IR_SIDE),
   analogRead(IR_BACK)
 );
 for(int i = 17; i > 0; i--) checksum += (memory[i] - '0');
 memory[18] = String(checksum % 10)[0];
 Serial.write(memory);
 delay(50);
}

// measure distance (in cm) based on the speed of sound.
unsigned long sonar(int echo, int trigger) {
 digitalWrite(trigger, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigger, HIGH);
 delayMicroseconds(10); 
 unsigned long ms = pulseIn(echo, HIGH) / 2;
 if(ms < 10000) return ms;
 return 9999;
}