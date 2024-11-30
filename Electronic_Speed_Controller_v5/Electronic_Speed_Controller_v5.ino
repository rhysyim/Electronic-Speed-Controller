#include <cmath>

const int pwmPin1 = 16;
const int pwmPin2 = 17;
const int pwmPin3 = 18;
const int comparatorPin = 13;

unsigned long long int prevMicros = 0;
int freq = 10;
int totalPeriod = 1000000 / freq;

uint32_t Current_Micros = micros();

void setup() {
  Serial.begin(9600);
  pinMode(pwmPin1,OUTPUT);
  pinMode(pwmPin2,OUTPUT);
  pinMode(pwmPin3,OUTPUT);
  analogWriteFreq(10000);
  
  for (int freq = 10; freq <= 200; freq+=3) {
    Serial.println(freq);
    int totalPeriod = 1000000 / freq;
    for (int k = 0; k < 100*freq; k++) {
      Current_Micros = micros();
      int sinusoidal = abs(255 * sin(2 * M_PI * Current_Micros / (totalPeriod/6)));
      if (Current_Micros >= (prevMicros + totalPeriod)) {
        prevMicros = micros();
      }
      if (Current_Micros < (prevMicros + (totalPeriod / 6))) {
        analogWrite(pwmPin1, sinusoidal);
        digitalWrite(pwmPin2, 0);
        analogWrite(pwmPin3, sinusoidal);
      }
      else if (Current_Micros > (prevMicros + (totalPeriod / 6)) && Current_Micros < (prevMicros + (totalPeriod / 3))) {
        analogWrite(pwmPin1, sinusoidal);
        digitalWrite(pwmPin2,0);
        digitalWrite(pwmPin3,0);
      }
      else if (Current_Micros > (prevMicros + (totalPeriod / 3)) && Current_Micros < (prevMicros + (totalPeriod / 2))) {
        analogWrite(pwmPin1, sinusoidal);
        analogWrite(pwmPin2, sinusoidal);
        digitalWrite(pwmPin3,0);
      }  
      else if (Current_Micros > (prevMicros + (totalPeriod / 2)) && Current_Micros < (prevMicros + (2 * totalPeriod / 3))) {
        digitalWrite(pwmPin1,0);
        analogWrite(pwmPin2, sinusoidal);
        digitalWrite(pwmPin3,0);
      }  
      else if (Current_Micros > (prevMicros + (2 * totalPeriod / 3)) && Current_Micros < (prevMicros + (5 * totalPeriod / 6))) {
        digitalWrite(pwmPin1,0);
        analogWrite(pwmPin2, sinusoidal);
        analogWrite(pwmPin3, sinusoidal);
      }  
      else if (Current_Micros > (prevMicros + (5 * totalPeriod / 6)) && Current_Micros < (prevMicros + totalPeriod)) {
        digitalWrite(pwmPin1,0);
        digitalWrite(pwmPin2,0);
        analogWrite(pwmPin3, sinusoidal);
      }  
    }
  }
}

void loop() {
  freq = 200;
  totalPeriod = 1000000 / freq;
  uint32_t Current_Micros=micros();
  int sinusoidal = abs(255 * sin(2 * M_PI * Current_Micros / (totalPeriod/6)));

  if (Current_Micros >= (prevMicros + totalPeriod)) {
    prevMicros = micros();
  }
  if (Current_Micros < (prevMicros + (totalPeriod / 6))) {
    analogWrite(pwmPin1, sinusoidal);
    digitalWrite(pwmPin2, 0);
    analogWrite(pwmPin3, sinusoidal);
  }
  else if (Current_Micros > (prevMicros + (totalPeriod / 6)) && Current_Micros < (prevMicros + (totalPeriod / 3))) {
    analogWrite(pwmPin1, sinusoidal);
    digitalWrite(pwmPin2,0);
    digitalWrite(pwmPin3,0);
  }
  else if (Current_Micros > (prevMicros + (totalPeriod / 3)) && Current_Micros < (prevMicros + (totalPeriod / 2))) {
    analogWrite(pwmPin1, sinusoidal);
    analogWrite(pwmPin2, sinusoidal);
    digitalWrite(pwmPin3,0);
  }  
  else if (Current_Micros > (prevMicros + (totalPeriod / 2)) && Current_Micros < (prevMicros + (2 * totalPeriod / 3))) {
    digitalWrite(pwmPin1,0);
    analogWrite(pwmPin2, sinusoidal);
    digitalWrite(pwmPin3,0);
  }  
  else if (Current_Micros > (prevMicros + (2 * totalPeriod / 3)) && Current_Micros < (prevMicros + (5 * totalPeriod / 6))) {
    digitalWrite(pwmPin1,0);
    analogWrite(pwmPin2, sinusoidal);
    analogWrite(pwmPin3, sinusoidal);
  }  
  else if (Current_Micros > (prevMicros + (5 * totalPeriod / 6)) && Current_Micros < (prevMicros + totalPeriod)) {
    digitalWrite(pwmPin1,0);
    digitalWrite(pwmPin2,0);
    analogWrite(pwmPin3, sinusoidal);
  }  
}
