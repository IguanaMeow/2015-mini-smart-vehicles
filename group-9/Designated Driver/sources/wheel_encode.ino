//author Malcolm Kente
int pin = 2;
int value = 0;
int newDetection = 0;
int noBlack = 0;
void setup()
{
  pinMode(2, INPUT);
  attachInterrupt(0, distance, CHANGE);
  Serial.begin(9600); // initialize
}
void loop()
{
  value = digitalRead(pin); //set value to read data from pin

  if(value == 0) {
      if(newDetection==1){
        newDetection=0;
        noBlack++;
      }
      Serial.println("Detecting");
    }
    else {
      if(newDetection==0){
        newDetection=1;
      }
      Serial.println("");
    }
  delay(10);
  Serial.println(noBlack);
}
void distance()
{
  value++;
}
