#define encoder0Pin 2

volatile int count, countold;

void setup()
{
  count = 0;
  countold = 0;
  pinMode(encoder0Pin, INPUT_PULLUP); 
  attachInterrupt(0, sensor1, FALLING);
  Serial.begin(38400); // initialize serial communication:
}

void loop()
{
  int x = wheelEncoder();
  Serial.println(x);
}

int wheelEncoder() {
  if (countold != count)
  {
    //Serial.println(count);
       countold = count;
  }
  return (countold);
}

void sensor1()
{
  count++;
}
