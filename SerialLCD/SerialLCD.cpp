#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#include "SerialLCD.h"


SerialLCD::SerialLCD() {
}

SerialLCD::SerialLCD(Stream &port) {
  setPort(port);
}

void SerialLCD::setPort(Stream &port) {
  _portPtr = &port;
}

void SerialLCD::clearScreen(void) {
  // Clear the LCD display sets x and y offsets to 0,0
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x00));
}

void SerialLCD::print(char *data) {
  // Print data to the LCD
  _portPtr -> print(data);
}

void SerialLCD::print(char *data, byte x, byte y) {
  // Print data at position given by x and y.
  setPos(x,y);
  print(data);
}

void SerialLCD::setBrightness(byte value) {
  // Sets the LCD backlight brightness - adjusts duty cycle.
  // value should be between 0 and 100.
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x02));
  _portPtr -> write(value);
}

void SerialLCD::setX(byte x) {
  // Set the x coordinate for where the next character will be drawn
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x18));
  _portPtr -> write(x);
}

void SerialLCD::setY(byte y) {
  // Set the y coordinate for where the next character will be drawn
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x19));
  _portPtr -> write(y);
}
void SerialLCD::setPos(byte x, byte y) {
  // Set x and y coordinates for where next character will be drawn
  setX(x);
  setY(y);
}

void SerialLCD::setPixel(byte x, byte y, byte state) {
  // Set pixel and coordinate x,y to value given by state.
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x10));
  _portPtr -> write(x);
  _portPtr -> write(y);
  _portPtr -> write(state);
}

void SerialLCD::drawLine(
                         byte startX,
                         byte startY,
                         byte endX,
                         byte endY,
                         byte state
                         )
{
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x0C));
  _portPtr -> write(startX);
  _portPtr -> write(startY);
  _portPtr -> write(endX);
  _portPtr -> write(endY);
  _portPtr -> write(state);
}

void SerialLCD::drawBox(
                        byte topLeftX,
                        byte topLeftY,
                        byte bottomRightX,
                        byte bottomRightY,
                        byte state
                        )
{
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x0F));
  _portPtr -> write(topLeftX);
  _portPtr -> write(topLeftY);
  _portPtr -> write(bottomRightX);
  _portPtr -> write(bottomRightY);
  _portPtr -> write(state);
}

void SerialLCD::drawCircle(
                           byte startX,
                           byte startY,
                           byte radius,
                           byte state
                           )
{
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x03));
  _portPtr -> write(startX);
  _portPtr -> write(startY);
  _portPtr -> write(radius);
  _portPtr -> write(state);
}

void SerialLCD::drawFilledBox(
                              byte topLeftX,
                              byte topLeftY,
                              byte bottomRightX,
                              byte bottomRightY,
                              byte state
                              )
{
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x12));
  _portPtr -> write(topLeftX);
  _portPtr -> write(topLeftY);
  _portPtr -> write(bottomRightX);
  _portPtr -> write(bottomRightY);
  _portPtr -> write(state);
}

void SerialLCD::toggleAuxFont() {
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x08));
}

void SerialLCD::toggleRevMode() {
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x14));
}

void SerialLCD::toggleSplash() {
  _portPtr -> write(UINT8_C(0x7C));
  _portPtr -> write(UINT8_C(0x15));
}

