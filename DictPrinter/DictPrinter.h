#ifndef DictPrinter_h
#define DictPrinter_h
#include <stdarg.h>

#define DP_DOUBLE_PREC 12
#define DP_STR_LEN 30

class DictPrinter {
 public:
  DictPrinter();
  void start();
  void stop();
  void addEmptyItem(char *key);
  void addCharItem(char *key, char value);
  void addStrItem(char *key, char *value);
  void addStrItem(char *key, String &value);
  void addFltItem(char *key, float value);
  void addDblItem(char *key, double value);
  void addIntItem(char *key, int value);
  void addLongItem(char *key, long value);
  void addLongTuple(char *key, uint8_t num, ...);
  int len();
 private:
  int numberOfItems;
  void addKey(char *key);
};
#endif
