#include <Wire.h>

// address of srf08 ultrasonic sensor 
#define srfAddress1 0x73
// digital pins used to connect srf04 ultrasound sensor
int trigPin = 2;   
int echoPin= 3; 

// analog pin used to connect the sharp sensor
int irPin1 = 0;
int irPin2 = 1;
int irPin3 = 2;

// variable decleration for infrared
int ir1_value, ir2_value, ir3_value;
String ir1_str, ir2_str, ir3_str;
String ir1_data, ir2_data, ir3_data;
// variable decleration for ultrasonic
int us1_value, us2_value;
String us1_str, us2_str;
String us1_data, us2_data;

int distance;
long val, duration;
String sensorData, netString;



void setup(){
  Wire.begin();
  Serial.begin(9600); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  delay(100);  
}
 
void loop(){
  //read infrared sensors
  ir1_value = getInfrared(irPin1);
  ir2_value = getInfrared(irPin2);
  ir3_value = getInfrared(irPin3);
  
    //convert values to string
  ir1_str = String(ir1_value);
  ir2_str = String(ir2_value);
  ir3_str = String(ir3_value);
  
  //convert values to 4 digit length string
  ir1_data = fourDigit(ir1_str);
  ir2_data = fourDigit(ir2_str);
  ir3_data = fourDigit(ir3_str);
  
  // read ultrasonic sensors
  us1_value = getUltrasonic1(srfAddress1);
  us2_value = getUltrasonic2(trigPin, echoPin);
   
  //convert values to string
  us1_str = String(us1_value);
  us2_str = String(us2_value);
  
  //convert values to 4 digit length string  
  us1_data = fourDigit(us1_str);
  us2_data = fourDigit(us2_str);
  
  sensorData = ir1_data + " " + ir2_data + " " + ir3_data + " " + us1_data + " " + us2_data;
  
  
  //encode sensor data to netstring
  netString = String(sensorData.length()) + ":" + sensorData + ",";
  Serial.println(netString);
  delay(1000);
}


int getInfrared(int pin){
  // reads the value of the sharp sensor
  val = analogRead(pin); 
  // convert voltage to length (cm)  
  distance= (2914 / (val + 5)) - 1;
  return distance;
}


int getUltrasonic1(int srfAddress){
  // source: http://www.arduino.cc/en/Tutorial/SFRRangerReader  
  Wire.beginTransmission(srfAddress);             
  Wire.write(byte(0x00));                             
  Wire.write(0x51);                                
  Wire.endTransmission();
  delay(100);                                     
  Wire.beginTransmission(srfAddress);            
  Wire.write(byte(0x02));                           
  Wire.endTransmission();
 
  Wire.requestFrom(srfAddress, 2);               
  while(Wire.available() < 2);                    
  byte highByte = Wire.read();                 
  byte lowByte = Wire.read();                  
 
  distance = (highByte << 8) + lowByte;              // Put them together
 
  return(distance);                                  // Returns Range
}

int getUltrasonic2(int trigPin, int echoPin){
  long duration,  cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  // convert the time into a distance
  distance = duration/58;
  return distance;
}

String fourDigit(String str){
  if(str.length() == 1){
    str = "000" + str;
  }
  else if(str.length() == 2){
    str = "00" + str; 
  }
  else if(str.length() == 3){
    str = "0" + str;
  }
  return str;
}
