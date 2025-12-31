//ESC Test Code
//Oisin Shaw
//08/12/25
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#define PWM_AH_pin 8
#define PWM_BH_pin 10
#define PWM_CH_pin 12
#define PWM_AL_pin 9
#define PWM_BL_pin 11
#define PWM_CL_pin 13
#define num_PWM_pins 6
uint8_t PWM_Pins[num_PWM_pins] = { PWM_AH_pin, PWM_AL_pin, PWM_BH_pin, PWM_BL_pin, PWM_CH_pin, PWM_CL_pin };
uint8_t PWM_pin_slices[num_PWM_pins];
// uint8_t AB_PWM_slice_num = 0;
// uint8_t C_PWM_slice_num = 0;
uint16_t TOP_value = 65535;
uint16_t deadtime_US = 2;
uint16_t deadtime_counters = 0;
/*Sequence step: Which step of the 3phase cycle is the system in
0= current A->B
1= current A->C
2= current B->C
3= current B->A
4= current C->A
5= current C->B
*/
uint8_t Sequence_step = 0;
uint32_t PWM_Frequency = 10000;  //PWM frequency in Hz
uint32_t Motor_Frequency = 100;  //Frequency at which full cycle of 3 phase sequence occurs, in Hz
uint32_t Sequence_period_micros = 0;
uint32_t Previous_seq_Micros = 0;
uint32_t PWM_calc_period_micros = 0;
uint32_t Previous_PWM_calc_Micros = 0;
uint32_t PWM_steps_per_sequence_period = 0;
uint32_t PWM_calc_counter = 0;
uint32_t Current_Micros = 0;
bool PWM_Phase_correct = 1;
uint32_t includedDeadtime = 0;
/* Waveform type:
0= square wave
1=trapezoid
2=sine
*/
uint8_t Waveform = 0;
float Duty_cycle = 0.1;  //Max value 1
uint16_t PWM_max_value = 0;
uint16_t PWM_vals[num_PWM_pins];
uint16_t PWM_falling_val = 0;
uint16_t PWM_rising_val = 0;
void setup() {
  delay(1000);
  Serial.begin(115200);
  // analogWriteFreq(PWM_Frequency);
  //OR
  //Fpwm = Fsys / period
  //period = (TOP + 1) x (DIV_INT + DIV_FRAC/16)
  pwm_config cfg = pwm_get_default_config();
  pwm_config_set_phase_correct(&cfg, PWM_Phase_correct);
  uint8_t DIV_INT_value = 1;
  uint8_t DIV_FRAC_value = 0;
  TOP_value = (clock_get_hz(clk_sys) / (PWM_Frequency * (PWM_Phase_correct + 1) * (DIV_INT_value + (DIV_FRAC_value / 16.0)))) - 1;
  deadtime_counters = (uint16_t)((TOP_value + 1) * PWM_Frequency * deadtime_US * 1e-6f);
  pwm_config_set_clkdiv_int_frac4(&cfg, DIV_INT_value, DIV_FRAC_value);
  pwm_config_set_wrap(&cfg, TOP_value);
  pwm_config_set_output_polarity(&cfg, 0, 1);  //Invert channel B outputs to use as !PWM signal (lowside). This gives no deadtime! fix later or in hardware
  if (Waveform == 0) {
    Sequence_period_micros = 1000000 / (Motor_Frequency * 6);
  } else if (Waveform == 1) {
    Sequence_period_micros = 1000000 / (Motor_Frequency * 12);
    PWM_calc_period_micros = ((TOP_value + 1) * (PWM_Phase_correct + 1) * ((DIV_INT_value + (DIV_FRAC_value / 16.0))) * 1000000) / clock_get_hz(clk_sys);
    PWM_steps_per_sequence_period = Sequence_period_micros / PWM_calc_period_micros;
  } else if (Waveform == 2) {
  } else {
  }
  PWM_max_value = TOP_value * Duty_cycle;  //move for variable control
  for (int i = 0; i < num_PWM_pins; i++) {
    gpio_set_function(PWM_Pins[i], GPIO_FUNC_PWM);
    PWM_pin_slices[i] = pwm_gpio_to_slice_num(PWM_Pins[i]);
    pwm_init(PWM_pin_slices[i], &cfg, false);
    pwm_set_both_levels(PWM_pin_slices[i], 0, 0);  //Start with PWM off
  }
  pwm_set_mask_enabled(0xFFFFFFFF);  //Set all PWM slices running simultaneously so they stay in step
  Serial.println("Setup complete");
  Serial.print("PWM Frequency = ");
  Serial.println(PWM_Frequency);
  Serial.print("Motor Frequency = ");
  Serial.println(Motor_Frequency);
  Serial.print("Sequence_period_micros = ");
  Serial.println(Sequence_period_micros);
  Serial.print("TOP value = ");
  Serial.println(TOP_value);
  Serial.print("PWM_max_value = ");
  Serial.println(PWM_max_value);
  Serial.print("PWM Phase correct = ");
  Serial.println(PWM_Phase_correct);
  Serial.print("Waveform set to ");
  if (Waveform == 0) {
    Serial.println("Square");
  } else if (Waveform == 1) {
    Serial.println("Trapezoidal");
    Serial.print("PWM_calc_period_micros = ");
    Serial.println(PWM_calc_period_micros);
  } else if (Waveform == 2) {
    Serial.println("Sinusoidal");
  } else {
    Serial.println("Unknown");
  }
}
void loop() {
  Current_Micros = micros();
  if (Current_Micros - Previous_seq_Micros >= Sequence_period_micros) {
    Previous_seq_Micros += Sequence_period_micros;
    Advance_sequence_state();
  }
  if (Waveform == 0) {
  } else if (Waveform == 1) {
    if (Current_Micros - Previous_PWM_calc_Micros >= PWM_calc_period_micros) {
      Previous_PWM_calc_Micros += PWM_calc_period_micros;
      PWM_rising_val = ((PWM_calc_counter * TOP_value) / PWM_steps_per_sequence_period);
      PWM_falling_val = TOP_value - PWM_rising_val;  //calculate PWM falling value
      PWM_calc_counter++;
    }
    if (Sequence_step == 0) {
      PWM_vals[4] = PWM_falling_val*Duty_cycle;
      PWM_vals[5] = PWM_falling_val*Duty_cycle;
    } else if (Sequence_step == 2) {
      PWM_vals[2] = PWM_falling_val;
      PWM_vals[3] = PWM_falling_val;
    } else if (Sequence_step == 4) {
      PWM_vals[0] = PWM_falling_val*Duty_cycle;
      PWM_vals[1] = PWM_falling_val*Duty_cycle;
    }
        if (Sequence_step == 6) {
      PWM_vals[4] = PWM_falling_val;
      PWM_vals[5] = PWM_falling_val;
    } else if (Sequence_step == 8) {
      PWM_vals[2] = PWM_falling_val*Duty_cycle;
      PWM_vals[3] = PWM_falling_val*Duty_cycle;
    } else if (Sequence_step == 10) {
      PWM_vals[0] = PWM_falling_val;
      PWM_vals[1] = PWM_falling_val;
    }
    if (Sequence_step == 1) {
      PWM_vals[4] = PWM_rising_val;
      PWM_vals[5] = PWM_rising_val;
    } else if (Sequence_step == 3) {
      PWM_vals[2] = PWM_rising_val*Duty_cycle;
      PWM_vals[3] = PWM_rising_val*Duty_cycle;
    } else if (Sequence_step == 5) {
      PWM_vals[0] = PWM_rising_val;
      PWM_vals[1] = PWM_rising_val;
    }
    if (Sequence_step == 7) {
      PWM_vals[4] = PWM_rising_val*Duty_cycle;
      PWM_vals[5] = PWM_rising_val*Duty_cycle;
    } else if (Sequence_step == 9) {
      PWM_vals[2] = PWM_rising_val;
      PWM_vals[3] = PWM_rising_val;
    } else if (Sequence_step == 11) {
      PWM_vals[0] = PWM_rising_val*Duty_cycle;
      PWM_vals[1] = PWM_rising_val*Duty_cycle;
    }
  } else if (Waveform == 2) {
  } else {
    //throw error
  }
}
void Advance_sequence_state() {
  Sequence_step++;
  if (Waveform == 0) {
    Sequence_step %= 6;  //reset to 0 once >5
    switch (Sequence_step) {
      case 0:
        pwm_set_both_levels(PWM_pin_slices[0], PWM_max_value, PWM_max_value-deadtime_counters);  //Maybe do something with deadtime here
        pwm_set_both_levels(PWM_pin_slices[2], 0, 0);              //BH =0, BL=1
        pwm_set_both_levels(PWM_pin_slices[4], 0, TOP_value + 1);                          //CH=0, CL=0
        break;
      case 1:
        pwm_set_both_levels(PWM_pin_slices[0], PWM_max_value, PWM_max_value-deadtime_counters);  //Maybe do something with deadtime here
        pwm_set_both_levels(PWM_pin_slices[2], 0, TOP_value + 1);
        pwm_set_both_levels(PWM_pin_slices[4], 0, 0);
        break;
      case 2:
        pwm_set_both_levels(PWM_pin_slices[0], 0, TOP_value + 1);  //Maybe do something with deadtime here
        pwm_set_both_levels(PWM_pin_slices[2], PWM_max_value, PWM_max_value-deadtime_counters);
        pwm_set_both_levels(PWM_pin_slices[4], 0, 0);
        break;
      case 3:
        pwm_set_both_levels(PWM_pin_slices[0], 0, 0);  //Maybe do something with deadtime here
        pwm_set_both_levels(PWM_pin_slices[2], PWM_max_value, PWM_max_value-deadtime_counters);
        pwm_set_both_levels(PWM_pin_slices[4], 0, TOP_value + 1);
        break;
      case 4:
        pwm_set_both_levels(PWM_pin_slices[0], 0, 0);  //Maybe do something with deadtime here
        pwm_set_both_levels(PWM_pin_slices[2], 0, TOP_value + 1);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_max_value, PWM_max_value-deadtime_counters);
        break;
      case 5:
        pwm_set_both_levels(PWM_pin_slices[0], 0, TOP_value + 1);  //Maybe do something with deadtime here
        pwm_set_both_levels(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_max_value, PWM_max_value-deadtime_counters);
        break;
    }
  } else if (Waveform == 1) {
    Sequence_step %= 12;   //reset to 0 once >5
    PWM_calc_counter = 0;  //for calculating PWM vals of slopes
    PWM_rising_val = 0;
    PWM_falling_val = PWM_max_value;
    switch (Sequence_step) {
      case 0:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 1);                      //set AL inverted AH
        pwm_set_both_levels(PWM_pin_slices[0], PWM_max_value, PWM_max_value-deadtime_counters);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[2], 0, TOP_value + 1);          //BH=0, BL=1
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 1);                  //set CL inverted CH
        pwm_set_both_levels(PWM_pin_slices[4], PWM_vals[4], ((PWM_vals[5]-deadtime_counters)>0)?(PWM_vals[5]-deadtime_counters):0);  //Falling PWM value
        break;
      case 1:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[0], PWM_max_value, PWM_max_value-deadtime_counters);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[2], 0, TOP_value + 1);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], 0, ((PWM_vals[5]-deadtime_counters)>0)?(PWM_vals[5]-deadtime_counters):0);  //CH=0, CL=rising PWM value
        break;
      case 2:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[0], PWM_max_value, PWM_max_value-deadtime_counters);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[2], 0, ((PWM_vals[3]-deadtime_counters)>0)?(PWM_vals[3]-deadtime_counters):0);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], 0, TOP_value + 1);
        break;
      case 3:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[0], PWM_max_value, PWM_max_value-deadtime_counters);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[2], PWM_vals[2], ((PWM_vals[3]-deadtime_counters)>0)?(PWM_vals[3]-deadtime_counters):0);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], 0, TOP_value + 1);
        break;
      case 4:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[0], PWM_vals[0], ((PWM_vals[1]-deadtime_counters)>0)?(PWM_vals[1]-deadtime_counters):0);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[2], PWM_max_value, PWM_max_value-deadtime_counters);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], 0, TOP_value + 1);
        break;
      case 5:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[0], 0, ((PWM_vals[1]-deadtime_counters)>0)?(PWM_vals[1]-deadtime_counters):0);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[2], PWM_max_value, PWM_max_value-deadtime_counters);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], 0, TOP_value + 1);
        break;
      case 6:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[0], 0, TOP_value + 1);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[2], PWM_max_value, PWM_max_value-deadtime_counters);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[4], 0, ((PWM_vals[5]-deadtime_counters)>0)?(PWM_vals[5]-deadtime_counters):0);
        break;
      case 7:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[0], 0, TOP_value + 1);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[2], PWM_max_value, PWM_max_value-deadtime_counters);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_vals[4], ((PWM_vals[5]-deadtime_counters)>0)?(PWM_vals[5]-deadtime_counters):0);
        break;
      case 8:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[0], 0, TOP_value + 1);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[2], PWM_vals[2], ((PWM_vals[3]-deadtime_counters)>0)?(PWM_vals[3]-deadtime_counters):0);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_max_value, PWM_max_value-deadtime_counters);
        break;
      case 9:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[0], 0, TOP_value + 1);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[2], 0, ((PWM_vals[3]-deadtime_counters)>0)?(PWM_vals[3]-deadtime_counters):0);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_max_value, PWM_max_value-deadtime_counters);
        break;
      case 10:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[0], 0, ((PWM_vals[1]-deadtime_counters)>0)?(PWM_vals[1]-deadtime_counters):0);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[2], 0, TOP_value + 1);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_max_value, PWM_max_value-deadtime_counters);
        break;
      case 11:
        pwm_set_output_polarity(PWM_pin_slices[0], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[0], PWM_vals[0], ((PWM_vals[1]-deadtime_counters)>0)?(PWM_vals[1]-deadtime_counters):0);  //Maybe do something with deadtime here
        pwm_set_output_polarity(PWM_pin_slices[2], 0, 0);
        pwm_set_both_levels(PWM_pin_slices[2], 0, TOP_value + 1);
        pwm_set_output_polarity(PWM_pin_slices[4], 0, 1);
        pwm_set_both_levels(PWM_pin_slices[4], PWM_max_value, PWM_max_value-deadtime_counters);
        break;
    }
  } else if (Waveform == 2) {
  } else {
    //throw error
  }
}