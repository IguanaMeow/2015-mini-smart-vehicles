unsigned long before;
unsigned long after;
unsigned long diff;

#define REAR_IR 0
#define FRONT_RIGHT_IR 2
#define REAR_RIGHT_IR 1
#define VoltsToCentimeters(volts) ((2914 / (volts + 5)) - 1)

int IR_front_right, IR_rear_right, IR_rear;

// this constant won't change.  It's the pin number
// of the sensor's output:
const int USFtrig = 5;
const int USFecho = 6;
const int USFRtrig = 4;
const int USFRecho = 7;


int sonarFront;
int sonarFrontRight;

void setup() {
  
  Serial.begin(9600);
  pinMode(USFecho, INPUT); 
  pinMode(USFtrig, OUTPUT);
  pinMode(USFRecho, INPUT);
  pinMode(USFRtrig, OUTPUT); 
  delay(100);
}

void loop()
{
   before= millis(); 
   String format = "";
   
     // IRT, IRB & IRR
    IR_front_right = IR_Distance(FRONT_RIGHT_IR);
    IR_rear_right = IR_Distance(REAR_RIGHT_IR);
    IR_rear = IR_Distance(REAR_IR);  

    sonarFront = US_getRange(USFtrig, USFecho);
    sonarFrontRight = US_getRange(USFRtrig, USFRecho); 
   
  
  
  // Creating temporary concatenation variables for each sensor
     String FUS = String(sonarFront);
     String FRUS = String(sonarFrontRight); 
     String IRT = String(IR_front_right);
     String IRB = String(IR_rear_right);
     String IRR = String(IR_rear);

      
     format = FUS + " " + FRUS + " " + IRT + " " + IRB + " " + IRR + " ";
     //  format = IRT + " " + IRB + " " + IRR + " ";
    
     Serial.println(format);  
    
    after=millis();
    diff = after - before;
    int time;
    
    if(diff < 50 )
    {
      time = 50 - diff;
      delay(time);
    }
}


// Auxiliary function to acquire IR distance: 
int IR_Distance(int IR_sensor)
{
  float sensorValue = 0;
  sensorValue = analogRead(IR_sensor);
  int distance = VoltsToCentimeters(sensorValue);
  
  if(distance < 40)
  {
    return(distance);
  }
  
  return(-1);
}

// Auxiliary function to get US range:
int US_getRange(int trigPin, int echoPin) 
{
  digitalWrite(trigPin, HIGH); //Trigger ultrasonic detection 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  int distance = pulseIn(echoPin, HIGH, 10000); //Read ultrasonic reflection
  distance= distance/58; //Calculate distance 
  
  if(distance <= 0 ) return -1;
  return distance;  
}


  