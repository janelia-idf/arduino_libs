// ----------------------------------------------------------------------------
// mcp4261.h
//
// Provides an SPI based interface to the MCP4261 dual ditigal POT with
// non-volatile memory.
//
// Author: Will Dickson
// ----------------------------------------------------------------------------
#ifndef _MCP4261_H_
#define _MCP4261_H_

#define MCP4261_WIPER_0 0
#define MCP4261_WIPER_1 1

class MCP4261 {
 private:
  int cs;
  int send16BitCmd(uint8_t addr, uint8_t cmd, int data);
  void send8BitCmd(uint8_t addr, uint8_t cmd);
  uint8_t getAddrEEPROM(int n);
  uint8_t getWiperAddr(int wiper);
  uint8_t getWiperAddr_NonVolatile(int value);
  bool csInvertFlag;
  void csEnable();
  void csDisable();
 public:
  MCP4261(int csPin);
  void initialize();
  void setWiper(int wiper, int value);
  void setWiper0(int value);
  void setWiper1(int value);
  void incrWiper(int wiper);
  void decrWiper(int wiper);
  void incrWiper0();
  void incrWiper1();
  void decrWiper0();
  void decrWiper1();
  void setWiper_NonVolatile(int wiper, int value);
  void setWiper0_NonVolatile(int value);
  void setWiper1_NonVolatile(int value);
  void writeEEPROM(int n, int value);
  void readEEPROM(int n, int value);
  void setCSInvert();
  void setCSNormal();
};


#endif
