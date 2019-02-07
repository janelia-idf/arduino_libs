#include "Arduino.h"
#include <Streaming.h>
#include <SPI.h>
#include "AD57X4R.h"

#define DAC_CS 49


AD57X4R dac = AD57X4R(DAC_CS);

char inputBuffer[128];
uint8_t idx = 0;
boolean inputComplete = false;
char *argv[8];
int arg1, arg2, arg3;
unsigned int dac_value_max;
unsigned int millivolt_max = 10000;

void parse(char *line, char **argv, uint8_t maxArgs) {
  uint8_t argCount = 0;
  while (*line != '\0') {       /* if not the end of line ....... */
    while (*line == ',' || *line == ' ' || *line == '\t' || *line == '\n')
      *line++ = '\0';     /* replace commas and white spaces with 0    */
    *argv++ = line;          /* save the argument position     */
    argCount++;
    if (argCount == maxArgs-1)
      break;
    while (*line != '\0' && *line != ',' && *line != ' ' &&
           *line != '\t' && *line != '\n')
      line++;             /* skip the argument until ...    */
  }
  *argv = '\0';                 /* mark the end of argument list  */
}

void setup() {
  // PC communications
  Serial.begin(115200);
  Serial.println("* System ready *");

  // Setup SPI communications
  SPI.setDataMode(SPI_MODE2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  // Initialize DAC
  dac.init(AD57X4R::AD5724R, AD57X4R::UNIPOLAR_10V, AD57X4R::ALL);
  dac_value_max = dac.getMaxDacValue();
}


void loop() {
  if (inputComplete) {
    parse((char*)inputBuffer, argv, sizeof(argv));
    if (strcmp(argv[0], "analogWrite") == 0) {
      if (0 < strlen(argv[1])) {
        unsigned int millivolt_value = atoi(argv[1]);
        unsigned int dac_value = map(millivolt_value,0,millivolt_max,0,dac_value_max);
        dac.analogWrite(AD57X4R::ALL,dac_value);
      } else {
        Serial << "analogWrite <MILLIVOLT_VALUE>, VALUE = {0.." << millivolt_max << "}" << endl;
      }
    } else if (strcmp(argv[0], "readPowerControlRegister") == 0) {
      int powerControlRegister = dac.readPowerControlRegister();
      Serial << "powerControlRegister = " << _BIN(powerControlRegister) << endl;
    } else {
      Serial.println("analogWrite <MILLIVOLT_VALUE>, readPowerControlRegister");
    }

    inputComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    uint8_t inByte;
    inByte = Serial.read();
    if ((inByte == '\n') || (inByte == '\r')) {
      Serial.println();
      inputBuffer[idx] = 0;
      idx = 0;
      inputComplete = true;
    } else if (((inByte == '\b') || (inByte == 0x7f)) && (idx > 0)) {
      idx--;
      Serial.write(inByte);
      Serial.print(" ");
      Serial.write(inByte);
    }
    else if ((inByte >= ' ') && (idx < sizeof(inputBuffer) - 1)) {
      inputBuffer[idx++] = inByte;
      Serial.write(inByte);
    }
  }
}