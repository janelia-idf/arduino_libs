#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class LookupTable {
    public:
        LookupTable();
        int getValue(int x);
        bool setTable(int _table[][2], unsigned int _size);
    private:
        unsigned int size;
        int (*table)[2];
};

#endif
