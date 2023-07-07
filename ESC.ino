int INA = 3;
int INB = 5;
int INC = 6;

void stepA(){
  digitalWrite(INA,1);
  digitalWrite(INB,0);
  digitalWrite(INC,0);
}

void stepB(){
  digitalWrite(INA,0);
  digitalWrite(INB,1);
  digitalWrite(INC,0);
}

void stepC(){
  digitalWrite(INA,0);
  digitalWrite(INB,0);
  digitalWrite(INC,1);
}

double delayTime(double rps, int coil){
  return rps / (coil * 3) * 1000000;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(INA,OUTPUT);
  pinMode(INB,OUTPUT);
  pinMode(INC,OUTPUT);
}

long long int currentTime = micros();
double delaytime = delayTime(5, 12);
int currentStep = 0;
  
void loop() {
  if (micros() > (currentTime + delaytime)){
    if (currentStep == 0){
      stepA();
      currentStep = 1;
    }
    else if (currentStep == 1){
      stepB();
      currentStep = 2;
    }
    else if (currentStep == 2){
      stepC();
      currentStep = 0;
    }
    currentTime = micros();
    //print(currentTime);
  }
}
