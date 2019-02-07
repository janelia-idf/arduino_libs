// ----------------------------------------------------------------------------
// MCP23SXX.h
//
// Provides an SPI based interface to the MCP23SXX
// I/O expanders with serial interface.
//
// For the cmd, AAA is the 3-bit MCP23S17 device hardware address.
// Useful for letting up to 8 chips sharing same SPI Chip select
// #define MCP23S17_READ  B0100AAA1
// #define MCP23S17_WRITE B0100AAA0
//
// The default SPI Control Register - SPCR = B01010000;
// interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
// sample on leading edge of clk,system clock/4 rate (fastest).
// Enable the digital pins 11-13 for SPI (the MOSI,MISO,SPICLK)#
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SPI.h>
#include "MCP23SXX.h"
#include <Streaming.h>

//---------- constructor ----------------------------------------------------

MCP23SXX::MCP23SXX() {
  initialized = false;
}

MCP23SXX::MCP23SXX(int csPin) {
  initialized = false;
  setupCS(csPin);
  pinCountMax = MCP23S17_PIN_COUNT;
}

// MCP23SXX::MCP23SXX(resolutions resolution, int csPin) {
//   this->resolution = resolution;
//   setupCS(csPin);
//   setupDevice(0b0000);
// }

// MCP23SXX::MCP23SXX(resolutions resolution, int csPin, byte aaa_hw_addr) {
//   // Set the aaa hardware address for this chip by tying the
//   // MCP23SXX's pins (A0, A1, and A2) to either 5v or GND.
//   this->resolution = resolution;
//   setupCS(csPin);
//   setupDevice(aaa_hw_addr);
// }

//------------------ private -----------------------------------------------

// ----------------------------------------------------------------------------
// MCP23SXX::setupCS
//
// Set chip select pin for SPI Bus, and start high (disabled)
// ----------------------------------------------------------------------------
void MCP23SXX::setupCS(int csPin) {
  csInvertFlag = false;
  ::pinMode(csPin,OUTPUT);
  ::digitalWrite(csPin,HIGH);
  this->csPin = csPin;
}

// ----------------------------------------------------------------------------
// MCP23SXX::init
//
// Initialize resolution
// ----------------------------------------------------------------------------
void MCP23SXX::init(resolutions resolution) {
  initialized = true;
  this->resolution = resolution;
  setupDevice(0b0000);
}

void MCP23SXX::setupDevice(byte aaa_hw_addr) {
  if (resolution == MCP23S08) {
    pinCount = MCP23S08_PIN_COUNT;
  } else if (resolution == MCP23S17) {
    pinCount = MCP23S17_PIN_COUNT;
  }
  for (int pin=0; pin<pinCount; pin++) {
    if (INPUT) {
      this->mode = this->mode | INPUT<<pin;
    } else {
      this->mode = this->mode & INPUT<<pin;
    }
  }

  this->aaa_hw_addr = aaa_hw_addr;
  this->read_cmd  = 0b01000000 | aaa_hw_addr<<1 | 1<<0; // MCP23SXX_READ  = 0b0100AAA1
  this->write_cmd = 0b01000000 | aaa_hw_addr<<1 | 0<<0; // MCP23SXX_WRITE = 0b0100AAA0

  // If MCP23S17, set BANK=1 so addresses will be the same as MCP23S08
  if (resolution == MCP23S17) {
    setData(IOCONA, BANK);
  }

  byte ioconValue;
  ioconValue = getData(IOCON);
  // Serial << "ioconValue = " << _BIN(ioconValue) << endl;

  // Enable MIRROR, SEQOP, HAEN
  // setData(IOCON, (getData(IOCON) | MIRROR | SEQOP | HAEN));

  // Enable SEQOP
  setData(IOCON, SEQOP);

  ioconValue = getData(IOCON);
  // Serial << "ioconValue = " << _BIN(ioconValue) << endl;
}

byte MCP23SXX::getData(byte addr) {
  byte data;
  byte dummyData=0;
  ::digitalWrite(csPin, LOW);
  SPI.transfer(read_cmd);
  SPI.transfer(addr);
  data = SPI.transfer(dummyData);
  ::digitalWrite(csPin, HIGH);
  return data;
}

// word MCP23SXX::getData(byte addr) {
//   byte low_byte;
//   byte high_byte;
//   ::digitalWrite(csPin, LOW);
//   SPI.transfer(read_cmd);
//   SPI.transfer(addr);
//   low_byte  = SPI.transfer(dummyData);
//   high_byte = SPI.transfer(dummyData);
//   ::digitalWrite(csPin, HIGH);
//   return word(high_byte,low_byte);
// }

void MCP23SXX::setData(byte addr, byte data) {
  ::digitalWrite(csPin, LOW);
  SPI.transfer(write_cmd);
  SPI.transfer(addr);
  SPI.transfer(data);
  ::digitalWrite(csPin, HIGH);
}

// void MCP23SXX::setData(byte addr, word data) {
//   ::digitalWrite(csPin, LOW);
//   SPI.transfer(write_cmd);
//   SPI.transfer(addr);
//   SPI.transfer(lowByte(data));
//   SPI.transfer(highByte(data));
//   SPI.transfer(data);
//   ::digitalWrite(csPin, HIGH);
// }

//---------- public ----------------------------------------------------

void MCP23SXX::pinMode(int mode) {
  byte input_pins;
  if(mode == OUTPUT) {
    input_pins = 0x00;
  } else {
    input_pins = 0xFF;
  }

  if (resolution == MCP23S17) {
    setData(IODIRA, input_pins);
    setData(IODIRB, input_pins);
  } else if (resolution == MCP23S08) {
    setData(IODIR, input_pins);
  }
  for (int pin=0; pin<pinCount; pin++) {
    if (mode) {
      this->mode = this->mode | mode<<pin;
    } else {
      this->mode = this->mode & mode<<pin;
    }
  }
}

int MCP23SXX::pinMode() {
  return mode;
}

void MCP23SXX::port(word value) {
  if (mode == INPUT) {
    if (value == HIGH) {
      if (resolution == MCP23S08) {
        setData(GPPU, 0xFF);
      } else {
        setData(GPPUA, 0xFF);
        setData(GPPUB, 0xFF);
      }
    } else if (value == LOW) {
      if (resolution == MCP23S08) {
        setData(GPPU, 0x00);
      } else {
        setData(GPPUA, 0x00);
        setData(GPPUB, 0x00);
      }
    }
  } else if (mode == OUTPUT) {
    if (resolution == MCP23S08) {
      setData(GPIO,(byte)value);
    } else if (resolution == MCP23S17) {
      setData(GPIO,(word)value);
    }
  }
}

byte MCP23SXX::port() {
  return getData(GPIO);
}

// word MCP23SXX::port() {
//   return getData(GPIO);
// }

void MCP23SXX::pinMode(int pin, int mode) {
  byte IODIR_;
  if (resolution == MCP23S08) {
    IODIR_ = IODIR;
  } else if (pin < 8) {
    IODIR_ = IODIRA;
  } else {
    IODIR_ = IODIRB;
  }

  if (mode == INPUT) {
    setData(IODIR_, getData(IODIR_) | 1<<pin );
  } else {
    setData(IODIR_, getData(IODIR_) & ~(1<<pin) );
  }
  if (mode) {
    this->mode = this->mode | mode<<pin;
  } else {
    this->mode = this->mode & mode<<pin;
  }
}

void MCP23SXX::digitalWrite(int pin, bool value) {
  if (mode == OUTPUT) {
    byte GPIO_;
    if (resolution == MCP23S08) {
      GPIO_ = GPIO;
    } else if (pin < 8) {
      GPIO_ = GPIOA;
    } else {
      GPIO_ = GPIOB;
    }

    if (value) {
      setData(GPIO_, getData(GPIO_) | 1<<pin );
    } else {
      setData(GPIO_, getData(GPIO_) & ~(1<<pin) );
    }
  } else if (mode == INPUT) {
    byte GPPU_;
    if (resolution == MCP23S08) {
      GPPU_ = GPPU;
    } else if (pin < 8) {
      GPPU_ = GPPUA;
    } else {
      GPPU_ = GPPUB;
    }

    if (value) {
      setData(GPPU_, getData(GPPU_) | 1<<pin );
    } else {
      setData(GPPU_, getData(GPPU_) & ~(1<<pin) );
    }
  }
}

int MCP23SXX::digitalRead(int pin) {
  // Serial << "External IO digitalRead from pin " << pin << endl;
  // Serial << "initialized  = " << initialized << endl;
  byte GPIO_;
  byte offset=0;
  int pinValue;
  if (resolution == MCP23S08) {
    GPIO_ = GPIO;
  } else if (pin < 8) {
    GPIO_ = GPIOA;
  } else {
    GPIO_ = GPIOB;
    offset = 8;
  }
  pinValue = (int)(getData(GPIO_) & 1<<(pin-offset));

  // TODO: make depend on polarity setting
  if (pinValue) {
    return HIGH;
  } else {
    return LOW;
  }
}

// ----------------------------------------------------------------------------
// MCP23SXX::enableInterrupt
//
// Enable GPIO input pin for interrupt-on-change event.
// Compare to previous value.
// ----------------------------------------------------------------------------
void MCP23SXX::enableInterrupt(int pin) {
  byte INTCON_;
  byte GPINTEN_;
  byte offset=0;
  if (resolution == MCP23S08) {
    INTCON_ = INTCON;
    GPINTEN_ = GPINTEN;
  } else if (pin < 8) {
    INTCON_ = INTCONA;
    GPINTEN_ = GPINTENA;
  } else {
    INTCON_ = INTCONB;
    GPINTEN_ = GPINTENB;
    offset = 8;
  }
  setData(INTCON_, getData(INTCON_) & ~(1<<(pin-offset)) );
  setData(GPINTEN_, getData(GPINTEN_) | (1<<(pin-offset)) );

  // Perform dummy read to make sure interrupts are cleared
  byte portData = port();
}

// ----------------------------------------------------------------------------
// MCP23SXX::enableInterrupt
//
// Enable GPIO input pin for interrupt-on-change event.
// Compare to default value.
// ----------------------------------------------------------------------------
void MCP23SXX::enableInterrupt(int pin, bool defaultValue) {
  byte DEFVAL_;
  byte INTCON_;
  byte GPINTEN_;
  byte offset=0;
  if (resolution == MCP23S08) {
    DEFVAL_ = DEFVAL;
    INTCON_ = INTCON;
    GPINTEN_ = GPINTEN;
  } else if (pin < 8) {
    DEFVAL_ = DEFVALA;
    INTCON_ = INTCONA;
    GPINTEN_ = GPINTENA;
  } else {
    DEFVAL_ = DEFVALB;
    INTCON_ = INTCONB;
    GPINTEN_ = GPINTENB;
    offset = 8;
  }
  if (defaultValue) {
    setData(DEFVAL_, getData(DEFVAL_) | (1<<(pin-offset)) );
  } else {
    setData(DEFVAL_, getData(DEFVAL_) & ~(1<<(pin-offset)) );
  }
  setData(INTCON_, getData(INTCON_) | (1<<(pin-offset)) );
  setData(GPINTEN_, getData(GPINTEN_) | (1<<(pin-offset)) );
}

// ----------------------------------------------------------------------------
// MCP23SXX::disableInterrupt
//
// Disable GPIO input pin for interrupt-on-change event.
// ----------------------------------------------------------------------------
void MCP23SXX::disableInterrupt(int pin) {
  byte GPINTEN_;
  byte offset=0;
  if (resolution == MCP23S08) {
    GPINTEN_ = GPINTEN;
  } else if (pin < 8) {
    GPINTEN_ = GPINTENA;
  } else {
    GPINTEN_ = GPINTENB;
    offset = 8;
  }
  setData(GPINTEN_, getData(GPINTEN_) & ~(1<<(pin-offset)) );
}

// ----------------------------------------------------------------------------
// MCP23SXX::getInterruptFlagRegister
//
// Returns the contents of the interrupt flag register.
// ----------------------------------------------------------------------------
int MCP23SXX::getInterruptFlagRegister() {
  int flagRegisterValues;
  if (resolution == MCP23S08) {
    flagRegisterValues = (int)(getData(INTF));
  } else {
    flagRegisterValues = (int)(getData(INTFA)) | (int)(getData(INTFB))<<8;
  }
  return flagRegisterValues;
}

// ----------------------------------------------------------------------------
// MCP23SXX::getInterruptCaptureRegister
//
// Returns the contents of the interrupt capture register.
// ----------------------------------------------------------------------------
int MCP23SXX::getInterruptCaptureRegister() {
  int captureRegisterValues;
  if (resolution == MCP23S08) {
    captureRegisterValues = (int)(getData(INTCAP));
  } else {
    captureRegisterValues = (int)(getData(INTCAPA)) | (int)(getData(INTCAPB))<<8;
  }
  return captureRegisterValues;
}
