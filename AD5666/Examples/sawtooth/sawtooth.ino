#include "Arduino.h"
#include <SPI.h>
#include <Streaming.h>
#include "AD5666.h"

#define LOOP_DELAY 10
#define DAC_CS 53

const uint16_t value_min = 0;
const uint16_t value_max = 65535;
const uint16_t value_inc = 6554;

AD5666 dac = AD5666(DAC_CS);
uint16_t value = value_min;

void setup() {
  // Setup serial communications
  Serial.begin(115200);

  // Setup SPI communications
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.begin();

  // Initialize DAC
  bool standaloneMode = true;
  bool referenceOff = false;
  dac.init(standaloneMode,referenceOff);

}


void loop() {
  Serial << "value = " << _DEC(value) << endl;
  if (value <= value_max) {
    value += value_inc;
  } else {
    value = value_min;
  }
  dac.analogWrite(AD5666::ALL,value);
  delay(LOOP_DELAY);
}
