#include "Arduino.h"
#include <SPI.h>
#include <Streaming.h>
#include "InputCapture.h"
#include "AD57X4R.h"

// Writes frequency value to AD57X4R DAC analog output.
// <80Hz = 0V
// >=80Hz && <=300Hz : 100Hz = 1V, 250Hz = 2.5V...
// >300Hz : ignore

// Attach function generator to ICP5 (pin 48 on Arudino Mega 2560) and
// use 0-5V square wave from ~31-400Hz (~32-2.5ms period).

#define DAC_CS 49

const uint16_t dac_value_min = 655; // 0.8V : (0.8/5)*4096
const uint16_t dac_value_max = 2458; // 3V : (3/5)*4096
const float freq_min = 80.0;
const float freq_max = 300.0;

AD57X4R dac = AD57X4R(DAC_CS);

void writeFreqDac(uint16_t period, uint16_t onDuration) {
  uint16_t dac_value;
  float freq = 2000000.0/(float)period;
  if (freq <= freq_max) {
    if (freq >= freq_min) {
      dac_value = map(freq*1000, freq_min*1000, freq_max*1000, dac_value_min, dac_value_max);
    } else {
      dac_value = 0;
      Serial << "glitch!" << endl;
    }
    dac.analogWrite(AD57X4R::A,dac_value);
  }
}

void setup() {
  //
  Serial.begin(9600);
  delay(1000);
  Serial << "reset!!" << endl;
  //
  inputCapture.init();

  // Setup SPI communications
  SPI.setDataMode(SPI_MODE2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  // Initialize DAC
  dac.init(AD57X4R::AD5724R, AD57X4R::UNIPOLAR_5V, AD57X4R::A);
  dac.analogWrite(AD57X4R::A,0);

  inputCapture.addCycleTask(writeFreqDac);
}


void loop() {
}
