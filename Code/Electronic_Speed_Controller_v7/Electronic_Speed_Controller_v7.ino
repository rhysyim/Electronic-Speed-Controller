#include <cmath>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

const int pwmPin1 = 16;
const int pwmPin2 = 17;
const int pwmPin3 = 18;
const int comparatorPin = 13;

unsigned long long int prevMicros = 0;
int freq = 10;
int totalPeriod = 1000000 / freq;
int clockCycles = 2500;
float minDutyCycle = 0.13;
float maxDutyCycle = 0.2;

uint32_t Current_Micros = micros();

void setup() {
  Serial.begin(9600);
  pinMode(pwmPin1,OUTPUT);
  pinMode(pwmPin2,OUTPUT);
  pinMode(pwmPin3,OUTPUT);

  gpio_set_function(pwmPin1, GPIO_FUNC_PWM);
  gpio_set_function(pwmPin2, GPIO_FUNC_PWM);
  gpio_set_function(pwmPin3, GPIO_FUNC_PWM);
  uint slice_num_1 = pwm_gpio_to_slice_num(pwmPin1);
  uint slice_num_2 = pwm_gpio_to_slice_num(pwmPin3);

  pwm_set_wrap(slice_num_1, clockCycles);
  pwm_set_wrap(slice_num_2, clockCycles);

  pwm_config_set_phase_correct(slice_num_1, 1);
  pwm_config_set_phase_correct(slice_num_2, 1);

  
  for (int freq = 10; freq <= 200; freq+=3) {
    Serial.println(freq);
    int totalPeriod = 1000000 / freq;
    int dutyCycle=map(freq, 0, 200, clockCycles*minDutyCycle, clockCycles*maxDutyCycle);
    for (int k = 0; k < 100*freq; k++) {
      Current_Micros = micros();
      int sinusoidal= abs(dutyCycle * sin(2 * M_PI * Current_Micros / (totalPeriod/6)));
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
  }
}

uint slice_num_1 = pwm_gpio_to_slice_num(pwmPin1);
uint slice_num_2 = pwm_gpio_to_slice_num(pwmPin3);

void loop() {
  freq = 200;
  totalPeriod = 1000000 / freq;
  uint32_t Current_Micros=micros();
  int sinusoidal= abs(dutyCycle * maxDutyCycle * sin(2 * M_PI * Current_Micros / (totalPeriod/6)));
  
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
