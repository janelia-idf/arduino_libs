#include <SPI.h>
#include "MCP4822.h"

#define AOUT_CS A1
#define AOUT_LDAC A3

MCP4822 analogOut = MCP4822(AOUT_CS,AOUT_LDAC);

void setup() {
  // Setup serial and SPI communications
  Serial.begin(115200);
  analogOut.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  // Configure analog outputs
  analogOut.setGain2X_AB();
}


void loop() {
  static int cnt=0;
  analogOut.setValue_AB(cnt,4095-cnt);
  cnt += 20;
  if (cnt > 4095) {
    cnt = 0;
  }
}
