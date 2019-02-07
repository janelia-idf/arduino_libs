#include <SPI.h>
#include "MAX1270.h"

#define LOOP_DELAY 10
#define AIN_CS 10
#define AIN_SSTRB 8

MAX1270 analogIn = MAX1270(AIN_CS,AIN_SSTRB);

void setup() {
  // Setup serial and SPI communications
  Serial.begin(115200);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  analogIn.initialize();

  // Configure analog inputs
  analogIn.setBipolar();
  analogIn.setRange10V();
}

void loop() {
  int val;
  val = analogIn.sample(0);
  Serial.println(val,DEC);
  delay(LOOP_DELAY);
}
