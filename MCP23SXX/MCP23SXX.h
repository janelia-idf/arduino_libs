// ----------------------------------------------------------------------------
// MCP23SXX.h
//
// Provides an SPI based interface to the MCP23SXX
// I/O expanders with serial interface.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------

#ifndef MCP23SXX_H_
#define MCP23SXX_H_

class MCP23SXX {
 public:
  enum resolutions {MCP23S08, MCP23S17};
  MCP23SXX();
  // You must specify the clock select pin
  MCP23SXX(int csPin);
  // You must specify the resolution and the clock select pin
  /* MCP23SXX(resolutions resolution, int csPin); */
  // Optionally, up to 8 devices can share the same clock select pin
  /* MCP23SXX(resolutions resolution, int csPin, byte aaa_hw_addr); */

  // You must specify the resolution
  void init(resolutions resolution);
  // Optionally, up to 8 devices can share the same clock select pin
  /* void init(resolutions resolution, byte aaa_hw_addr); */
  /* int readPowerControlRegister(); */

  // Work on all pins
  void pinMode(int mode);
  int pinMode();
  void port(word value);
  byte port();
  /* word port(); */

  // Work on individual pins 0-15
  void pinMode(int pin, int mode);
  void digitalWrite(int pin, bool value);
  int  digitalRead(int pin);
  void enableInterrupt(int pin);
  void enableInterrupt(int pin, bool defaultValue);
  void disableInterrupt(int pin);
  int  getInterruptFlagRegister();
  int  getInterruptCaptureRegister();

 private:
  // Private Constants
  const static int MCP23S08_PIN_COUNT = 8;
  const static int MCP23S17_PIN_COUNT = 16;

  const static byte IODIRA = 0x00;
  const static byte IODIRB = 0x10;
  const static byte IODIR  = IODIRA;

  const static byte IPOLA = 0x01;
  const static byte IPOLB = 0x11;
  const static byte IPOL  = IPOLA;

  const static byte GPINTENA = 0x02;
  const static byte GPINTENB = 0x12;
  const static byte GPINTEN  = GPINTENA;

  const static byte DEFVALA = 0x03;
  const static byte DEFVALB = 0x13;
  const static byte DEFVAL  = DEFVALA;

  const static byte INTCONA = 0x04;
  const static byte INTCONB = 0x14;
  const static byte INTCON  = INTCONA;

  const static byte IOCONA = 0x0A;   // Config register is here on power up
  const static byte IOCON  = 0x05;   // Config register is here after switch to BANK=1

  const static byte GPPUA  = 0x06;
  const static byte GPPUB  = 0x16;
  const static byte GPPU   = GPPUA;

  const static byte INTFA  = 0x07;
  const static byte INTFB  = 0x17;
  const static byte INTF   = INTFA;

  const static byte INTCAPA  = 0x08;
  const static byte INTCAPB  = 0x18;
  const static byte INTCAP   = INTCAPA;

  const static byte GPIOA  = 0x09;
  const static byte GPIOB  = 0x19;
  const static byte GPIO   = GPIOA;

  const static byte OLATA  = 0x0A;
  const static byte OLATB  = 0x1A;
  const static byte OLAT   = OLATA;

  // Config options for IOCON
  // We set BANK, that re-maps all the addresses
  const static byte BANK   = 1<<7; // 0b10000000 Sets BANK=1
  const static byte MIRROR = 1<<6; // 0b01000000
  const static byte SEQOP  = 1<<5; // 0b00100000 Not needed if BANK=0 (default)
  const static byte DISSLW = 1<<4; // 0b00010000 Not needed for SPI communication?
  const static byte HAEN   = 1<<3; // 0b00001000 Enable the AAA 3-bit chip select
  const static byte ODR    = 1<<2; // 0b00000100 Enable open drain
  const static byte INTPOL = 1<<1; // 0b00000010 Set interrupt polarity HIGH

  int resolution;
  int csPin;
  int pinCount;
  int pinCountMax;
  bool csInvertFlag;
  byte aaa_hw_addr;

  byte read_cmd;
  byte write_cmd;

  word mode;

  void setupCS(int csPin);
  void setupDevice(byte aaa_hw_addr);

  byte getData(byte addr);
  /* word getData(byte addr); */
  void setData(byte addr, byte data);
  /* void setData(byte addr, word data); */

  bool initialized;
};

#endif // Mcp23sxx_h



