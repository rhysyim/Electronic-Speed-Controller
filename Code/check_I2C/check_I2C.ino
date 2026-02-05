#include <Wire.h>

void scanI2C() {
  byte error, address;
  int devices = 0;

  Serial.println("Scanning I2C bus...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      devices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (devices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Scan done");

  Serial.println();
}

void setup() {
  Serial.begin(9600);
Wire.setSDA(12);
Wire.setSCL(13);
  Wire.begin();
//   Wire.setClock(50000);
  while (!Serial) {
    delay(10);
  }
  
  
}

void loop() {
  scanI2C();
  delay(1000);
}
