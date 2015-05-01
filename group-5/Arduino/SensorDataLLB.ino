// analog pin used to connect the sharp sensor
int irPin1 = 0;
int irPin2 = 1;
int irPin3 = 2;

int ir1_value, ir2_value, ir3_value;
String ir1_str, ir2_str, ir3_str;
String ir1_data, ir2_data, ir3_data;
String sensorData, netString;

void setup(){
  Serial.begin(9600);               
}
 
void loop(){
  ir1_value = infraredData(irPin1);
  ir2_value = infraredData(irPin2);
  ir3_value = infraredData(irPin3);
  
  //convert values to string
  ir1_str = String(ir1_value);
  ir2_str = String(ir2_value);
  ir3_str = String(ir3_value);
  
  //convert values to 4 digit length string
  ir1_data = fourDigit(ir1_str);
  ir2_data = fourDigit(ir2_str);
  ir3_data = fourDigit(ir3_str);
  
  sensorData = ir1_data + ", " + ", " + ir2_data + ", " + ", " + ir3_data;
  Serial.println(sensorData);
  
  //encode sensor data to netstring
  netString = sensorData.length() + ":" + sensorData + ",";
  
  delay(100);
}


int infraredData(int pin){
  int val = 0;
  // reads the value of the sharp sensor
  val = analogRead(pin); 
  // convert voltage to length (cm)  
  int distance= (2914 / (val + 5)) - 1;
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
