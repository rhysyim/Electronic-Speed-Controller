#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <math.h>

Adafruit_ADS1015 ads;  // Create ADS1015 object
Adafruit_ADS1015 ads1;  // Create ADS1015 object

// Thermistor constants
const float R_SERIES = 10000.0;
const float R0 = 10000.0;
const float B = 3435.0;
const float T0 = 25.0 + 273.15;

// Pi Pico supplies 3.3V to your voltage divider
const float VREF_ACTUAL = 3.3;
const float VREF_PGA = 4.096;  // For GAIN_ONE setting
const float ADC_MAX = 2047.0;  // ADS1015 11-bit for single-ended

float adcToTemp(int16_t adcValue) {
  // Convert ADC reading to voltage
  float Vout = adcValue * (VREF_PGA / ADC_MAX);
  
  if (Vout >= VREF_ACTUAL || Vout <= 0) {
    return NAN;
  }
  
  // Calculate thermistor resistance from voltage divider
  float Rtherm = R_SERIES * (Vout / (VREF_ACTUAL - Vout));
  
  // Apply Beta parameter equation
  float T = 1.0 / ((1.0 / T0) + (1.0 / B) * log(Rtherm / R0));
  
  return T - 273.15;
}

void setup() {
  Serial.begin(9600);
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();
  // Wire.setClock(50000);
  // Wait for serial port to connect
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("ADS1015 Thermistor Temperature Monitor");
  Serial.println("--------------------------------------");
  
  // Initialize ADS1015 at default address 0x48
  if (!ads.begin(0x48)) {
    Serial.println("Failed to initialize ADS1015 0x48. Check wiring!");
    while (1);
  }
  
  // if (!ads1.begin(0x49)) {
  //   Serial.println("Failed to initialize ADS1015 0x49. Check wiring!");
  //   while (1);
  // }
  // Set gain to ±4.096V range
  // ads.setGain(GAIN_ONE);
  // ads1.setGain(GAIN_ONE);
  
  Serial.println("ADS1015 initialized successfully");
  Serial.println();
}

void loop() {
  
  int16_t BH_raw = ads1.readADC_SingleEnded(1);
  int16_t BL_raw = ads1.readADC_SingleEnded(0);
  int16_t AH_raw = ads.readADC_SingleEnded(1);
  int16_t AL_raw = ads.readADC_SingleEnded(0);
  int16_t CH_raw = ads1.readADC_SingleEnded(2);
  int16_t CL_raw = ads1.readADC_SingleEnded(3);
  
  
  // Convert to temperature
  float AH = adcToTemp(AH_raw);
  float AL = adcToTemp(AL_raw);
  float BL = adcToTemp(BL_raw);
  float BH = adcToTemp(BH_raw);
  float CL = adcToTemp(CL_raw);
  float CH = adcToTemp(CH_raw);
  
Serial.print(AH); Serial.print(" ");
Serial.print(AL); Serial.print(" ");
Serial.print(BL); Serial.print(" ");
Serial.print(BH); Serial.print(" ");
Serial.print(CL); Serial.print(" ");
Serial.println(CH);

  
  delay(500);
}
