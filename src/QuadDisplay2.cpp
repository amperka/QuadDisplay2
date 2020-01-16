/*
 * This file is a part of QuadDisplay2 library.
 *
 * Product page: https://https://amperka.ru/product/troyka-quad-display
 * © Amperka LLC (https://amperka.com, dev@amperka.com)
 * 
 * Author: Igor Dementiev <igor@amperka.ru>
 * License: GPLv3, all text here must be included in any redistribution.
 */

#include "QuadDisplay2.h"

const static uint8_t numerals[] = { QD_0, QD_1, QD_2, QD_3, QD_4, QD_5, QD_6, QD_7, QD_8, QD_9 };

QuadDisplay::QuadDisplay(uint8_t pinCS) {
    _pinCS = pinCS;
    _useSPI = true;
}

QuadDisplay::QuadDisplay(uint8_t pinCS, boolean useSPI) {
    if (useSPI) {
        _pinCS = pinCS;
        _useSPI = true;
    } else {
        _pinCS = pinCS;
        _pinSCK = SCK;
        _pinDI = MOSI;
        _useSPI = false;
    }
}

QuadDisplay::QuadDisplay(uint8_t pinCS, uint8_t pinMOSI, uint8_t pinSCK) {
    _pinCS = pinCS;
    _pinDI = pinMOSI;
    _pinSCK = pinSCK;
}

void QuadDisplay::begin() {
    pinMode(_pinCS, OUTPUT);
    digitalWrite(_pinCS, LOW);
    if (_useSPI) {
        SPI.begin();
    } else {
        pinMode(_pinSCK, OUTPUT);
        pinMode(_pinDI, OUTPUT);
        digitalWrite(_pinSCK, LOW);
        digitalWrite(_pinDI, LOW);
    }
}

void QuadDisplay::end() {
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinSCK, LOW);
    digitalWrite(_pinCS, LOW);
    pinMode(_pinSCK, INPUT);
    pinMode(_pinDI, INPUT);
    pinMode(_pinCS, INPUT);
}

void QuadDisplay::beginWrite() {
    digitalWrite(_pinSCK, LOW);
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinCS, LOW);
}

void QuadDisplay::writeData(uint8_t data, uint8_t n) {
    shiftOut(_pinDI, _pinSCK, LSBFIRST, data);
}

void QuadDisplay::writeData(uint32_t data, uint8_t n) {
    for (uint8_t i = n; i > 0; i--) {
        digitalWrite(_pinDI, (data & 1));
        digitalWrite(_pinSCK, HIGH);
        digitalWrite(_pinSCK, LOW);
        data >>= 1;
    }
}

void QuadDisplay::endWrite() {
    digitalWrite(_pinDI, HIGH);
    digitalWrite(_pinSCK, HIGH);
    digitalWrite(_pinSCK, LOW);
    digitalWrite(_pinCS, HIGH);
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinSCK, LOW);
}

uint8_t QuadDisplay::reverse(uint8_t x) {
    uint8_t mask = x & 0x01;
    x = x >> 1;
    x = ((x & 0x55) << 1) | ((x & 0xAA) >> 1);
    x = ((x & 0xCC) >> 2) | ((x & 0x33) << 2);
    x = (x >> 4) | (x << 4);
    x = x | mask;
    return x;
}

void QuadDisplay::setDots(uint8_t array[]) {
    for (int i = 1; i < 4; i++) {
        if (array[i] & 0x01 == 0) {
            array[i - 1] &= 0xFE;
            array[i] |= 0x01;
        }
    }
}

void QuadDisplay::displayDigits(uint8_t digit1, uint8_t digit2, uint8_t digit3,
    uint8_t digit4) {
    if (_useSPI) {
        uint8_t digitsArray[] = { digit1, digit2, digit3, digit4 };
        setDots(digitsArray);
        digitalWrite(_pinCS, LOW);
        for (int i = 0; i < 4; i++) {
            SPI.transfer(reverse(digitsArray[i]));
        }
        digitalWrite(_pinCS, HIGH);
    } else {
        beginWrite();
        writeData(digit1);
        writeData(digit2);
        writeData(digit3);
        writeData(digit4);
        endWrite();
    }
}

void QuadDisplay::displaySegments(uint32_t digits) {
    beginWrite();
    writeData(digits);
    endWrite();
}

void QuadDisplay::displayClear() {
    displayDigits(QD_NONE, QD_NONE, QD_NONE, QD_NONE);
}

uint8_t QuadDisplay::getBit(uint8_t byte, uint8_t number) {
    return ((byte >> (8 - number)) & 1);
}

void QuadDisplay::displayInt(int val, bool padZeros, uint8_t dots) {
    uint8_t digits[4] = { 0xff, 0xff, 0xff, 0xff };

    if (!padZeros && !val) {
        digits[3] = numerals[0];
    } else {
        bool negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 4; i--;) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;
            val /= 10;

            if (!val && !padZeros) {
                break;
            }
        }
        if (negative) {
            digits[i - 1] = QD_MINUS;
        }

        if (getBit(dots, 8)) {
            digits[3] &= QD_DOT;
        }
        if (getBit(dots, 7)) {
            digits[2] &= QD_DOT;
        }
        if (getBit(dots, 6)) {
            digits[1] &= QD_DOT;
        }
        if (getBit(dots, 5)) {
            digits[0] &= QD_DOT;
        }
    }

    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}

void QuadDisplay::displayFloat(float val, uint8_t precision, bool padZeros) {
    uint8_t dot = 0b0001;
    while (precision) {
        val *= 10;
        --precision;
        dot <<= 1;
    }
    displayInt((int)val, padZeros, dot);
}

void QuadDisplay::displayTemperatureC(int val, bool padZeros) {

    uint8_t digits[4] = { 0xff, 0xff, QD_DEGREE, QD_C };

    if (!padZeros && !val) {
        digits[1] = numerals[0];
    } else {
        bool negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 2; i--;) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;

            val /= 10;
            if (!val && !padZeros) {
                break;
            }
        }

        if (negative) {
            digits[max(0, i - 1)] = QD_MINUS;
        }
    }
    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}

void QuadDisplay::displayHumidity(int val, bool padZeros) {

    uint8_t digits[4] = { 0xff, 0xff, QD_DEGREE, QD_UNDER_DEGREE };

    if (!padZeros && !val) {
        digits[1] = numerals[0];
    } else {
        bool negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 2; i--;) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;

            val /= 10;
            if (!val && !padZeros) {
                break;
            }
        }
        if (negative) {
            digits[max(0, i - 1)] = QD_MINUS;
        }
    }
    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}

void QuadDisplay::displayScore(int hour, int minute, bool blink) {
    uint8_t digits[4] = { 0xff, 0xff, 0xff, 0xff };
    if (!hour) {
        digits[0] = numerals[0];
        digits[1] = numerals[0];
    } else {
        if (hour < 10) {
            digits[0] = numerals[0];
        }
        int8_t i;
        for (i = 2; i--;) {
            uint8_t digit = hour % 10;
            digits[i] = hour ? numerals[digit] : 0xff;

            hour /= 10;
            if (!hour) {
                break;
            }
        }
    }

    if (!minute) {
        digits[2] = numerals[0];
        digits[3] = numerals[0];
    } else {
        if (minute < 10) {
            digits[2] = numerals[0];
        }
        int8_t i;
        for (i = 4; i--;) {
            uint8_t digit = minute % 10;
            digits[i] = minute ? numerals[digit] : 0xff;

            minute /= 10;
            if (!minute) {
                break;
            }
        }
    }
    if (blink) {
        if (millis() - _startMillis > 500) {
            _state = !_state;
            _startMillis = millis();
        }
        if (_state) {
            digits[1] &= QD_DOT;
        } else {
            digits[1] |= ~QD_DOT;
        }
    } else {
        digits[1] &= QD_DOT;
    }
    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}
