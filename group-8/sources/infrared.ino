#define infrared_pin1 0// Pin to which the distance sensor is connected
#define infrared_pin2 1
int infrared_value;          // Integer to hold the sensor’s reading

float infrared_scaledValue;  // Distance sensor’s scaled value

void setup ()
{
  Serial.begin (9600); // Initiates serial communication at 9600 baud
}

void loop ()
{

  infrared_value = analogRead (infrared_pin1);  // Reads the distance sensor
  infrared_scaledValue = ((float)infrared_value / 615) * 1024;  
  // Calculates the scaled value

  Serial.println ((int)infrared_scaledValue);  // Screams out the scaled value as integer
  delay (200);   // Delay of 200ms
  
  infrared_value = analogRead (infrared_pin2);  // Reads the distance sensor
  infrared_scaledValue = ((float)infrared_value / 615) * 1024;  
  // Calculates the scaled value

  Serial.println ((int)infrared_scaledValue);  // Screams out the scaled value as integer
  delay (200);
}
