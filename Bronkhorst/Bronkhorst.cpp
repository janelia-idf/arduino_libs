#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Bronkhorst.h"
#include <Streaming.h>


// ----------------------------------------------------------------------------
// Bronkhorst::Bronkhorst
//
// Constructor
// ----------------------------------------------------------------------------
Bronkhorst::Bronkhorst() {
}

// ----------------------------------------------------------------------------
// Bronkhorst::Bronkhorst
//
// Constructor
// ----------------------------------------------------------------------------
Bronkhorst::Bronkhorst(int mfcIndex) {
  this->mfcIndex = mfcIndex;
}

void Bronkhorst::init() {
  getRspTriesMax = 6500;
  if (mfcIndex == 1) {
    Serial1.begin(38400);
  } else if (mfcIndex == 2) {
    Serial2.begin(38400);
  }
}

bool Bronkhorst::setSetpoint(int setpoint) {
  if ((0 <= setpoint) && (setpoint <= 32000)) {
    String cmdStr = ":0680010121";
    String setpointHexStr = String(setpoint,HEX);
    int setpointHexStrLen = setpointHexStr.length();
    for (int i=(4-setpointHexStrLen); 0 < i; i--) {
      setpointHexStr = String("0") + setpointHexStr;
    }
    setpointHexStr.toUpperCase();
    cmdStr += setpointHexStr;
    sendCmd(cmdStr);

    // getRsp causes setSetpoint to fail in interrupts
    // String rspStr;
    // getRsp(rspStr);
    // return rspStr == ":0480000005";
    return true;
  } else {
    return false;
  }
}

int Bronkhorst::getSetpoint() {
  String cmdStr = ":06800401210121";
  String rspStr;
  sendCmd(cmdStr);
  getRsp(rspStr);
  String subStr = rspStr.substring(11);
  return hexStrToInt(subStr);
}

bool Bronkhorst::testHexConversions() {
  String setpointHexStr;
  int setpointConverted;
  for (int setpoint=32000; 0 <= setpoint; setpoint--) {
    setpointHexStr = String(setpoint,HEX);
    setpointConverted = hexStrToInt(setpointHexStr);
    Serial << "setpoint: " << setpoint << ", setpointHexStr: "
           << setpointHexStr << ", setpointConverted: "
           << setpointConverted << endl;
    if (setpointConverted != setpoint) {
      Serial << "Error" << endl;
      return false;
    }
  }
  return true;
}

bool Bronkhorst::testSetSetpoint() {
  String setpointHexStr;
  int setpointConverted;
  int setpoint = 32000;
  int setpointGot;
  int setpointHexStrLen;
  while (0 <= setpoint) {
    setSetpoint(setpoint);
    setpointHexStr = String(setpoint,HEX);
    setpointHexStrLen = setpointHexStr.length();
    for (int i=(4-setpointHexStrLen); 0 < i; i--) {
      setpointHexStr = String("0") + setpointHexStr;
    }
    setpointHexStrLen = setpointHexStr.length();
    setpointConverted = hexStrToInt(setpointHexStr);
    setpointGot = getSetpoint();
    Serial << "setpoint: " << setpoint
           << ", setpointHexStr: " << setpointHexStr
           << ", setpointHexStrLen: " << setpointHexStrLen
           << ", setpointConverted: " << setpointConverted
           << ", setpointGot: " << setpointGot
           << endl;
    if (setpointGot != setpoint) {
      Serial << "Error" << endl;
      return false;
    }
    setpoint -= 100;
  }
  return true;
}

int Bronkhorst::getMeasure() {
  String cmdStr = ":06800401210120";
  String rspStr;
  sendCmd(cmdStr);
  getRsp(rspStr);
  String subStr = rspStr.substring(11);
  return hexStrToInt(subStr);
}

bool Bronkhorst::getSerialNumber(String &serialNumberStr) {
  String cmdStr = ":078004716C716300";
  String rspStr;
  sendCmd(cmdStr);
  getRsp(rspStr);
  String subStr = rspStr.substring(13,rspStr.length() - 2);
  String asciiStr;
  hexStrToAsciiStr(subStr,asciiStr);
  asciiStr.trim();
  serialNumberStr = asciiStr;
  return true;
}

float Bronkhorst::getCapacity() {
  String cmdStr = ":068004014F014D";
  String rspStr;
  sendCmd(cmdStr);
  getRsp(rspStr);
  String subStr = rspStr.substring(11);
  return hexStrToFloat(subStr);
}

bool Bronkhorst::getCapacityUnits(String &capacityUnitsStr) {
  String cmdStr = ":0780040170017F07";
  String rspStr;
  sendCmd(cmdStr);
  getRsp(rspStr);
  String subStr = rspStr.substring(13);
  String asciiStr;
  hexStrToAsciiStr(subStr,asciiStr);
  asciiStr.trim();
  capacityUnitsStr = asciiStr;
  return true;
}

bool Bronkhorst::getFluidName(String &fluidNameStr) {
  String cmdStr = ":078004017101710A";
  String rspStr;
  sendCmd(cmdStr);
  getRsp(rspStr);
  String subStr = rspStr.substring(13);
  String asciiStr;
  hexStrToAsciiStr(subStr,asciiStr);
  asciiStr.trim();
  fluidNameStr = asciiStr;
  return true;
}

void Bronkhorst::sendCmd(String &cmdStr) {
  flushInput();
  if (mfcIndex == 1) {
    Serial1.println(cmdStr);
  } else if (mfcIndex ==2) {
    Serial2.println(cmdStr);
  }
}

int Bronkhorst::available() {
  if (mfcIndex == 1) {
    return Serial1.available();
  } else if (mfcIndex ==2) {
    return Serial2.available();
  }
}

char Bronkhorst::read() {
  if (mfcIndex == 1) {
    return (char)Serial1.read();
  } else if (mfcIndex ==2) {
    return (char)Serial2.read();
  }
}

void Bronkhorst::getRsp(String &rspStr) {
  bool rspReceived = false;
  long tries = 0;
  String inputStr = "";
  while (!rspReceived && (tries < getRspTriesMax)) {
    if (available()) {
      char inputChar = read();
      if ((inputChar == '\r') or (inputChar == '\n')){
        rspReceived = true;
      } else {
        inputStr += inputChar;
      }
    } else {
      tries += 1;
    }
  }
  inputStr.trim();
  rspStr = inputStr;
}

void Bronkhorst::flushInput() {
  while (available()) {
    char inputChar = read();
  }
}

int Bronkhorst::hexCharToInt(char c){
  if(c >=97) {
    c=c-32;
  }
  int first = c / 16 - 3;
  int second = c % 16;
  int result = first*10 + second;
  if(result > 9) result--;
  return result;
}

int Bronkhorst::hexStrToInt(String &hexStr) {
  int len = hexStr.length();
  int index = len - 1;
  int multiplier = 1;
  int convertedInt = 0;
  int tempInt;
  while (0 <= index) {
    tempInt = hexCharToInt(hexStr.charAt(index));
    convertedInt += tempInt * multiplier;
    index--;
    multiplier *= 16;
  }
  return convertedInt;
}

void Bronkhorst::hexStrToAsciiStr(String &hexStr, String &asciiStr) {
  String convertedStr = "";
  int len = hexStr.length();
  int index = 0;
  int convertedInt;
  while ((index + 1) < len) {
    String subStr = hexStr.substring(index,index+2);
    convertedInt = hexStrToInt(subStr);
    convertedStr += String((char)convertedInt);
    index += 2;
  }
  asciiStr = convertedStr;
}

float Bronkhorst::hexStrToFloat(String &hexStr) {
  bool isBigEndian = true;
  float convertedFloat = 0.0;
  union {
    byte asBytes[4];
    float asFloat;
  } converter;
  int len = hexStr.length();
  if (isBigEndian) {
    int indexStr = 0;
    int indexBytes = len/2 - 1;
    while ((indexStr + 1) < len) {
      String subStr = hexStr.substring(indexStr,indexStr+2);
      converter.asBytes[indexBytes] = hexStrToInt(subStr);
      indexStr += 2;
      indexBytes--;
    }
  } else {
    int indexStr = 0;
    int indexBytes = 0;
    while ((indexStr + 1) < len) {
      String subStr = hexStr.substring(indexStr,indexStr+2);
      converter.asBytes[indexBytes] = hexStrToInt(subStr);
      indexStr += 2;
      indexBytes++;
    }
  }
  return converter.asFloat;
}
