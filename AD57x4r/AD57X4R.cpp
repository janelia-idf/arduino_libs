// ----------------------------------------------------------------------------
// AD57X4R.cpp
//
// Provides an SPI based interface to the AD57X4R
// Complete, Quad, 12-/14-/16-Bit, Serial Input,
// Unipolar/Bipolar Voltage Output DACs.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SPI.h"
#include "AD57X4R.h"
#include <Streaming.h>


// Read/Write Bit
#define READ                 1
#define WRITE                0
#define READ_WRITE_BIT_SHIFT 7
#define READ_WRITE_BIT_COUNT 1

// Register Select Bits
#define REGISTER_SELECT_DAC                 0b000
#define REGISTER_SELECT_OUTPUT_RANGE_SELECT 0b001
#define REGISTER_SELECT_POWER_CONTROL       0b010
#define REGISTER_SELECT_CONTROL             0b011
#define REGISTER_SELECT_BIT_SHIFT           3
#define REGISTER_SELECT_BIT_COUNT           3

// DAC Address Bits
#define DAC_ADDRESS_A         0b000
#define DAC_ADDRESS_B         0b001
#define DAC_ADDRESS_C         0b010
#define DAC_ADDRESS_D         0b011
#define DAC_ADDRESS_ALL       0b100
#define DAC_ADDRESS_BIT_SHIFT 0
#define DAC_ADDRESS_BIT_COUNT 3

// Output Ranges
#define OUTPUT_RANGE_UNIPOLAR_5V  0b000
#define OUTPUT_RANGE_UNIPOLAR_10V 0b001
#define OUTPUT_RANGE_BIPOLAR_5V   0b011
#define OUTPUT_RANGE_BIPOLAR_10V  0b100

// ----------------------------------------------------------------------------
// AD57X4R::AD57X4R
//
// Constructor
// ----------------------------------------------------------------------------
AD57X4R::AD57X4R() {
}

// ----------------------------------------------------------------------------
// AD57X4R::AD57X4R
//
// Constructor
// ----------------------------------------------------------------------------
AD57X4R::AD57X4R(int csPin) {
  setupCS(csPin);
  output.header = 0;
}

// ----------------------------------------------------------------------------
// AD57X4R::init
//
// Initialize resolution, output range, and power control
// ----------------------------------------------------------------------------
void AD57X4R::init() {
  // Set default values
  init(AD5724R, UNIPOLAR_5V, ALL);
}

// ----------------------------------------------------------------------------
// AD57X4R::init
//
// Initialize resolution, output range, and power control
// ----------------------------------------------------------------------------
void AD57X4R::init(resolutions resolution, output_ranges output_range) {
  init(resolution, output_range, ALL);
}

// ----------------------------------------------------------------------------
// AD57X4R::init
//
// Initialize resolution, output range, and power control
// ----------------------------------------------------------------------------
void AD57X4R::init(resolutions resolution, output_ranges output_range, channels channel) {
  this->resolution = resolution;
  setOutputRange(output_range, channel);
  setPowerControlRegister(channel);
}

// ----------------------------------------------------------------------------
// AD57X4R::setupCS
//
// Set chip select pin for SPI Bus, and start high (disabled)
// ----------------------------------------------------------------------------
void AD57X4R::setupCS(int csPin) {
  csInvertFlag = false;
  ::pinMode(csPin,OUTPUT);
  ::digitalWrite(csPin,HIGH);
  this->csPin = csPin;
}

// ----------------------------------------------------------------------------
// AD57X4R::setHeader
//
// Set header bits
// ----------------------------------------------------------------------------
void AD57X4R::setHeader(byte value, byte bit_shift, byte bit_count) {
  byte bit_mask = 0;
  for (byte bit=0; bit<bit_count; bit++) {
    bitSet(bit_mask,(bit+bit_shift));
  }
  byte header = output.header;
  header &= ~bit_mask;
  value = value << bit_shift;
  header |= value;
  output.header = header;
}

// ----------------------------------------------------------------------------
// AD57X4R::setReadWrite
//
// Set header read/write bit to value
// ----------------------------------------------------------------------------
void AD57X4R::setReadWrite(byte value)
{
  setHeader(value,READ_WRITE_BIT_SHIFT,READ_WRITE_BIT_COUNT);
}

// ----------------------------------------------------------------------------
// AD57X4R::setRegisterSelect
//
// Set header register select bits
// ----------------------------------------------------------------------------
void AD57X4R::setRegisterSelect(byte value) {
  setHeader(value,REGISTER_SELECT_BIT_SHIFT,REGISTER_SELECT_BIT_COUNT);
}

// ----------------------------------------------------------------------------
// AD57X4R::setDACAddress
//
// Set header DAC address bits
// ----------------------------------------------------------------------------
void AD57X4R::setDACAddress(channels channel) {
  byte value;
  if (channel == A){
    value = DAC_ADDRESS_A;
  } else if (channel == B) {
    value = DAC_ADDRESS_B;
  } else if (channel == C) {
    value = DAC_ADDRESS_C;
  } else if (channel == D) {
    value = DAC_ADDRESS_D;
  } else if (channel == ALL) {
    value = DAC_ADDRESS_ALL;
  }
  setHeader(value,DAC_ADDRESS_BIT_SHIFT,DAC_ADDRESS_BIT_COUNT);
}

// ----------------------------------------------------------------------------
// AD57X4R::setNOP
//
// Set header to NOP condition for reading
// ----------------------------------------------------------------------------
void AD57X4R::setNOP() {
  output.header = 0x18;
}

// ----------------------------------------------------------------------------
// AD57X4R::csEnable
//
// Enable spi communications
// ----------------------------------------------------------------------------
void AD57X4R::csEnable() {
  if (csInvertFlag == false) {
    digitalWrite(csPin,LOW);
  } else {
    digitalWrite(csPin,HIGH);
  }
}

// ----------------------------------------------------------------------------
// AD57X4R::csDisable
//
// Disable spi communications
// ----------------------------------------------------------------------------
void AD57X4R::csDisable() {
  if (csInvertFlag == false) {
    digitalWrite(csPin,HIGH);
  } else {
    digitalWrite(csPin,LOW);
  }
}

// ----------------------------------------------------------------------------
// AD57X4R::sendOutput
//
// Send output shift register to DAC
// ----------------------------------------------------------------------------
void AD57X4R::sendOutput() {
  byte outByteHeader;
  byte outByteDataHigh;
  byte outByteDataLow;
  byte returnByte;

  // Enable SPI communication
  csEnable();

  // Create and send command bytes
  outByteHeader = output.header;
  if (unipolar) {
    unsigned int data;
    data = output.data.unipolar;
    output.data.unipolar = 0;
    outByteDataHigh = highByte(data);
    outByteDataLow = lowByte(data);
  } else {
    int data;
    data = output.data.bipolar;
    output.data.bipolar = 0;
    outByteDataHigh = highByte(data);
    outByteDataLow = lowByte(data);
  }
  returnByte = SPI.transfer(outByteHeader);
  returnByte = SPI.transfer(outByteDataHigh);
  returnByte = SPI.transfer(outByteDataLow);
  // Serial << "outByteHeader = " << _BIN(outByteHeader) << endl;
  // Serial << "outByteDataHigh = " << _BIN(outByteDataHigh) << endl;
  // Serial << "outByteDataLow = " << _BIN(outByteDataLow) << endl;

  // Disable SPI communication
  csDisable();
}

// ----------------------------------------------------------------------------
// AD57X4R::readInput
//
// Sends NOP to the DAC to receive and return data
// ----------------------------------------------------------------------------
int AD57X4R::readInput() {
  byte outByteHeader;
  byte outByteDataHigh;
  byte outByteDataLow;
  byte inByteHeader;
  byte inByteDataHigh;
  byte inByteDataLow;
  int returnData;

  // Send NOP command in header
  setNOP();

  // Enable SPI communication
  csEnable();

  // Create and send command bytes
  outByteHeader = output.header;
  outByteDataHigh = 0;
  outByteDataLow = 0;
  inByteHeader = SPI.transfer(outByteHeader);
  inByteDataHigh = SPI.transfer(outByteDataHigh);
  inByteDataLow = SPI.transfer(outByteDataLow);
  // Serial << "inByteHeader = " << _BIN(inByteHeader) << endl;
  // Serial << "inByteDataHigh = " << _BIN(inByteDataHigh) << endl;
  // Serial << "inByteDataLow = " << _BIN(inByteDataLow) << endl;

  // Disable SPI communication
  csDisable();

  // Fill return data
  returnData = ((int)inByteDataHigh << 8) | (int)inByteDataLow;
  return returnData;
}

// ----------------------------------------------------------------------------
// AD57X4R::setData
//
// Sets output shift register data to value
// ----------------------------------------------------------------------------
void AD57X4R::setData(unsigned int value) {
  if (resolution == AD5754R) {
    output.data.unipolar = value;
  } else if (resolution == AD5734R) {
    output.data.unipolar = value << 2;
  } else if (resolution == AD5724R) {
    output.data.unipolar = value << 4;
  }
}

// ----------------------------------------------------------------------------
// AD57X4R::setData
//
// Sets output shift register data to value
// ----------------------------------------------------------------------------
void AD57X4R::setData(int value) {
}

// ---------------------------------------------------------------------------
// AD57X4R::setOutputRange
//
// Sets the output range
// ---------------------------------------------------------------------------
void AD57X4R::setOutputRange(output_ranges output_range, channels channel) {
  setReadWrite(WRITE);
  setRegisterSelect(REGISTER_SELECT_OUTPUT_RANGE_SELECT);
  setDACAddress(channel);
  if (output_range == UNIPOLAR_5V) {
    unipolar = true;
    output.data.unipolar = 0b000;
  } else if (output_range == UNIPOLAR_10V) {
    unipolar = true;
    output.data.unipolar = 0b001;
  } else if (output_range == BIPOLAR_5V) {
    unipolar = false;
    output.data.bipolar = 0b011;
  } else if (output_range == BIPOLAR_10V) {
    unipolar = false;
    output.data.bipolar = 0b101;
  }
  sendOutput();
}

// ---------------------------------------------------------------------------
// AD57X4R::setPowerControlRegister
//
// Set data in the power control register
// ---------------------------------------------------------------------------
void AD57X4R::setPowerControlRegister(channels channel) {
  setReadWrite(WRITE);
  setRegisterSelect(REGISTER_SELECT_POWER_CONTROL);
  setDACAddress(A);
  bool unipolarPrevious = unipolar;
  unipolar = true;
  // place internal reference in normal operating mode
  output.data.unipolar = 0b10000;
  // power up DACs
  if (channel == A) {
    output.data.unipolar |= 0b0001;
  } else if (channel == B) {
    output.data.unipolar |= 0b0010;
  } else if (channel == C) {
    output.data.unipolar |= 0b0100;
  } else if (channel == D) {
    output.data.unipolar |= 0b1000;
  } else if (channel == ALL) {
    output.data.unipolar |= 0b1111;
  }
  sendOutput();
  unipolar = unipolarPrevious;
}

// ---------------------------------------------------------------------------
// AD57X4R::readPowerControlRegister
//
// Reads and returns the data in the power control register
// ---------------------------------------------------------------------------
int AD57X4R::readPowerControlRegister() {
  int returnData;
  setReadWrite(READ);
  setRegisterSelect(REGISTER_SELECT_POWER_CONTROL);
  setDACAddress(A);
  sendOutput();
  returnData = readInput();
  return returnData;
}

// ---------------------------------------------------------------------------
// AD57X4R::analogWrite
//
// Sets the DAC channel to value
// ---------------------------------------------------------------------------
void AD57X4R::analogWrite(channels channel, unsigned int value) {
  if (unipolar) {
    setReadWrite(WRITE);
    setRegisterSelect(REGISTER_SELECT_DAC);
    setDACAddress(channel);
    setData(value);
    sendOutput();
  }
}

// ---------------------------------------------------------------------------
// AD57X4R::analogWrite
//
// Sets the DAC channel to value
// ---------------------------------------------------------------------------
void AD57X4R::analogWrite(channels channel, int value) {
}

// ---------------------------------------------------------------------------
// AD57X4R::analogWrite
//
// Sets the DAC channel to value
// ---------------------------------------------------------------------------
void AD57X4R::analogWrite(int pin, unsigned int value) {
  channels channel;
  // Unnecessary and way too much code, but very explicit
  switch (pin) {
  case 0 : channel = A; break;
  case 1 : channel = B; break;
  case 2 : channel = C; break;
  case 3 : channel = D; break;
  }
  analogWrite(channel, value);
}

// ---------------------------------------------------------------------------
// AD57X4R::analogWrite
//
// Sets the DAC channel to value
// ---------------------------------------------------------------------------
void AD57X4R::analogWrite(int pin, int value) {
  channels channel;
  // Unnecessary and way too much code, but very explicit
  switch (pin) {
  case 0 : channel = A; break;
  case 1 : channel = B; break;
  case 2 : channel = C; break;
  case 3 : channel = D; break;
  }
  analogWrite(channel, value);
}

// ---------------------------------------------------------------------------
// AD57X4R::getMaxDacValue
//
// Returns the maximum dac value based on resolution
// ---------------------------------------------------------------------------
unsigned int AD57X4R::getMaxDacValue() {
  switch (resolution) {
  case AD5724R: return 4095; break;
  case AD5734R: return 16383; break;
  case AD5754R: return 65535; break;
  }
}

// ----------------------------------------------------------------------------
// AD57X4R::setCSInvert
//
// use inverted chip select - high to enable
// ----------------------------------------------------------------------------
void AD57X4R::setCSInvert() {
  csInvertFlag = true;
}

// ----------------------------------------------------------------------------
// AD57X4R::setCSNormal
//
// use normal chip select - low to enable
// ----------------------------------------------------------------------------
void AD57X4R::setCSNormal() {
  csInvertFlag = false;
}
