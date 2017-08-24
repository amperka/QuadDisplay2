#include "QuadDisplay2.h"

const static uint8_t numerals[] = {QD_0, QD_1, QD_2, QD_3, QD_4, QD_5, QD_6, QD_7, QD_8, QD_9};

QuadDisplay::QuadDisplay(uint8_t pinCS)
{
    _pinCS = pinCS;
    _pinSCK = SCK;
    _pinDI = MOSI;
}

QuadDisplay::QuadDisplay(uint8_t pinCS, uint8_t pinMOSI, uint8_t pinSCK)
{
    _pinCS = pinCS;
    _pinDI = pinMOSI;
    _pinSCK = pinSCK;
}

void QuadDisplay::begin()
{
    pinMode(_pinSCK, OUTPUT);
    pinMode(_pinDI, OUTPUT);
    pinMode(_pinCS, OUTPUT);
    digitalWrite(_pinSCK, LOW);
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinCS, LOW);
}

void QuadDisplay::end()
{
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinSCK, LOW);
    digitalWrite(_pinCS, LOW);
    pinMode(_pinSCK, INPUT);
    pinMode(_pinDI, INPUT);
    pinMode(_pinCS, INPUT);
}

void QuadDisplay::beginWrite()
{
    digitalWrite(_pinSCK, LOW);
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinCS, LOW);
}

void QuadDisplay::writeData(uint8_t data, uint8_t n)
{
    shiftOut(_pinDI, _pinSCK, LSBFIRST, data);
}

void QuadDisplay::writeData(uint32_t data, uint8_t n)
{
    for (uint8_t i = n; i > 0; i--) {
        digitalWrite(_pinDI, (data & 1));
        digitalWrite(_pinSCK, HIGH);
        digitalWrite(_pinSCK, LOW);
        data >>= 1;
    }
}

void QuadDisplay::endWrite()
{
    digitalWrite(_pinDI, HIGH);
    digitalWrite(_pinSCK, HIGH);
    digitalWrite(_pinSCK, LOW);
    //
    digitalWrite(_pinCS, HIGH);
    digitalWrite(_pinDI, LOW);
    digitalWrite(_pinSCK, LOW);
}

void QuadDisplay::displayDigits(uint8_t digit1, uint8_t digit2, uint8_t digit3, uint8_t digit4)
{
    beginWrite();
    writeData(digit1);
    writeData(digit2);
    writeData(digit3);
    writeData(digit4);
    endWrite();
}

void QuadDisplay::displaySegments(uint32_t digits)
{
    beginWrite();
    writeData(digits);
    endWrite();
}

void QuadDisplay::displayClear()
{
    displayDigits(QD_NONE, QD_NONE, QD_NONE, QD_NONE);
}


void QuadDisplay::displayInt(int val, bool padZeros, uint8_t dots)
{
    uint8_t digits[4] = {0xff, 0xff, 0xff, 0xff};

    if (!padZeros && !val) {
        digits[3] = numerals[0];
    } else {
        bool negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 4; i--; ) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;
            val /= 10;

            if (!val && !padZeros)
                break;
        }
        if (negative)
            digits[i - 1] = QD_MINUS;

        digits[4 - dots] &= QD_DOT;

    }

    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}

void QuadDisplay::displayFloat(float val, uint8_t precision, bool padZeros)
{
    uint8_t dot = 0;
    while (precision) {
        val *= 10;
        --precision;
        dot++;
    }
    displayInt((int)val, padZeros, dot);
}

void QuadDisplay::displayTemperatureC(int val, bool padZeros)
{

    uint8_t digits[4] = {0xff, 0xff, QD_DEGREE, QD_C};
    
    if (!padZeros && !val)
        digits[1] = numerals[0];
    else {
        bool negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 2; i--; ) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;

            val /= 10;
            if (!val && !padZeros)
                break;
        }

        if (negative)
            digits[max(0, i-1)] = QD_MINUS;
    }
    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}

void QuadDisplay::displayHumidity(int val, bool padZeros)
{

    uint8_t digits[4] = {0xff, 0xff, QD_DEGREE, QD_UNDER_DEGREE};
    
    if (!padZeros && !val)
        digits[1] = numerals[0];
    else {
        bool negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 2; i--; ) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;

            val /= 10;
            if (!val && !padZeros)
                break;
        }

        if (negative)
            digits[max(0, i-1)] = QD_MINUS;
    }
    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}

void QuadDisplay::displayScore(int hour, int minute, bool blink)
{
    uint8_t digits[4] = {0xff, 0xff, 0xff, 0xff};
    if (!hour) {
        digits[0] = numerals[0];
        digits[1] = numerals[0];
    } else {
        if (hour < 10) {
            digits[0] = numerals[0];
        }
        int8_t i;
        for (i = 2; i--; ) {
            uint8_t digit = hour % 10;
            digits[i] = hour ? numerals[digit] : 0xff;

            hour /= 10;
            if (!hour)
                break;
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
       	for (i = 4; i--; ) {
            uint8_t digit = minute % 10;
            digits[i] = minute ? numerals[digit] : 0xff;

            minute /= 10;
            if (!minute)
                break;
        }
    }
    if (blink) {
    	if (millis() - _startMillis > 500) {
        	_state = !_state;
        	_startMillis = millis();
    	}
    	if (_state) {
        	digits[2] &= QD_DOT;
    	} else {
        	digits[2] |= ~QD_DOT;
    	}
    } else {
    	digits[2] &= QD_DOT;
    }
    displayDigits(digits[0], digits[1], digits[2], digits[3]);
}