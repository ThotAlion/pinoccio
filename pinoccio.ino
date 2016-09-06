//this Arduino Mega pilots 5 stepper motors in position with respect to a mechanical stop button. It matches with an RAMPS 1.4 card
// There is a finite state machine for each stepper
int M_STEP_PIN[] = {54,60,46,26,36};
int M_DIR_PIN[] = {55,61,48,28,34};
int M_ENABLE_PIN[] = {38,56,62,24,30};
int M_STOP_PIN[] = {3,2,14,15,18};
int LED_PIN = 13;
//Declaration
int pcur[] = {0,0,0,0,0};
int pcom[] = {0,0,0,0,0};
int state[] = {0,0,0,0,0};

int serialState = 0;
String strmot = "";
String strpas = "";
// minimal pulse duration for stepper in micro second
int minPulseMicroS = 3;

void setup() {
  Serial.begin(9600);
  
  for(int m=0;m<=4;m++){
    pinMode( M_STEP_PIN[m], OUTPUT );
    pinMode( M_DIR_PIN[m]   , OUTPUT );
    pinMode( M_ENABLE_PIN[m]  , OUTPUT );
    pinMode( M_STOP_PIN[m]  , INPUT );
    digitalWrite( M_STEP_PIN[m],LOW );
    digitalWrite( M_DIR_PIN[m],HIGH );
    digitalWrite( M_ENABLE_PIN[m],LOW);
  }
}

void loop() {
  for(int m=0;m<=4;m++){
    switch (state[m]){
      case 0:
        // go up until the switch
        digitalWrite( M_STEP_PIN[m],HIGH );
        delayMicroseconds(minPulseMicroS);
        digitalWrite( M_STEP_PIN[m],LOW );
        if(digitalRead(M_STOP_PIN[m])==HIGH){
          state[m]=1;
        }
        break;
        
      case 1:
        // is at the switch limit
        pcur[m]=1;
        pcom[m]=1;
        state[m]=2;
        break;
      case 2:
        // control
        if(pcur[m]<pcom[m]){
          digitalWrite( M_DIR_PIN[m],LOW );
          delayMicroseconds(minPulseMicroS);
          digitalWrite( M_STEP_PIN[m],HIGH );
          delayMicroseconds(minPulseMicroS);
          digitalWrite( M_STEP_PIN[m],LOW );
          pcur[m] = pcur[m]+1;
        }
        else if(pcur[m]>pcom[m]){
          digitalWrite( M_DIR_PIN[m],HIGH );
          delayMicroseconds(minPulseMicroS);
          digitalWrite( M_STEP_PIN[m],HIGH );
          delayMicroseconds(minPulseMicroS);
          digitalWrite( M_STEP_PIN[m],LOW );
          pcur[m] = pcur[m]-1;
        }
        if(digitalRead(M_STOP_PIN[m])==HIGH){
          pcur[m] = 0;
        }
        if (pcom[m]==0){
          state[m]=0;
        }
        break;
    }
  }
  delay(1);
}
  
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    switch (serialState){
      case 0:
        if(inChar=='M'){
          strmot="";
          strpas="";
          serialState = 1;
        }
        break;
      case 1:
        if(inChar=='P'){
          serialState=2;
        }
        else{
          strmot+=inChar;
        }
        break;
      case 2:
        if(inChar=='\n'){
          if(strmot.toInt()<=5){
            pcom[strmot.toInt()-1] = strpas.toInt();
            Serial.println("got it!");
          }
          serialState = 0;
        }
        else{
          strpas+=inChar;
        }
        break;
    }
  }
}
