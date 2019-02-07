#include "WProgram.h"
#include <SPI.h>
#include <MCP23SXX.h>
#include <Streaming.h>

#define LOOP_DELAY 100
#define IO_CS  3

MCP23SXX IO = MCP23SXX(IO_CS);
byte portValue;


void setup()
{
  // Setup serial communications
  Serial.begin(115200);

  // Setup SPI communications
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  // Initialize IO
  IO.init(MCP23SXX::MCP23S08);

  // Set all pins to be inputs
  IO.pinMode(INPUT);

  // Set pullup resistors on all pins
  IO.port(HIGH);
}

void loop()
{
  // Read entire port at once
  portValue = IO.port();
  Serial << "portValue = " << _BIN(portValue) << endl;
  delay(LOOP_DELAY);
}


