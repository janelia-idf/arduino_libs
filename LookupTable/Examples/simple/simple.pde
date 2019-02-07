#include "Streaming.h"
#include "LookupTable.h"

#define TABLE_SIZE 3
int table[TABLE_SIZE][2] = {
    {0, 0},     
    {10, 20}, 
    {20, 40}
    };


LookupTable lookup;

void setup() {
    bool rtnVal;
    Serial.begin(9600);
    rtnVal = lookup.setTable(table,TABLE_SIZE);
    Serial << "setTable rtnVal = " << rtnVal << endl;
}

void loop() {
    int y;
    for (int x= -10; x<30; x+=1) {
       y = lookup.getValue(x);
       Serial << "x = " << _DEC(x) << " y = " << _DEC(y) << endl;
    }
    Serial << endl;
    delay(1000);
}
