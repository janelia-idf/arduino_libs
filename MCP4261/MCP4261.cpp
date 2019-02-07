// ----------------------------------------------------------------------------
// MCP4261.cpp
//
// Provides an SPI based interface to the MCP4261 dual ditigal POT with
// non-volatile memory.
//
// Author: Will Dickson
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SPI.h"
#include "MCP4261.h"

// Memory locations
#define ADDR_VOLATILE_WIPER_0     0b0000
#define ADDR_VOLATILE_WIPER_1     0b0001
#define ADDR_NONVOLATILE_WIPER_0  0b0010
#define ADDR_NONVOLATILE_WIPER_1  0b0011
#define ADDR_TCON_REGISTER        0b0100
#define ADDR_STATUS_REGISTER      0b0101
#define ADDR_EEPROM_START         0b0110
#define ADDR_EEPROM_END           0b1111

// Commands bits
#define CMD_READ_DATA  0b11
#define CMD_WRITE_DATA 0b00
#define CMD_INCREMENT  0b01
#define CMD_DECREMENT  0b10

// ----------------------------------------------------------------------------
// MCP4261::MCP4261
//
// Constructor
// ----------------------------------------------------------------------------
MCP4261::MCP4261(int csPin) {
  cs = csPin;
  csInvertFlag = false;
}


// ----------------------------------------------------------------------------
// MCP4261::csEnable
//
// Enable spi communications
// ----------------------------------------------------------------------------
void MCP4261::csEnable() {
  if (csInvertFlag == false) {
    digitalWrite(cs,LOW);
  }
  else {
    digitalWrite(cs,HIGH);
  }
}

// ----------------------------------------------------------------------------
// MCP4261::csDisable
//
// Disable spi communications
// ----------------------------------------------------------------------------
void MCP4261::csDisable() {
  if (csInvertFlag == false) {
    digitalWrite(cs,HIGH);
  }
  else {
    digitalWrite(cs,LOW);
  }
}

// ----------------------------------------------------------------------------
// MCP4261::send16BitCmd
//
// Sends a 16 bit command with the given addr and data
// ----------------------------------------------------------------------------
int MCP4261::send16BitCmd(uint8_t addr, uint8_t cmd, int data) {
  uint8_t outByte0;
  uint8_t outByte1;
  uint8_t inByte0;
  uint8_t inByte1;
  int data9Bits;
  int retData;

  // Enable SPI communication
  csEnable();

  // Restrict data to 9 bits
  data9Bits = (data & 0b0000000111111111);

  // Create and send command bytes
  outByte0 =  (addr & 0b00001111) << 4;
  outByte0 |= (cmd  & 0b00000011) << 2;
  outByte0 |= (uint8_t) (data9Bits >> 8);
  outByte1 = (uint8_t)( data9Bits & 0b11111111);
  inByte0 = SPI.transfer(outByte0);
  inByte1 = SPI.transfer(outByte1);

  // Disable SPI communication
  csDisable();

  // Packup return data
  inByte0 = (inByte0 & 0b00000001);
  retData =  (((int) inByte0)<< 8) | (inByte1);
  return retData;
}

// ----------------------------------------------------------------------------
// MCP4261::send8BitCmd
//
// Sends a 8 bit command with the given address and data
// ----------------------------------------------------------------------------
void MCP4261::send8BitCmd(uint8_t addr, uint8_t cmd) {
  uint8_t byte;

  // Enable SPI communication
  csEnable();

  // Create and send command byte
  byte = (addr & 0b00001111) << 4;
  byte |= (cmd & 0b00000011) << 2;
  SPI.transfer(byte);

  // Disable SPI communication
  csDisable();
}

// ----------------------------------------------------------------------------
// MCP4261::getWiperAddr
//
// Get the address (volatile) for the given wiper
// ----------------------------------------------------------------------------
uint8_t MCP4261::getWiperAddr(int wiper) {
  if (wiper == MCP4261_WIPER_0) {
    return ADDR_VOLATILE_WIPER_0;
  }
  else {
    return ADDR_VOLATILE_WIPER_1;
  }
}

// ----------------------------------------------------------------------------
// MCP4261::getWiperAddr_NonVolatile
//
// Get the nonvolatile address of the given wiper
// ----------------------------------------------------------------------------
uint8_t MCP4261::getWiperAddr_NonVolatile(int wiper) {
  if (wiper == MCP4261_WIPER_0) {
    return ADDR_NONVOLATILE_WIPER_0;
  }
  else {
    return ADDR_NONVOLATILE_WIPER_1;
  }
}

// ----------------------------------------------------------------------------
// MCP4261::setWiper
//
// Set the given wiper to the given value.
// ----------------------------------------------------------------------------
void MCP4261::setWiper(int wiper, int value) {
  uint8_t wiperAddr;
  if ((value >= 0) && (value <= 256)) {
    wiperAddr = getWiperAddr(wiper);
    send16BitCmd(wiperAddr, CMD_WRITE_DATA, value);
  }
}

// ----------------------------------------------------------------------------
// MCP4261::setWiper0
//
// Sets wiper 0 (volatile) to the given value
// ----------------------------------------------------------------------------
void MCP4261::setWiper0(int value) {
  setWiper(MCP4261_WIPER_0, value);
}

// ----------------------------------------------------------------------------
// MCP4261::setWiper1
//
// Sets wiper 1 (volatile) to the given value
// ----------------------------------------------------------------------------
void MCP4261::setWiper1(int value) {
  setWiper(MCP4261_WIPER_1, value);
}

// ---------------------------------------------------------------------------
// MCP4261::setWiper_NonVolatile
//
// Sets the non-volatile value for the wiper.
//
// Note, after writing to a nonvolatile memory location you need to wait until
// the end of the eeprom write cycle before making another nonvolatile write -
// so put the appropriate delays into your code.  (see the MCP4261 spec sheet)
// ---------------------------------------------------------------------------
void MCP4261::setWiper_NonVolatile(int wiper, int value) {
  uint8_t wiperAddr;
  if ((value >=0) && (value <= 256)) {
    wiperAddr = getWiperAddr_NonVolatile(wiper);
    send16BitCmd(wiperAddr, CMD_WRITE_DATA, value);
  }
}

// ----------------------------------------------------------------------------
// MCP4261::setWiper0_NonVolatile
//
// Sets the non-volatile value for wiper 0
//
// Note, after writing to a nonvolatile memory location you need to wait until
// the end of the eeprom write cycle before making another nonvolatile write -
// so put the appropriate delays into your code.  (see the MCP4261 spec sheet)
// ---------------------------------------------------------------------------
void MCP4261::setWiper0_NonVolatile(int value) {
  setWiper_NonVolatile(MCP4261_WIPER_0, value);
}

// ---------------------------------------------------------------------------
// MCP4261::setWiper1_NonVolatile
//
// Sets the non-volatile value for wiper 1
//
// Note, after writing to a nonvolatile memory location you need to wait until
// the end of the eeprom write cycle before making another nonvolatile write -
// so put the appropriate delays into your code.  (see the MCP4261 spec sheet)
// ---------------------------------------------------------------------------
void MCP4261::setWiper1_NonVolatile(int value) {
  setWiper_NonVolatile(MCP4261_WIPER_1, value);
}


// ---------------------------------------------------------------------------
// MCP4261::incrWiper
//
// Increments the (volatile) value of the given wiper
// ---------------------------------------------------------------------------
void MCP4261::incrWiper(int wiper) {
  uint8_t wiperAddr;
  wiperAddr = getWiperAddr(wiper);
  send8BitCmd(wiperAddr,CMD_INCREMENT);
}

// ---------------------------------------------------------------------------
// MCP4261::decrWiper
//
// Decrements the (volatile) value of the given wiper
// ---------------------------------------------------------------------------
void MCP4261::decrWiper(int wiper) {
  uint8_t wiperAddr;
  wiperAddr = getWiperAddr(wiper);
  send8BitCmd(wiperAddr,CMD_DECREMENT);
}


// ----------------------------------------------------------------------------
// MCP4261::incrWiper0
//
// Increments wiper 0
// ----------------------------------------------------------------------------
void MCP4261::incrWiper0() {
  incrWiper(MCP4261_WIPER_0);
}

// ----------------------------------------------------------------------------
// MCP4261::decrWiper0
//
// Decrements wiper 0
// ----------------------------------------------------------------------------
void MCP4261::decrWiper0() {
  decrWiper(MCP4261_WIPER_0);
}

// ----------------------------------------------------------------------------
// MCP4261::incrWiper1
//
// Increments wiper 1
// ----------------------------------------------------------------------------
void MCP4261::incrWiper1() {
  incrWiper(MCP4261_WIPER_1);
}

// ----------------------------------------------------------------------------
// MCP4261::decrWiper1
//
// Decrements wiper 1
// ----------------------------------------------------------------------------
void MCP4261::decrWiper1() {
  decrWiper(MCP4261_WIPER_1);
}

// ---------------------------------------------------------------------------
// MCP4261::initialize
//
// Initializes the TCON register which connects terminals A, B and wipers for
// both potentiometers.
// ---------------------------------------------------------------------------
void MCP4261::initialize() {
  uint8_t byte0;
  uint8_t byte1;

  pinMode(cs,OUTPUT);
  digitalWrite(cs,HIGH);

  // Enable SPI communication
  digitalWrite(cs,LOW);
  // Send command
  send16BitCmd(ADDR_TCON_REGISTER, CMD_WRITE_DATA, 0b0000000111111111);
  // Disable SPI communication
  digitalWrite(cs,HIGH);
}

// ----------------------------------------------------------------------------
// MCP4261::getAddrEEPROM
//
// Returns the address of EEPROM n
// ----------------------------------------------------------------------------
uint8_t MCP4261::getAddrEEPROM(int n) {
  return ((uint8_t) n) + ADDR_EEPROM_START;
}

// ----------------------------------------------------------------------------
// MCP4261::getAddrEEPROM
//
// use inverted chip select - high to enable
// ----------------------------------------------------------------------------
void MCP4261::setCSInvert() {
  csInvertFlag = true;
}

// ----------------------------------------------------------------------------
// MCP4261::getAddrEEPROM
//
// use normal chip select - low to enable
// ----------------------------------------------------------------------------
void MCP4261::setCSNormal() {
  csInvertFlag = false;
}
