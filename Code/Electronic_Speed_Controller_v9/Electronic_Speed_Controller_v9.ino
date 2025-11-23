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
int clockCycles = 1330;
float minDutyCycle = 0.6;
float maxDutyCycle = 0.6;

uint32_t Current_Micros = micros();

void setup() {
  Serial.begin(9600);
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


  // pwm_config config = pwm_get_default_config();
  // pwm_config_set_phase_correct(&config, true);



  pwm_set_wrap(slice_num_1, clockCycles);
  pwm_set_wrap(slice_num_2, clockCycles);

  // pwm_init(slice_num_1, &config, true);
  // pwm_init(slice_num_2, &config, true);


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
      float theta = 2.0f * M_PI * (Current_Micros % totalPeriod) / totalPeriod;

      int dutyA = (int)((sin(theta) + 1.0) * (dutyCycle / 2));
      int dutyB = (int)((sin(theta - 2.0*M_PI/3.0) + 1.0) * (dutyCycle / 2));
      int dutyC = (int)((sin(theta - 4.0*M_PI/3.0) + 1.0) * (dutyCycle / 2));
      pwm_set_chan_level(slice_num_1, PWM_CHAN_A, dutyA);
      pwm_set_chan_level(slice_num_1, PWM_CHAN_B, dutyB);
      pwm_set_chan_level(slice_num_2, PWM_CHAN_A, dutyC);
        pwm_set_enabled(slice_num_1, true);
  pwm_set_enabled(slice_num_2, true);
  }
}
}
void loop() {
  freq = 400;
  totalPeriod = 1000000 / freq;
  uint32_t Current_Micros=micros();
    uint slice_num_1 = pwm_gpio_to_slice_num(pwmPin1);
  uint slice_num_2 = pwm_gpio_to_slice_num(pwmPin3);
  float dutyCycle = maxDutyCycle;
       Current_Micros = micros();
      float theta = 2.0f * M_PI * (Current_Micros % totalPeriod) / totalPeriod;

      int dutyA = (int)((sin(theta) + 1.0) * (dutyCycle / 2));
      int dutyB = (int)((sin(theta - 2.0*M_PI/3.0) + 1.0) * (dutyCycle / 2));
      int dutyC = (int)((sin(theta - 4.0*M_PI/3.0) + 1.0) * (dutyCycle / 2));
      pwm_set_chan_level(slice_num_1, PWM_CHAN_A, dutyA);
      pwm_set_chan_level(slice_num_1, PWM_CHAN_B, dutyB);
      pwm_set_chan_level(slice_num_2, PWM_CHAN_A, dutyC);
  pwm_set_enabled(slice_num_1, true);
  pwm_set_enabled(slice_num_2, true);
}
