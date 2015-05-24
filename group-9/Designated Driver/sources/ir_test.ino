

int a = 2914; 
int distance, distance2; 
int ir_back = 14;
int ir_rfront = 13;
int ir_rback = 15;
int voltage, voltage2;


void setup () 
{ 
  pinMode(ir_rfront, INPUT);
  pinMode(ir_rback, INPUT);
  pinMode(ir_back, INPUT);
  Serial.begin (38400); 
}

void loop () 
{ 
  int IR_FR = getDist(ir_rfront);
  int IR_BR = getDist(ir_rback);
  int IR_Back = getDist(ir_back);
  delay(100);
  
  String IR_FR_Str = String(IR_FR);
  String IR_BR_Str = String(IR_BR);
  String IR_Back_Str = String(IR_Back);
  String to_print = "Front: " + IR_FR_Str + " RightBack: " + 
  IR_BR_Str + " Back: " + IR_Back_Str;
  Serial.println(to_print);
  
}

int getDist(int ir) {
  voltage = analogRead(ir);
  distance = (a/(voltage+5))-1; 
  //Serial.println("FrontRight: " + distance2);
  delay(200);
  return (distance);
}
