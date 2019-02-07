// ----------------------------------------------------------------------------
// AD5666.cpp
//
// Provides an SPI based interface to the AD5666 Quad 16-Bit DACs.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SPI.h"
#include "AD5666.h"
#include <Streaming.h>


// Command Bits
#define CMD_WRITE_N             0b0000
#define CMD_UPDATE_N            0b0001
#define CMD_WRITE_N_UPDATE_ALL  0b0010
#define CMD_WRITE_N_UPDATE_N    0b0011
#define CMD_POWER_DOWN_UP       0b0100
#define CMD_LOAD_CLEAR_CODE_REG 0b0101
#define CMD_LOAD_LDAC_REG       0b0110
#define CMD_RESET               0b0111
#define CMD_SET_UP_DCEN_REF_REG 0b1000
#define CMD_NO_OP               0b1001

// DAC Address Bits
#define DAC_ADDRESS_A   0b0000
#define DAC_ADDRESS_B   0b0001
#define DAC_ADDRESS_C   0b0010
#define DAC_ADDRESS_D   0b0011
#define DAC_ADDRESS_ALL 0b1111

// ----------------------------------------------------------------------------
// AD5666::AD5666
//
// Constructor
// ----------------------------------------------------------------------------
AD5666::AD5666() {
}

// ----------------------------------------------------------------------------
// AD5666::AD5666
//
// Constructor
// ----------------------------------------------------------------------------
AD5666::AD5666(int csPin) {
  setupCS(csPin);
}

// ----------------------------------------------------------------------------
// AD5666::setupCS
//
// Set chip select pin for SPI Bus, and start high (disabled)
// ----------------------------------------------------------------------------
void AD5666::setupCS(int csPin) {
  ::pinMode(csPin,OUTPUT);
  ::digitalWrite(csPin,HIGH);
  this->csPin = csPin;
}

void AD5666::init() {
  bool standaloneMode = true;
  bool refOff = false;
  init(standaloneMode, refOff);
}

void AD5666::init(bool standaloneMode, bool refOff) {
  int sBit = 0;
  int rBit = 0;
  if (!standaloneMode) {
    sBit = 1;
  }
  if (!refOff) {
    rBit = 1;
  }
  // Setup DAC REF register
  digitalWrite(csPin, LOW);
  SPI.transfer(CMD_SET_UP_DCEN_REF_REG);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer((sBit<<1) + rBit);
  digitalWrite(csPin, HIGH);
  digitalRead(csPin); // add some time

  // Power up all four DACs
  digitalWrite(csPin, LOW);
  SPI.transfer(CMD_POWER_DOWN_UP);
  SPI.transfer(0x00);
  SPI.transfer(0x00); // Normal operation (power-on)
  SPI.transfer(0b1111); // All four DACs
  digitalWrite(csPin, HIGH);
  digitalRead(csPin);
}

void AD5666::analogWrite(channels channel, uint16_t value) {
  // value = 16 bit output value
  int dac;
  switch (channel)
    {
    case A:
      dac = DAC_ADDRESS_A;
      break;
    case B:
      dac = DAC_ADDRESS_B;
      break;
    case C:
      dac = DAC_ADDRESS_C;
      break;
    case D:
      dac = DAC_ADDRESS_D;
      break;
    case ALL:
      dac = DAC_ADDRESS_ALL;
      break;
    }
  digitalWrite(csPin, LOW);
  SPI.transfer(CMD_WRITE_N_UPDATE_N);
  SPI.transfer(((dac & 0x0f)<<4) | ((value & 0xf000)>>12));
  SPI.transfer((value & 0x0ff0)>>4);
  SPI.transfer((value & 0x000f)<<4);
  digitalWrite(csPin, HIGH);
  digitalRead(csPin);
}
