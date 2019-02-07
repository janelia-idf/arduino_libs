#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Streaming.h"
#include "DictPrinter.h"

DictPrinter::DictPrinter() {
  numberOfItems = 0;
}

void DictPrinter::start() {
  Serial << "{";
  numberOfItems = 0;
}

void DictPrinter::stop() {
  Serial << "}" << endl;
  numberOfItems = 0;
}

void DictPrinter::addEmptyItem(char *key) {
  addKey(key);
  Serial << "\"" << "\"";
}

void DictPrinter::addCharItem(char *key, char value) {
  addKey(key);
  Serial << "\"" << value << "\"";
}

void DictPrinter::addIntItem(char *key, int value) {
  addKey(key);
  Serial <<  _DEC(value);
}

void DictPrinter::addLongItem(char *key, long value) {
  addKey(key);
  Serial << _DEC(value);
}

void DictPrinter::addStrItem(char *key, char *value) {
  addKey(key);
  Serial << "\"" << value << "\"";
}

void DictPrinter::addStrItem(char *key, String &value) {
  addKey(key);
  Serial << "\"" << value << "\"";
}

void DictPrinter::addDblItem(char *key, double value) {
  char valueStr[DP_STR_LEN];
  addKey(key);
  dtostre(value,valueStr,DP_DOUBLE_PREC,0);
  Serial <<  valueStr;
}

void DictPrinter::addFltItem(char *key, float value) {
  char valueStr[DP_STR_LEN];
  addKey(key);
  dtostre((double)value, valueStr, DP_DOUBLE_PREC,0);
  Serial <<  valueStr;
}

void DictPrinter::addLongTuple(char *key, uint8_t num, ...) {
  va_list args;
  long value;
  addKey(key);
  Serial << "(";
  va_start(args,num);
  for (uint8_t i=0; i<num; i++) {
    value = va_arg(args,long);
    Serial << _DEC(value);
    if (i < num-1) {
      Serial << ",";
    }
  }
  Serial << ")";
}

int DictPrinter::len() {
  return numberOfItems;
}

void DictPrinter::addKey(char *key) {
  if (numberOfItems > 0) {
    Serial << ",";
  }
  Serial << "\"" << key << "\"" << ":";
  numberOfItems++;
}
