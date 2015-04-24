// Define the pins for the IR sensors
#define IR_pin 0 // Define the pins for the US sensors
#define USRF_pin 9// Define the netstring data to be transfered to odroid

  int len;
int infraredDist_cm;


void setup() {
 
     Serial.begin(9600);     // opens serial port, sets baud rate to 9600 bps
}    
   
   
String payload = "ILoveSerialConnections";
           
void loop() {
  // 1) read data from IR and store in myData
           int voltReading = analogRead(IR_pin);
           infraredDist_cm = ((2914/(voltReading + 5) - 1));
           //payload = infraredDist_cm;
             
  // 2) read data from us and store in myData
          //TODO:
         
  // 3)   Put data in array
          //TODO: ???
         
   // 4) get length of payload
           len = payload.length();
   
   // 5) NetStrings Protocol ENCODE: write length into a string followed by colon
           // concat data to string after the colon
           // end with comma
           String result = ":" + payload;
           String finalResult = (len + result);
           String definitelyTheFinalResult = (finalResult + ",");
                   
   // 6) Send payload to serialPort
           Serial.print(definitelyTheFinalResult);  
}
// **** NOTES ****//
// Will need to specify which sensor the data is coming from. 
