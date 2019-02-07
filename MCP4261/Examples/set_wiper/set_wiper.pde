#include <SPI.h>
#include "MCP4261.h"

#define LOOP_DELAY 10
#define DIGIPOT_CS A2

MCP4261 digiPot = MCP4261(DIGIPOT_CS);

void setup() {

  // Setup SPI communications
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  // Initialize pot
  digiPot.initialize();
}


void loop() {
  static int cnt=10;
  digiPot.setWiper0(cnt);
  cnt += 1;
  if (cnt > 246) {
    cnt = 10;
  }
  delay(LOOP_DELAY);
}
