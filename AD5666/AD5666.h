// ----------------------------------------------------------------------------
// AD5666.h
//
// Provides an SPI based interface to the AD5666 Quad 16-Bit DACs.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#ifndef _AD5666_H_
#define _AD5666_H_

class AD5666 {
 public:
  enum channels {A, B, C, D, ALL};
  AD5666();
  AD5666(int csPin);
  void init();
  void init(bool standaloneMode, bool refOff);
  void analogWrite(channels channel, uint16_t value);

 private:
  int csPin;
  void setupCS(int csPin);
};


#endif
