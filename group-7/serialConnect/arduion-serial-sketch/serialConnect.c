// Define the pins for the IR sensors
#define IR_pin 0

// Define the pins for the US sensors
#define USRF_pin 9

// Define the netstring data to be transfered to odroid
String myData = "5:Z,";
int len;
int infraredDist_cm;

void setup() {
        Serial.begin(9600);     // opens serial port, sets baud rate to 9600 bps
}

void loop() {
   // 1) read data from IR and store in myData
               int voltReading = analogRead(IR_pin);
               infraredDist_cm = ((2914/(voltReading + 5) - 1));

   // 2) read data from us and store in myData

   // 3) Put data in array
              //theData[] = infraredDist_cm;

    // 4) get length of value
              len = myData.length();

    // 5) NetStrings Protocol ENCODE: write length into a string followed by colon
            // concat data to string after the colon
            // end with comma

    // 6) Send myData to serialPort
    for(int i = 0; i < len; i++) {
        Serial.write(myData[i]);
    }

}

// **** NOTES ****//
// Will need to specify which sensor the data is coming from.
