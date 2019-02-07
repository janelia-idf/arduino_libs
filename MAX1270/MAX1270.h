// ----------------------------------------------------------------------------
// MAX1270.h
//
// Provides an SPI based interface to the MAX1270 multirange data acquisition IC
// from Maxim.
//
// Author: Will Dickson
// ----------------------------------------------------------------------------
#ifndef _MAX1270_H_
#define _MAX1270_H_

#define MAX1270_NUMCHAN 8

class MAX1270 {
 private:
  int cs;                            // Chip select DIO
  int sstrb;                         // Sample strobe DIO
  int chanRange[MAX1270_NUMCHAN];    // Channel Ranges
  int chanPolarity[MAX1270_NUMCHAN]; // Channel Polarity
  int mode;                          // Clock selection
  int getChanMask(int chan);
  int getCtlByte(int chan);
 public:
  MAX1270(int csPin, int sstrbPin);
  void setBipolar();
  void setBipolar(int chan);
  void setUnipolar();
  void setUnipolar(int chan);
  void setRange5V();
  void setRange5V(int chan);
  void setRange10V();
  void setRange10V(int chan);
  void setInternalClock();
  void setExternalClock();
  void initialize();
  int  sample(int chan);
  void sampleAll(int value[]);
  float sampleVolts(int chan);
  void sampleAllVolts(float value[]);
  int numChan();
};

#endif
