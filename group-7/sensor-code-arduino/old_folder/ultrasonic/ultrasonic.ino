#include <SonarSRF.h>
#include <Wire.h>
#define ultrasonicAddress (0xE4 >> 1) //sensor's adress (right) 
#define ultrasonicAddress1 (0x70) //(right)
#define sensorpin 0 //pin for right IR
#define sensorpin2 2 // pin for the back IR           
#define GAIN 0x09 // Setup Analogue Gain
#define LOCATION 0x8C // Setup Range Location
int Distance, Distance1, val,val2;
SonarSRF MainSonar, secondSonar;
char unit = 'c'; // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds
float sensorReading = 0;
float sensorReading2 = 0;

void setup() {
    Serial.begin(9600);

    MainSonar.connect(ultrasonicAddress,GAIN, LOCATION);
    secondSonar.connect(ultrasonicAddress1, GAIN, LOCATION);
    
}

void loop() {
    
    sensorReading = MainSonar.getRange(unit, true);
    sensorReading2 = secondSonar.getRange(unit, true);
    distance("sensor", sensorReading);
    distance("sensor2", sensorReading2);
    val = analogRead(sensorpin);  // reads the value of the sharp sensor
    val2 = analogRead(sensorpin2);
    Distance1 = ((2914/(val + 5) - 1)); //converts to cm
    Distance = ((2914/(val2 + 5) - 1)); 
    Serial.print("Back ");
    Serial.println(Distance);
    Serial.print("Right ");
    Serial.println(Distance1);  // prints the value of the sensor to the serial monitor
    delay(100);                 // wait for this much time before printing next value
   
}

// Print out distance
void distance(String reference, float sensorReading) {
    Serial.print("Distance from " + reference + ": ");
    Serial.print(sensorReading);
    Serial.println(unit);
}



