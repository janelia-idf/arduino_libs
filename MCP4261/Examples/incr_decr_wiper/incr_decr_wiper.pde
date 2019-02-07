#include <SPI.h>
#include "MCP4261.h"

#define LOOP_DELAY 10
#define DIGIPOT_CS A2
#define TOP 200
#define BOTTOM 10
#define UP 0
#define DOWN 1

MCP4261 digiPot = MCP4261(DIGIPOT_CS);

void setup() {

  // Setup serial and SPI communications
  Serial.begin(115200);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  // Initialize pot
  digiPot.initialize();
  digiPot.setWiper0(BOTTOM);
}


void loop() {
  static int cnt=BOTTOM;
  static int state = UP;
  Serial.println(cnt);
  if (state==UP) {
    digiPot.incrWiper0();
    cnt++;
  }
  else {
    digiPot.decrWiper0();
    cnt--;
  }
  if (cnt==TOP) {
    state = DOWN;
  }
  else if (cnt==BOTTOM) {
    state = UP;
  }
  delay(LOOP_DELAY);
}
