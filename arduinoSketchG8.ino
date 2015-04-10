
//----Special Bytes ----
const byte endByte = 'f';
const byte startByte = 's';

//----Incoming Data ----
byte recSignal[8];         // a byte array to hold incoming data
int speedIn = 0;               //incoming speed data
int steeringIn = 90;            //incoming steering data
//byte lightsIn = 0;
boolean recComplete = false; //Check if valid packet

//----Old Incoming Data ----
int speedInOld;               //Last changed speed data
int steeringInOld;            //Last changed steering data
//byte lightsInOld

//----Outgoing Data ----
byte sendSignal[16];         // a byte array to hold outgoing data
int speedOut;              //outgoing speed data from Wheel encoder
int steeringOut;           //outgoing steering data (current setting)
int ir1Out;                //IR sensor 1 outgoing
int ir2Out;                //IR sensor 2 outgoing
int us1Out;                //US sensor 1 outgoing
int us2Out;                //US sensor 2 outgoing
byte lightsOut = 0;





void setup() {
  // initialize serial:
  Serial.begin(9600);
  
  // TODO - A blocking function that waits for a predefined signal from the Odroid here or in loop
  // Possibly use the handshake example?
}

void loop() {
  // print the string when a newline arrives:
  if (recComplete) {
    distIncoming();
    //Set Speed data if different from current
    if(speedIn != speedInOld){
      setTheSpeed();
      speedInOld = speedIn;
    }
    //Set Speed data if different from current - currently using old value, maybe use wheel encoder?
    if(steeringIn != steeringInOld){
      setTheSteering();
      steeringInOld = steeringIn;
    }    
    recComplete = false;
  }
  //Gather sensor Data
  getSpeed();
  getSteering();
  getIR1();
  getIR2();
  getUS1();
  getUS2();
  sendData();
}

//Distribute incoming instructions to arduino structures
void distIncoming(){
  speedIn = recSignal[1] + ((int)recSignal[2] << 8);
  steeringIn = recSignal[3] + ((int)recSignal[4] << 8);
  //lightsIn = recSignal[5]
}

//Build buffer for serial out, send it over the wire
void sendData(){
  sendSignal[15] = 0;
  byte i = 0;
  sendSignal[0] = startByte;
  sendSignal[1] = speedOut & 0xFF;
  sendSignal[2] = (speedOut >> 8) & 0xFF;
  sendSignal[3] = steeringOut & 0xFF;
  sendSignal[4] = (steeringOut >> 8) & 0xFF;
  sendSignal[5] = ir1Out & 0xFF;
  sendSignal[6] = (ir1Out >> 8) & 0xFF;
  sendSignal[7] = ir2Out & 0xFF;
  sendSignal[8] = (ir2Out >> 8) & 0xFF;
  sendSignal[9] = us1Out & 0xFF;
  sendSignal[10] = (us1Out >> 8) & 0xFF;
  sendSignal[11] = us1Out & 0xFF;
  sendSignal[12] = (us2Out >> 8) & 0xFF;
  sendSignal[13] = lightsOut;
  sendSignal[14] = endByte;
  for(i = 0; i < 15; i++){
    sendSignal[15] ^= sendSignal[i];
  } 
  Serial.write(sendSignal, 15);
}

//Set the speed controller
void setTheSpeed(){
  //TODO
}

//Set the steering servo angle
void setTheSteering(){
  //TODO
}

//void setTheLights(){
//  //TODO
//}

//Get speed from wheel encoder
void getSpeed(){
  //TODO
}

//Check current steering data (uneccessary maybe?)
void getSteering(){
  //TODO
}

//get first IR Sensor
void getIR1(){
  //TODO
}

//get second IR Sensor
void getIR2(){
  //TODO
}

//get first Ultra Sonic Sensor
void getUS1(){
  //TODO
}

//get second Ultra Sonic Sensor
void getUS2(){
  //TODO
}


//interrupt style function, get serial from Odroid and store
void serialEvent() {
  byte i = 0;
  byte check = 0;
  byte findEnd;
  while (Serial.available() && i < 8) {
    recSignal[i] = Serial.read();
    check ^= recSignal[i];
    i++;
  }
  if(i==8 && recSignal[0]== startByte && recSignal[6] == endByte && check == 0){
    recComplete=true;
  }
  while (Serial.available() && findEnd != 'f') {
    findEnd = Serial.read();
  }
  Serial.read();
}
