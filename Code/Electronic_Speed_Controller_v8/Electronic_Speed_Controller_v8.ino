#include <cmath>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

const int pwmPin1 = 8;
const int pwmPin2 = 9;
const int pwmPin3 = 10;
const int comparatorPin = 13;

unsigned long long int prevMicros = 0;
int freq = 10;
int totalPeriod = 1000000 / freq;
int clockCycles = 2500;
float minDutyCycle = 0.6;
float maxDutyCycle = 0.6;

uint32_t Current_Micros = micros();

void setup() {
  Serial.begin(9600);
  analogWriteFreq(250000);
  pinMode(pwmPin1,OUTPUT);
  pinMode(pwmPin2,OUTPUT);
  pinMode(pwmPin3,OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(11, 1);
  digitalWrite(12, 1);
  digitalWrite(13, 1);

  gpio_set_function(pwmPin1, GPIO_FUNC_PWM);
  gpio_set_function(pwmPin2, GPIO_FUNC_PWM);
  gpio_set_function(pwmPin3, GPIO_FUNC_PWM);
  uint slice_num_1 = pwm_gpio_to_slice_num(pwmPin1);
  uint slice_num_2 = pwm_gpio_to_slice_num(pwmPin3);

  pwm_set_wrap(slice_num_1, clockCycles);
  pwm_set_wrap(slice_num_2, clockCycles);

  // pwm_config_set_phase_correct(slice_num_1, 1);
  // pwm_config_set_phase_correct(slice_num_2, 1);

  
  for (int freq = 200; freq <= 400; freq+=2) {
    Serial.println(freq);
    int totalPeriod = 1000000 / freq;
    // float ratio = 100000*freq/200*(maxDutyCycle-minDutyCycle);
    // int dutyCycle=map(freq,100,300,clockCycles*minDutyCycle,clockCycles*maxDutyCycle);
    int dutyCycle = clockCycles * maxDutyCycle;
    // Serial.println(ratio);
    // float dutyCycle = minDutyCycle + ratio/100000;
    for (int k = 0; k < 100*freq; k++) {
      Current_Micros = micros();
      int sinusoidal= abs(dutyCycle*sin(2 * M_PI * Current_Micros / (totalPeriod/6)));
      if (Current_Micros >= (prevMicros + totalPeriod)) {
        prevMicros = micros();
      }
      if (Current_Micros < (prevMicros + (totalPeriod / 6))) {
        // analogWrite(pwmPin1, sinusoidal);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_A, sinusoidal);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_B, 0);
        pwm_set_chan_level(slice_num_2, PWM_CHAN_A, sinusoidal);
        
      }
      else if (Current_Micros > (prevMicros + (totalPeriod / 6)) && Current_Micros < (prevMicros + (totalPeriod / 3))) {
        pwm_set_chan_level(slice_num_1, PWM_CHAN_A, sinusoidal);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_B, 0);
        pwm_set_chan_level(slice_num_2, PWM_CHAN_A, 0);
      }
      else if (Current_Micros > (prevMicros + (totalPeriod / 3)) && Current_Micros < (prevMicros + (totalPeriod / 2))) {
        pwm_set_chan_level(slice_num_1, PWM_CHAN_A, sinusoidal);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_B, sinusoidal);
        pwm_set_chan_level(slice_num_2, PWM_CHAN_A, 0);
      }  
      else if (Current_Micros > (prevMicros + (totalPeriod / 2)) && Current_Micros < (prevMicros + (2 * totalPeriod / 3))) {
        pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_B, sinusoidal);
        pwm_set_chan_level(slice_num_2, PWM_CHAN_A, 0);
      }  
      else if (Current_Micros > (prevMicros + (2 * totalPeriod / 3)) && Current_Micros < (prevMicros + (5 * totalPeriod / 6))) {
        pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_B, sinusoidal);
        pwm_set_chan_level(slice_num_2, PWM_CHAN_A, sinusoidal);
      }  
      else if (Current_Micros > (prevMicros + (5 * totalPeriod / 6)) && Current_Micros < (prevMicros + totalPeriod)) {
        pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0);
        pwm_set_chan_level(slice_num_1, PWM_CHAN_B, 0);
        pwm_set_chan_level(slice_num_2, PWM_CHAN_A, sinusoidal);
      }  
      pwm_set_enabled(slice_num_1, true);
      pwm_set_enabled(slice_num_2, true);
    }
    
      // Serial.println(dutyCycle);
  }
}

uint slice_num_1 = pwm_gpio_to_slice_num(pwmPin1);
uint slice_num_2 = pwm_gpio_to_slice_num(pwmPin3);

void loop() {
  freq = 400;
  totalPeriod = 1000000 / freq;
  uint32_t Current_Micros=micros();
  float dutyCycle = maxDutyCycle;
  int sinusoidal = abs(clockCycles * dutyCycle * sin(2 * M_PI * Current_Micros / (totalPeriod/6)));

  if (Current_Micros >= (prevMicros + totalPeriod)) {
    prevMicros = micros();
  }
  if (Current_Micros < (prevMicros + (totalPeriod / 6))) {
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, sinusoidal);
    pwm_set_chan_level(slice_num_1, PWM_CHAN_B, 0);
    pwm_set_chan_level(slice_num_2, PWM_CHAN_A, sinusoidal);
  }
  else if (Current_Micros > (prevMicros + (totalPeriod / 6)) && Current_Micros < (prevMicros + (totalPeriod / 3))) {
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, sinusoidal);
    pwm_set_chan_level(slice_num_1, PWM_CHAN_B, 0);
    pwm_set_chan_level(slice_num_2, PWM_CHAN_A, 0);
  }
  else if (Current_Micros > (prevMicros + (totalPeriod / 3)) && Current_Micros < (prevMicros + (totalPeriod / 2))) {
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, sinusoidal);
    pwm_set_chan_level(slice_num_1, PWM_CHAN_B, sinusoidal);
    pwm_set_chan_level(slice_num_2, PWM_CHAN_A, 0);
  }  
  else if (Current_Micros > (prevMicros + (totalPeriod / 2)) && Current_Micros < (prevMicros + (2 * totalPeriod / 3))) {
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num_1, PWM_CHAN_B, sinusoidal);
    pwm_set_chan_level(slice_num_2, PWM_CHAN_A, 0);
  }  
  else if (Current_Micros > (prevMicros + (2 * totalPeriod / 3)) && Current_Micros < (prevMicros + (5 * totalPeriod / 6))) {
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num_1, PWM_CHAN_B, sinusoidal);
    pwm_set_chan_level(slice_num_2, PWM_CHAN_A, sinusoidal);
  }  
  else if (Current_Micros > (prevMicros + (5 * totalPeriod / 6)) && Current_Micros < (prevMicros + totalPeriod)) {
    pwm_set_chan_level(slice_num_1, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num_1, PWM_CHAN_B, 0);
    pwm_set_chan_level(slice_num_2, PWM_CHAN_A, sinusoidal);
  }  
  pwm_set_enabled(slice_num_1, true);
  pwm_set_enabled(slice_num_2, true);
}
