int sensor = 1;
float sensorValue = 0;
#define VoltsToCentimeters(volts) ((2914 / (volts + 5)) - 1)

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600); 
}

void loop() {
  
  
  
  sensorValue = analogRead(sensor);
  int millivolts= (sensorValue/1024.0) * 5000;
  int distance = VoltsToCentimeters(sensorValue);
  //Serial.println(sensorValue);
  if(distance < 40){
  Serial.print("Distance is: ");
  Serial.print(distance);
  Serial.print(" cm");
  Serial.println();
  }
  //Serial.println(millivolts);
  if(millivolts > 2600 && millivolts <= 2800){
    Serial.println("Distance is 4 cm");
   } 
   else if ( millivolts > 1500 && millivolts < 1600){
     Serial.println("Distance is 8 cm");
    } 
    
  delay(50);
}
