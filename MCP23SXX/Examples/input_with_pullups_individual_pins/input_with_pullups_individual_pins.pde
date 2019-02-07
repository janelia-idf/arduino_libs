#include "WProgram.h"
#include <SPI.h>
#include <MCP23SXX.h>
#include <Streaming.h>

#define LOOP_DELAY 100
#define IO_CS  3

MCP23SXX IO = MCP23SXX(IO_CS);
int pinValue;
int pinCount = 6;

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

  // initialize individual pins as input and turn on pullup resistors
  for (int pin = 0; pin < pinCount; pin++) {
    IO.pinMode(pin, INPUT);
    IO.digitalWrite(pin, HIGH);
  }
}

void loop()
{
  // Read each IO line individually
  for (int pin = 0; pin < pinCount; pin++) {
    pinValue = IO.digitalRead(pin);
    if (pinValue == HIGH) {
      Serial << "pin " << _DEC(pin) << " = " << "HIGH" << endl;
    } else {
      Serial << "pin " << _DEC(pin) << " = " << "LOW" << endl;
    }
  }
  delay(LOOP_DELAY);
}


