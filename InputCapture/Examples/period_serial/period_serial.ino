#include "Arduino.h"
#include "InputCapture.h"
#include <Streaming.h>

// Writes period and onDuration values to serial port for display.

// Attach function generator to ICP5 (pin 48 on Arudino Mega 2560) and
// use 0-5V square wave from ~31-100Hz (~32-10ms period). Multiply
// period and onDuration values by 500 to get units of nanoseconds.


void writePeriodSerial(uint16_t period, uint16_t onDuration) {
  long period_ns = period*500L;
  long onDuration_ns = onDuration*500L;
  Serial << "period_ns = " << period_ns << ", onDuration_ns = " << onDuration_ns << endl;
}

void setup() {
  inputCapture.init();

  // Setup serial communications
  Serial.begin(115200);

  inputCapture.addCycleTask(writePeriodSerial);
}


void loop() {
}
