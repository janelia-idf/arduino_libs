// ----------------------------------------------------------------------------
// NewhavenDisplay.h
//
// Provides an interface to the Newhaven serial liquid crystal display
// modules.
//
// Author: Peter Polidoro
// ----------------------------------------------------------------------------
#ifndef _NEWHAVEN_DISPLAY_H_
#define _NEWHAVEN_DISPLAY_H_

class NewhavenDisplay {
 public:
  NewhavenDisplay();
  NewhavenDisplay(int rowCount, int colCount);
  void init();
  void print(const String &);
  void print(const char[]);
  void print(char);
  void printPadLeft(const String &, int total_length);
  void printPadLeft(const char[], int total_length);
  void printPadLeft(char, int total_length);
  void printPadRight(const String &, int total_length);
  void printPadRight(const char[], int total_length);
  void printPadRight(char, int total_length);
  void displayOn();
  void displayOff();
  void setCursor(int row,int col);
  void homeCursor();
  void underlineCursorOn();
  void underlineCursorOff();
  void moveCursorLeft();
  void moveCursorRight();
  void blinkingCursorOn();
  void blinkingCursorOff();
  void backspace();
  void clearScreen();
  void setContrast(int contrast);
  void setBrightness(int brightness);
  void moveDisplayLeft();
  void moveDisplayRight();
  void displayFirmwareVersion();
  void displayRs232Rate();

 private:
  int rowCount;
  int colCount;
  void sendCmd(int cmd);
  void stringPadLeft(String &, int length_total);
  void stringPadRight(String &, int length_total);
};


#endif
