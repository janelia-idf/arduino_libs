#include "WProgram.h"
#include <SPI.h>
#include <MCP23SXX.h>
#include <Streaming.h>

#define LOOP_DELAY 100
#define IO_CS  3
#define IO_INT_PIN_A 4
#define IO_INT_PIN_B 5
#define INT_PIN 2

MCP23SXX IO = MCP23SXX(IO_CS);
int pinValue;
int flagRegister;
int captureRegister;

void callback() {
  flagRegister = IO.getInterruptFlagRegister();
  // Get capture register to clear interrupts
  captureRegister = IO.getInterruptCaptureRegister();
  if ((1<<IO_INT_PIN_A) & flagRegister) {
    pinValue = IO.digitalRead(IO_INT_PIN_A);
    if (pinValue == LOW) {
      Serial << "Button A pressed." << endl;
    } else {
      Serial << "Button A released." << endl;
    }
  }
  if ((1<<IO_INT_PIN_B) & flagRegister) {
    Serial << "Button B pressed." << endl;
  }
}

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

  // Initialize IO interrupt pin A as input and turn on pullup resistor
  IO.pinMode(IO_INT_PIN_A, INPUT);
  IO.digitalWrite(IO_INT_PIN_A, HIGH);

  // Enable IO interrupt pin A, compare to previous value
  IO.enableInterrupt(IO_INT_PIN_A);

  // Initialize IO interrupt pin B as input and turn on pullup resistor
  IO.pinMode(IO_INT_PIN_B, INPUT);
  IO.digitalWrite(IO_INT_PIN_B, HIGH);

  // Enable IO interrupt pin B, compare to default value HIGH
  IO.enableInterrupt(IO_INT_PIN_B,HIGH);

  // Enable arduino interrupt
  attachInterrupt(0,callback,FALLING);
}

void loop()
{
  delay(LOOP_DELAY);
}


