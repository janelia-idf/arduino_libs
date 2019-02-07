// ----------------------------------------------------------------------------
// Tle7232g.h
//
// Provides an SPI based interface to the TLE7232 eight channel
// low-side power switch for enhanced relay control
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>
#include "Tle7232g.h"

//---------- constructor ----------------------------------------------------

TLE7232G::TLE7232G() {
  initialized = false;
}

TLE7232G::TLE7232G(int csPin, int inPin) {
  initialized = false;

  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  this->csPin = csPin;

  pinMode(inPin,OUTPUT);
  digitalWrite(inPin, LOW);
  this->inPin = inPin;
}

//---------- public ----------------------------------------------------


// ----------------------------------------------------------------------------
// TLE7232G::init
//
// ----------------------------------------------------------------------------
void TLE7232G::init() {
  init(DEVICE_COUNT_MAX);
}

void TLE7232G::init(int deviceCount) {
  if ((0 < deviceCount) && (deviceCount <= DEVICE_COUNT_MAX)) {
    this->deviceCount = deviceCount;
  } else {
    this->deviceCount = DEVICE_COUNT_MAX;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    channels = 0;
  }
  setChannels(channels);
  initialized = true;
}

void TLE7232G::setChannels(uint32_t channels) {
  digitalWrite(csPin, LOW);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    this->channels = channels;
    for (int device = (deviceCount - 1); 0 <= device; device--) {
      SPI.transfer(CMD_WRITE + ADDR_CTL);
      SPI.transfer(channels>>(device*8));
    }
  }
  digitalWrite(csPin, HIGH);
  digitalRead(csPin);
}

void TLE7232G::setChannelOn(int channel) {
  if ((0 <= channel) && (channel < 32)) {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      channels = channels | bit;
    }
    setChannels(channels);
  }
}

void TLE7232G::setChannelOff(int channel) {
  if ((0 <= channel) && (channel < 32)) {
    uint32_t bit = 1;
    bit = bit << channel;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      channels = channels & ~bit;
    }
    setChannels(channels);
  }
}

uint32_t TLE7232G::getChannelsOn() {
  return channels;
}

//------------------ private -----------------------------------------------

