arduino_libs
---------------------

A collection of libraries for use with the Arduino

Summary:

* AD5666: Provides an SPI based interface to the AD5666 Quad 16-Bit
  DACs.

* AD57X4R: Provides an SPI based interface to the AD57X4R complete,
  Quad, 12-/14-/16-Bit, Serial Input, Unipolar/Bipolar Voltage Output
  DACs.

* Bronkhorst: Provides a digital (RS232) interface to the Bronkhorst
  IQ+FLOW digital mass flow meter/controllers. Use a MAX232 chip to
  connect from Arduino serial TTL to RS232.

* ByteBuffer: A circular buffer implementation for Arduino created by
  Sigurdur Orn, July 19, 2010.

* DictPrinter: An arduino library for printing python dictionaries to
  the serial port.

* FastADXL345: Modified version of the triple Axis Accelerometer
  Arduino library by Love Electronics (loveelectronics.co.uk) for use
  with the FastWire library.

* FastWire: Modified version of the Arduino Wire library for 400kHz
  I2C communications.

* InputCapture: Measure period and onDuration of square wave and
  execute arbitrary function once per cycle.

* LinearSensorArray: Interface to an array of linear sensors. For
  example, TSL1401BCS-LF by AMS-TAOS.

* LookupTable: A simple integer valued lookup table library for
  Arduino.

* MAX1270: provides an SPI based interface to the MAX1270 multirange
  data acquisition IC from Maxim.
  
* MCP23XX: A library to control the Microchip MCP23xx 8/16-bit IO
  Expanders.  For the Arduino Platform. Modified version of the
  library by Dreamcat4 https://github.com/dreamcat4/Mcp23s17.

* MCP4261: provides an SPI based interface to the MCP4261 dual ditigal
  POT with non-volatile memory.

* MCP4822: provides an SPI based interface to the MCP4822 dual voltage
  output digital to analog converter.

* NewhavenDisplay: interface to Newhaven serial liquid crystal display
  modules.

* SerialLCD: a library for communicating with the Sparkfun Serial
  Graphic LCD 128x64. Requires upgraded firmware for the serial
  backpack SerialGLCD
  (http://sourceforge.net/projects/serialglcd/files/) by by Jen Holt.

* SerialReceiver: a simple library for handling serial communication
  on the Arduino.

* ShiftPWM: ShiftPWM is a software PWM library for Arduino that uses
  shift registers to expand the number of PWM outputs. With only 3
  data pins, you can control an almost unlimited amount of PWM
  outputs. Because ShiftPWM is a software PWM library, it is not
  limited by the number of hardware PWM outputs on the Arduino. Shift
  registers are also a lot cheaper than dedicated hardware PWM IC’s,
  like the TLC5940. http://www.elcojacobs.com/shiftpwm/

* SoftPWM: An Arduino and Wiring Library to produce PWM signals on any
  arbitrary pin. It was originally designed for use controlling the
  brightness of LEDs, but could be modified to control servos and
  other low frequency PWM controlled devices as well. It uses a single
  hardware timer (Timer 2) on the microcontroller to generate up to 20
  PWM channels.
  http://code.google.com/p/rogue-code/wiki/SoftPWMLibraryDocumentation

* StandardCplusplus: This is a straight port of uClibc++ for Arduino.
  https://github.com/maniacbug/StandardCplusplus.git

* Streaming: Streaming C++-style Output with Operator <<
  http://arduiniana.org/libraries/streaming/

* TimerOne: This library is a collection of routines for configuring
  the 16 bit hardware timer called Timer1 on the ATmega168/328. There
  are 3 hardware timers available on the chip, and they can be
  configured in a variety of ways to achieve different
  functionality. The development of this library began with the need
  for a way to quickly and easily set the PWM period or frequency, but
  has grown to include timer overflow interrupt handling and other
  features.
  http://code.google.com/p/arduino-timerone/

* Tlc5940: An Arduino Library for the Texas Instruments TLC5940
  16-channel LED Driver.
  http://code.google.com/p/tlc5940arduino/

* Tle7232g: Provides an SPI based interface to the TLE7232 eight
  channel low-side power switch for enhanced relay control. Also works
  with TLE7230 and perhaps other models as well.

* Tone: Produces square-wave of the specified frequency (and 50% duty
  cycle) on any Arduino pin.
  http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation


For installation instructions see the INSTALL.txt file.


