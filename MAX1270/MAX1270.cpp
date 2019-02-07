// ----------------------------------------------------------------------------
// MAX1270.cpp
//
// Provides an SPI based interface to the MAX1270 multirange data acquisition IC
// from Maxim.
//
// Author: Will Dickson
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SPI.h"
#include "MAX1270.h"

// Definitions
#define CLK_INTERNAL  0b00000000
#define CLK_EXTERNAL  0b00000001
#define RANGE_5V      0b00000000
#define RANGE_10V     0b00001000
#define BIPOLAR       0b00000100
#define UNIPOLAR      0b00000000
#define CTL_BYTE_INIT 0b10000000
#define SSTRB_WAIT_US 1

// LSB to voltages for the different gain and polarity combinations
#define LSB2VOLTS_5V_BIPOLAR    0.00244140625
#define LSB2VOLTS_5V_UNIPOLAR   0.001220703125
#define LSB2VOLTS_10V_BIPOLAR   0.0048828125
#define LSB2VOLTS_10V_UNIPOLAR  0.00244140625

// ----------------------------------------------------------------------------
// MAX1270::MAX1270
//
// Constructor
// ----------------------------------------------------------------------------
MAX1270::MAX1270(int csPin, int sstrbPin) {
  mode = 0;
  // Initialize digital IO
  cs = csPin;
  sstrb = sstrbPin;
}

// ---------------------------------------------------------------------------
// Public Methods
// ----------------------------------------------------------------------------

void MAX1270::initialize() {
  pinMode(cs,OUTPUT);
  pinMode(sstrb, INPUT);
  digitalWrite(cs,HIGH);
  // Set default polarity, range and clock
  setBipolar();
  setRange10V();
  setInternalClock();
}

// ----------------------------------------------------------------------------
// MAX1270::sample
//
// Returns an analog sample for the given channel.
// ----------------------------------------------------------------------------
int MAX1270::sample(int chan) {
  int ctlByte;
  int sample;
  int msb;
  int lsb;
  int sgn;

  // Enable chip SPI communications
  digitalWrite(cs,LOW);

  // Transfer control byte
  ctlByte = getCtlByte(chan);
  SPI.transfer(ctlByte);

  // Wait for sstrb to go high
  while(digitalRead(sstrb) == LOW) {};

  // Read sample
  msb = SPI.transfer(0x00);
  lsb = SPI.transfer(0x00);

  // Convert sample from 12bit to 16bit number
  lsb = lsb >> 4;
  sgn = (0b10000000 & msb) >> 7;
  msb = msb << 4;
  if (sgn == 1) {
    msb = 0b1111000000000000 | msb;
  }
  sample = msb | lsb;

  // Disable chip SPI communications
  digitalWrite(cs,HIGH);

  return sample;
}

// ----------------------------------------------------------------------------
// MAX1270::sampleAll
//
// Gets an analog sample from all input channels
// ----------------------------------------------------------------------------
void MAX1270::sampleAll(int values[] ) {
  for (int i=0; i<MAX1270_NUMCHAN; i++) {
    values[i] = sample(i);
  }
}


// ----------------------------------------------------------------------------
// MAX1270::sampleVolts
//
// Gets an analog sample from the given channel and converts to a floating point
// voltage value
//
// Note, may want to move this to fixed point instead floating point.
// ----------------------------------------------------------------------------
float MAX1270::sampleVolts(int chan) {
  int valueInt;
  float valueVolts = 0;
  if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
    valueInt = sample(chan);
    switch (chanRange[chan] | chanPolarity[chan]) {

    case (RANGE_5V | BIPOLAR):
      valueVolts = LSB2VOLTS_5V_BIPOLAR*valueInt;
      break;

    case (RANGE_5V | UNIPOLAR):
      valueVolts = LSB2VOLTS_5V_UNIPOLAR*valueInt;
      break;

    case (RANGE_10V | BIPOLAR):
      valueVolts = LSB2VOLTS_10V_BIPOLAR*valueInt;
      break;

    case (RANGE_10V | UNIPOLAR):
      valueVolts = LSB2VOLTS_10V_UNIPOLAR*valueInt;
      break;

    default:
      break;
    }
  }
  return valueVolts;
}

// ----------------------------------------------------------------------------
// MAX1270::sampleAllVolts
//
// Gets an analog sample from all input channels and converts to a floating
// point voltage value.
//
// Note, may want to move this to fixed point instead of floating point.
// ----------------------------------------------------------------------------
void MAX1270::sampleAllVolts(float values[] ) {
  for (int i=0; i<MAX1270_NUMCHAN; i++) {
    values[i] = sampleVolts(i);
  }
}
// ----------------------------------------------------------------------------
// MAX1270::setInternalClock
//
// Set the MAX1270 to use its internal clock
// ----------------------------------------------------------------------------
void MAX1270::setInternalClock() {
  mode &= 0b10;
  mode |= CLK_INTERNAL;
}

// ----------------------------------------------------------------------------
// MAX1270::setExternalClock
//
// Set the MAX1270 to use an external clock source.
// ----------------------------------------------------------------------------
void MAX1270::setExternalClock() {
  mode &= 0b10;
  mode |= CLK_EXTERNAL;
}

// ----------------------------------------------------------------------------
// MAX1270::setBipolar
//
// Set given the analog input channel to bipolar operation
// ----------------------------------------------------------------------------
void MAX1270::setBipolar(int chan) {
  if ((chan >=0) && (chan < MAX1270_NUMCHAN)) {
    chanPolarity[chan] = BIPOLAR;
  }
}

// ----------------------------------------------------------------------------
// MAX1270::setBipolar
//
// Set all analog input channels to bipolar operation
// ----------------------------------------------------------------------------
void MAX1270::setBipolar() {
  for (int i=0; i<MAX1270_NUMCHAN;i++) {
    setBipolar(i);
  }
}

// ----------------------------------------------------------------------------
// MAX1270::setUnipolar
//
// Set the given analog input channel to unipolar operation
// ----------------------------------------------------------------------------
void MAX1270::setUnipolar(int chan) {
  if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
    chanPolarity[chan] = UNIPOLAR;
  }
}

// ----------------------------------------------------------------------------
// MAX1270::setUnipolar
//
// Set all analog input channels to unipolar operation
// ----------------------------------------------------------------------------
void MAX1270::setUnipolar() {
  for (int i=0; i<MAX1270_NUMCHAN;i++) {
    setUnipolar(i);
  }
}

// ---------------------------------------------------------------------------
// MAX1270::setRange10V
//
// Sets the range of the given analog input channel to 10V. Note, if polarity
// is set to bipolar than range will be (-10V, 10V) and if polarity is set to
// unipolar than range will be (0,10V).
// ---------------------------------------------------------------------------
void MAX1270::setRange10V(int chan) {
  if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
    chanRange[chan] = RANGE_10V;
  }
}

// --------------------------------------------------------------------------
// MAX1270::setRange10V
//
// Sets the range of all analog inputs to 10V.  Note, if polarity  is set to
// bipolar than range will be (-10V, 10V) and if polarity is set to unipolar
// than range will be (0,10V).
// --------------------------------------------------------------------------
void MAX1270::setRange10V() {
  for (int i=0; i<MAX1270_NUMCHAN; i++) {
    setRange10V(i);
  }
}

// ----------------------------------------------------------------------------
// MAX1270::setRange5V
//
// Sets the range of the given analog input channel to 5V. Note, if polarity is
// set to bipolar than range will be (-5V, 5V) and if polarity is set to unipolar
// than range will be (0,5V).
// ----------------------------------------------------------------------------
void MAX1270::setRange5V(int chan) {
  if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
    chanRange[chan] = RANGE_5V;
  }
}

// ----------------------------------------------------------------------------
// MAX1270::setRange5V
//
// Sets the range of all analog inputs to 5V. Note, if polarity  is set to
// bipolar than range will be (-5V, 5V) and if polarity is set to unipolar
// than range will be (0,5V).
// ----------------------------------------------------------------------------
void MAX1270::setRange5V() {
  for (int i=0; i<MAX1270_NUMCHAN; i++) {
    setRange5V(i);
  }
}

// ----------------------------------------------------------------------------
// MAX1270:numchan
//
// Returns the number of analog input channels. Note, this is the same as the
// definition MAX1270_NUMCHAN.
// ---------------------------------------------------------------------------
int MAX1270::numChan() {
  return MAX1270_NUMCHAN;
}

// ----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// MAX1270::getChanMask
//
// Returns control byte channel selection mask. Bits 4,5, and 6 of the control
// byte select the analog input channel.
// ----------------------------------------------------------------------------
int MAX1270::getChanMask(int chan) {
  int mask = 0;
  if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
    mask = chan << 4;
  }
  return mask;
}

// ---------------------------------------------------------------------------
// MAX1270::getCtlByte
//
// Returns the control byte for the given channel based on the current settings
// such as range, polarity, clock selection.
// ----------------------------------------------------------------------------
int MAX1270::getCtlByte(int chan) {
  int ctlByte = CTL_BYTE_INIT;
  ctlByte |= getChanMask(chan);
  ctlByte |= chanRange[chan];
  ctlByte |= chanPolarity[chan];
  ctlByte |= mode;
  return ctlByte;
}

