#include "Arduino.h"
#include <Streaming.h>
#include "Bronkhorst.h"


// Todo: Make mfc array mfc[]
Bronkhorst mfc1 = Bronkhorst(1);
Bronkhorst mfc2 = Bronkhorst(2);
char inputBuffer[128];
uint8_t idx = 0;
boolean inputComplete = false;
char *argv[8];
int arg1, arg2, arg3;

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

  mfc1.init();
  mfc2.init();
}


void loop() {
  if (inputComplete) {
    parse((char*)inputBuffer, argv, sizeof(argv));
    if (strcmp(argv[0], "setSetpoint") == 0) {
      if ((0 < strlen(argv[1])) && (0 < strlen(argv[2]))) {
        arg1 = atoi(argv[1]);
        arg2 = atoi(argv[2]);
        if ((arg2 < 0) || (32000 < arg2)) {
          Serial.println("Error! Setpoint must be between 0 and 32000.");
        } else {
          if (arg1 == 1) {
            if (mfc1.setSetpoint(arg2)) {
              Serial.println("mfc1 setpoint set successful.");
            } else {
              Serial.println("mfc1 setpoint set unsuccessful.");
            }              
          } else if (arg1 == 2) {
            if (mfc2.setSetpoint(arg2)) {
              Serial.println("mfc2 setpoint set successful.");
            } else {
              Serial.println("mfc2 setpoint set unsuccessful.");
            }              
          } else {
            Serial.println("Error! MFC value must be 1 or 2.");
          }
        }
      } else {
        Serial.println("setSetpoint <MFC> <PERCENT_CAPACITY>, MFC = {1..2}, PERCENT_CAPACITY = {0..32000}");
      }
    } else if (strcmp(argv[0], "getSetpoint") == 0) {
      if (0 < strlen(argv[1])) {
        arg1 = atoi(argv[1]);
        if (arg1 == 1) {
          int setpoint = mfc1.getSetpoint();
          Serial.println(setpoint);
        } else if (arg1 == 2) {
          int setpoint = mfc2.getSetpoint();
          Serial.println(setpoint);
        } else {
          Serial.println("Error! MFC value must be 1 or 2.");
        }
      } else {
        Serial.println("getSetpoint <MFC>, MFC = {1..2}");
      }
    } else if (strcmp(argv[0], "getMeasure") == 0) {
      if (0 < strlen(argv[1])) {
        arg1 = atoi(argv[1]);
        if (arg1 == 1) {
          int measure = mfc1.getMeasure();
          Serial.println(measure);
        } else if (arg1 == 2) {
          int measure = mfc2.getMeasure();
          Serial.println(measure);
        } else {
          Serial.println("Error! MFC value must be 1 or 2.");
        }
      } else {
        Serial.println("getMeasure <MFC>, MFC = {1..2}");
      }
    } else if (strcmp(argv[0], "getSerialNumber") == 0) {
      if (0 < strlen(argv[1])) {
        arg1 = atoi(argv[1]);
        String serialNumberStr;
        if (arg1 == 1) {
          mfc1.getSerialNumber(serialNumberStr);
          Serial.println(serialNumberStr);
        } else if (arg1 == 2) {
          mfc2.getSerialNumber(serialNumberStr);
          Serial.println(serialNumberStr);
        } else {
          Serial.println("Error! MFC value must be 1 or 2.");
        }
      } else {
        Serial.println("getSerialNumber <MFC>, MFC = {1..2}");
      }
    } else if (strcmp(argv[0], "getCapacity") == 0) {
      if (0 < strlen(argv[1])) {
        arg1 = atoi(argv[1]);
        if (arg1 == 1) {
          float capacity = mfc1.getCapacity();
          Serial.println(capacity);
        } else if (arg1 == 2) {
          float capacity = mfc2.getCapacity();
          Serial.println(capacity);
        } else {
          Serial.println("Error! MFC value must be 1 or 2.");
        }
      } else {
        Serial.println("getCapacity <MFC>, MFC = {1..2}");
      }
    } else if (strcmp(argv[0], "getCapacityUnits") == 0) {
      if (0 < strlen(argv[1])) {
        arg1 = atoi(argv[1]);
        String capacityUnitsStr;
        if (arg1 == 1) {
          mfc1.getCapacityUnits(capacityUnitsStr);
          Serial.println(capacityUnitsStr);
        } else if (arg1 == 2) {
          mfc2.getCapacityUnits(capacityUnitsStr);
          Serial.println(capacityUnitsStr);
        } else {
          Serial.println("Error! MFC value must be 1 or 2.");
        }
      } else {
        Serial.println("getCapacityUnits <MFC>, MFC = {1..2}");
      }
    } else if (strcmp(argv[0], "getFluidName") == 0) {
      if (0 < strlen(argv[1])) {
        arg1 = atoi(argv[1]);
        String fluidNameStr;
        if (arg1 == 1) {
          mfc1.getFluidName(fluidNameStr);
          Serial.println(fluidNameStr);
        } else if (arg1 == 2) {
          mfc2.getFluidName(fluidNameStr);
          Serial.println(fluidNameStr);
        } else {
          Serial.println("Error! MFC value must be 1 or 2.");
        }
      } else {
        Serial.println("getFluidName <MFC>, MFC = {1..2}");
      }
    } else if (strcmp(argv[0], "testHexConversions") == 0) {
      Serial.println("testingHexConversions");
      mfc1.testHexConversions();
    } else if (strcmp(argv[0], "testSetSetpoint") == 0) {
      Serial.println("testingSetSetpoint");
      mfc1.testSetSetpoint();
    } else {
      Serial.println("setSetpoint <MFC> <PERCENT_CAPACITY>, getSetpoint <MFC>, getMeasure <MFC>, getSerialNumber <MFC>, getCapacity <MFC>, getCapacityUnits <MFC>, getFluidName <MFC>, testHexConversions, testSetSetpoint");
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