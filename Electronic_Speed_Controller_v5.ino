const int pwmPin1 = 6;
const int pwmPin2 = 9;
const int pwmPin3 = 3;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  pinMode(pwmPin1,OUTPUT);
  pinMode(pwmPin2,OUTPUT);
  pinMode(pwmPin3,OUTPUT);
}

unsigned long long int prevMicros = 0;

// float highRatio = 1;

int freq = 200; //in Hz

int totalPeriod = 1000000 / freq;

void loop() {
  uint32_t Current_Micros=micros();
  if (Current_Micros >= (prevMicros + totalPeriod)) {
    prevMicros = micros();
  }
  if (Current_Micros < (prevMicros + (totalPeriod / 6))) {
    digitalWrite(pwmPin1,1);
    digitalWrite(pwmPin2,0);
    digitalWrite(pwmPin3,1);
  }
  else if (Current_Micros > (prevMicros + (totalPeriod / 6)) && Current_Micros < (prevMicros + (totalPeriod / 3))) {
    digitalWrite(pwmPin1,1);
    digitalWrite(pwmPin2,0);
    digitalWrite(pwmPin3,0);
  }
  else if (Current_Micros > (prevMicros + (totalPeriod / 3)) && Current_Micros < (prevMicros + (totalPeriod / 2))) {
    digitalWrite(pwmPin1,1);
    digitalWrite(pwmPin2,1);
    digitalWrite(pwmPin3,0);
  }  
  else if (Current_Micros > (prevMicros + (totalPeriod / 2)) && Current_Micros < (prevMicros + (2 * totalPeriod / 3))) {
    digitalWrite(pwmPin1,0);
    digitalWrite(pwmPin2,1);
    digitalWrite(pwmPin3,0);
  }  
  else if (Current_Micros > (prevMicros + (2 * totalPeriod / 3)) && Current_Micros < (prevMicros + (5 * totalPeriod / 6))) {
    digitalWrite(pwmPin1,0);
    digitalWrite(pwmPin2,1);
    digitalWrite(pwmPin3,1);
  }  
  else if (Current_Micros > (prevMicros + (5 * totalPeriod / 6)) && Current_Micros < (prevMicros + totalPeriod)) {
    digitalWrite(pwmPin1,0);
    digitalWrite(pwmPin2,1);
    digitalWrite(pwmPin3,1);
  }  
}
