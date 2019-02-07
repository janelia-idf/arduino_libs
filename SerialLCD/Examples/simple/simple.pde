#include <SoftwareSerial.h>
#include "SerialLCD.h"

#define MSG_SIZE 20

SoftwareSerial softSerial(8,7);
SerialLCD lcd(softSerial);

void setup() {
    softSerial.begin(115200);
    delay(2500);
    lcd.clearScreen();
    lcd.setBrightness(40);
    lcd.drawLine(0,10,100,10,1);
    lcd.drawBox(2,12,17,27,1);
}


void loop() {
    static uint16_t cnt = 0;
    char msg[MSG_SIZE]; 
    lcd.setPos(20,30);
    snprintf(msg,MSG_SIZE,"value = %d      ", cnt);
    lcd.print(msg);
    delay(100);
    cnt++;
}
