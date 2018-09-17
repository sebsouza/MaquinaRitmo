/*
 * FastShiftIn.h
 *
 *  Created on: Sep 16, 2018
 *      Author: sebsouza
 */

#ifndef FASTSHIFTIN_H_
#define FASTSHIFTIN_H_


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define FASTSHIFTIN_LIB_VERSION "0.1.3"

#define FASTSHIFTIN_NOVALUE -1

class FastShiftIn
{
public:
    FastShiftIn(const uint8_t, const uint8_t, const uint8_t);
    int read(void);

private:
    uint8_t _bitorder;
    int _value;

    uint8_t _databit;
    volatile uint8_t *_datain;

    uint8_t _clockbit;
    volatile uint8_t *_clockin;
};


#endif /* FASTSHIFTIN_H_ */
