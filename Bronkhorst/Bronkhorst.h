// ----------------------------------------------------------------------------
// Bronkhorst.h
//
// Provides a digital (RS232) interface to the
// Bronkhorst IQ+FLOW digital mass flow meter/controllers.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#ifndef _BRONKHORST_H_
#define _BRONKHORST_H_

class Bronkhorst {
 public:
  Bronkhorst();
  Bronkhorst(int mfcIndex);
  void init();
  bool setSetpoint(int setpoint);
  int getSetpoint();
  int getMeasure();
  bool getSerialNumber(String &serialNumberStr);
  float getCapacity();
  bool getCapacityUnits(String &capacityUnitsStr);
  bool getFluidName(String &fluidNameStr);
  bool testHexConversions();
  bool testSetSetpoint();

 private:
  int mfcIndex;
  long getRspTriesMax;
  void sendCmd(String &cmdStr);
  int available();
  char read();
  void getRsp(String &rspStr);
  void getRspNoWait(String &rspStr);
  void flushInput();
  int hexCharToInt(char c);
  int hexStrToInt(String &hexStr);
  void hexStrToAsciiStr(String &hexStr, String &asciiStr);
  float hexStrToFloat(String &hexStr);
};


#endif
