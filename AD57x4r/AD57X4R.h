// ----------------------------------------------------------------------------
// AD57X4R.h
//
// Provides an SPI based interface to the AD57X4R
// Complete, Quad, 12-/14-/16-Bit, Serial Input,
// Unipolar/Bipolar Voltage Output DACs.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#ifndef _AD57X4R_H_
#define _AD57X4R_H_

class AD57X4R {
 public:
  enum resolutions {AD5724R, AD5734R, AD5754R};
  enum output_ranges {UNIPOLAR_5V, UNIPOLAR_10V, BIPOLAR_5V, BIPOLAR_10V};
  enum channels {A, B, C, D, ALL};
  AD57X4R();
  AD57X4R(int csPin);
  void init();
  void init(resolutions resolution, output_ranges output_range);
  void init(resolutions resolution, output_ranges output_range, channels channel);
  int readPowerControlRegister();
  void analogWrite(channels channel, unsigned int value);
  void analogWrite(channels channel, int value);
  void analogWrite(int pin, unsigned int value);
  void analogWrite(int pin, int value);
  unsigned int getMaxDacValue();
  void setCSInvert();
  void setCSNormal();
 private:
  int resolution;
  int csPin;
  struct shift_register {
    byte header;
    union {
      unsigned int unipolar;
      int bipolar;
    } data;
  } output;
  struct shift_register input;
  bool unipolar;
  bool csInvertFlag;
  void setupCS(int csPin);
  void setHeader(byte value, byte bit_shift, byte bit_count);
  void setReadWrite(byte value);
  void setRegisterSelect(byte value);
  void setDACAddress(channels channel);
  void setNOP();
  void sendOutput();
  int readInput();
  void setPowerControlRegister(channels channel);
  void setOutputRange(output_ranges output_range, channels channel);
  void setData(unsigned int value);
  void setData(int value);
  void csEnable();
  void csDisable();
};


#endif
