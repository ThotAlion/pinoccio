//this Arduino Mega pilots 5 stepper motors in position with respect to a mechanical stop button. It matches with an RAMPS 1.4 card
// There is a finite state machine for each stepper
int M_STEP_PIN[] = {54,60,46,26,36};
int M_DIR_PIN[] = {55,61,48,28,34};
int M_ENABLE_PIN[] = {38,56,62,24,30};
int M_STOP_PIN[] = {3,2,14,15,18};
int LED_PIN = 13;
int JUMP_I = 23;
int JUMP_O = 25;

//Declaration
int pcur[] = {0,0,0,0,0};
int pcom[] = {1,1,1,1,1};
int state[] = {0,0,0,0,0};
int ncycle[] = {100,100,100,100,100};
int tps[] = {20,20,20,20,20};

int serialState = 0;
String strmot = "";
String strpas = "";
int mode = 0;
// minimal pulse duration for stepper in micro second
int minPulseMicroS = 1;
// time of step in micro seconds
int sampling_period = 5;
// initial speed for acceleration
int tps_init = 100;
// dead zone
int zone = 100;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode( JUMP_O, OUTPUT );
  pinMode( JUMP_I  , INPUT );
  digitalWrite( JUMP_O,LOW );
  if(digitalRead(JUMP_I)==HIGH){
    mode=1;
  }
  digitalWrite( JUMP_O,HIGH );
  if(digitalRead(JUMP_I)==LOW){
    mode=1;
  }
  
  for(int m=0;m<=4;m++){
    pinMode( M_STEP_PIN[m], OUTPUT );
    pinMode( M_DIR_PIN[m]   , OUTPUT );
    pinMode( M_ENABLE_PIN[m]  , OUTPUT );
    pinMode( M_STOP_PIN[m]  , INPUT );
    digitalWrite( M_STEP_PIN[m],LOW );
    digitalWrite( M_DIR_PIN[m],HIGH );
    digitalWrite( M_ENABLE_PIN[m],LOW);
    digitalWrite(M_STOP_PIN[m], HIGH);
  }
}

void loop() {
  for(int m=0;m<=4;m++){
    switch (state[m]){
      case 0:
        // disable the motor
        digitalWrite( M_ENABLE_PIN[m],HIGH);
        if (pcom[m]==0){
          tps[m] = tps_init;
          ncycle[m] = tps[m];
          state[m]=1;
        }
        break;
        
      case 1:
        // go up until the switch
        digitalWrite( M_ENABLE_PIN[m],LOW);
        digitalWrite( M_DIR_PIN[m],HIGH );
        if(ncycle[m]<=0){
          tps[m] = tps[m]-1;
          if(tps[m]<=minPulseMicroS+1){
            tps[m]=minPulseMicroS+1;
          }
          ncycle[m]=tps[m];
        }
        else{
          if(ncycle[m]<=minPulseMicroS){
            digitalWrite( M_STEP_PIN[m],HIGH );
          }
          else{
            digitalWrite( M_STEP_PIN[m],LOW );
          }
          ncycle[m] = ncycle[m]-1;
        }
        if(digitalRead(M_STOP_PIN[m])==HIGH){
          state[m]=2;
        }
        if (pcom[m]==-1){
          state[m]=0;
        }
        break;
        
      case 2:
        // is at the switch limit
        pcur[m]=1;
        pcom[m]=1;
        ncycle[m] = tps_init;
        state[m]=3;
        break;
        
      case 3:
        // control
        // go up until the switch
        digitalWrite( M_ENABLE_PIN[m],LOW);
        if(ncycle[m]<=0){
          if(pcur[m]<pcom[m]){
            digitalWrite( M_DIR_PIN[m],LOW );
            pcur[m] = pcur[m]+1;
          }
          else if(pcur[m]>pcom[m]){
            digitalWrite( M_DIR_PIN[m],HIGH );
            pcur[m] = pcur[m]-1;
          }
          if(abs(pcur[m]-pcom[m])<zone){
            tps[m]=tps[m]+1;
          }
          else{
            tps[m]=tps[m]-1;
          }
          if(tps[m]>=tps_init){
            tps[m] = tps_init;
          }
          if(tps[m]<=minPulseMicroS+1){
            tps[m] = minPulseMicroS+1;
          }
          ncycle[m]=tps[m];
        }
        else{
          if(pcur[m]!=pcom[m]){
            if(ncycle[m]<=minPulseMicroS){
              digitalWrite( M_STEP_PIN[m],HIGH );
            }
            else{
              digitalWrite( M_STEP_PIN[m],LOW );
            }
            ncycle[m] = ncycle[m]-1;
          }
        }
        if (pcom[m]==0){
          tps[m] = tps_init;
          ncycle[m] = tps[m];
          state[m]=1;
        }
        if (pcom[m]==-1){
          state[m]=0;
        }
        break;
    }
  }
  delayMicroseconds(sampling_period);
}
  
void serialEvent() {
  if(mode==0){
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
             // Serial.println("got it!"); DEBUG SAVOIR SI CA BUG MAX DE RENVOYER DES DONNEES
            }
            else{
              Serial2.println("M"+strmot+"P"+strpas);
             // Serial.println("I transfer");
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
}

void serialEvent2() {
  if(mode==1){
    while (Serial2.available()) {
      char inChar = (char)Serial2.read();
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
            if(strmot.toInt()>=6){
              pcom[strmot.toInt()-6] = strpas.toInt();
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
}
