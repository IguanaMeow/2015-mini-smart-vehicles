
volatile unsigned long ticks = 0; 

void setup()
{
  Serial.begin(9600);
   
  attachInterrupt(5, encoderInterrupt, CHANGE); // int 5 represents pin 18 on mega
}

void loop()
{
  Serial.println(ticks);
  delay(100);
}

// Interrupt service routine for the wheel encoder
void encoderInterrupt()
{
  ticks++;
}
