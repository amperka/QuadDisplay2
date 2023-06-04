/*
 * This file is a part of QuadDisplay2 library.
 *
 * Product page: https://https://amperka.ru/product/troyka-quad-display
 * © Amperka LLC (https://amperka.com, dev@amperka.com)
 * 
 * Author: Igor Dementiev <igor@amperka.ru>
 * License: GPLv3, all text here must be included in any redistribution.
 */

#ifndef __QUAD_DISPLAY__
#define __QUAD_DISPLAY__

#include <Arduino.h>
#include <SPI.h>
#include <inttypes.h>

/*
 *            a
 *         -----
 *     f /     / b
 *      /  g  /
 *      -----
 *  e /     / c
 *   /  d  /
 *   -----    • dp
 *
 *  Let's code the PI number
 *
 *  Bit order: g f e d c b a dp     0b     0x
 *      3.     * . . * * * * *   01100000  60
 *      1      . . . . * * . .   11110011  f3
 *      4      * * . . * * . .   00110011  33
 *      2      * . * * . * * .   01001001  49
 *
 *  "3.142" = 0x60f33349
 */

#define QD_NONE 0b11111111
#define QD_DOT 0b11111110
#define QD_MINUS 0b01111111

#define QD_UNDERSCORE 0b11101111
#define QD_DEGREE 0b00111001
#define QD_UNDER_DEGREE 0b01000111

#define QD_0 0b10000001
#define QD_1 0b11110011
#define QD_2 0b01001001
#define QD_3 0b01100001
#define QD_4 0b00110011
#define QD_5 0b00100101
#define QD_6 0b00000101
#define QD_7 0b11110001
#define QD_8 0b00000001
#define QD_9 0b00100001

#define QD_A 0b00010001
#define QD_a 0b01000001
#define QD_b 0b00000111
#define QD_C 0b10001101
#define QD_c 0b01001111
#define QD_d 0b01000011
#define QD_E 0b00001101
#define QD_f 0b00011101
#define QD_F QD_f
#define QD_H 0b00010011
#define QD_h 0b00010111
#define QD_I QD_1
#define QD_J 0b11100011
#define QD_K QD_H
#define QD_L 0b10001111
#define QD_M 0b11010101
#define QD_n 0b01010111
#define QD_o 0b01000111
#define QD_O QD_0
#define QD_P 0b00011001
#define QD_r 0b01011111
#define QD_S 0b00100101
#define QD_t 0b00001111
#define QD_u 0b11000111
#define QD_U 0b10000011
#define QD_Y 0b00100011

class QuadDisplay {
private:
    unsigned long _startMillis = millis();
    bool _state;
    uint8_t _pinSCK;
    uint8_t _pinDI;
    uint8_t _pinCS;
    bool _useSPI;
    void beginWrite();
    void pulse(uint8_t pin);
    void writeData(uint8_t data);
    void writeData(uint32_t data);
    void endWrite();
    uint8_t getBit(uint8_t, uint8_t);
    uint8_t getBit(uint32_t, uint8_t);

public:
    QuadDisplay(uint8_t pinCS);
    QuadDisplay(uint8_t pinCS, boolean useSPI);
    QuadDisplay(uint8_t pinCS, uint8_t pinCOPI, uint8_t pinSCK);
    void begin();
    void end();
    void displayInt(int val, bool padZeros = false, uint8_t dots = 0x00);
    void displayFloat(float val, uint8_t precision, bool padZeros = false);
    void displayDigits(uint8_t digit1, uint8_t digit2, uint8_t digit3, uint8_t digit4);
    void displaySegments(uint32_t digits);
    void displayTemperatureC(int val, bool padZeros = false);
    void displayHumidity(int val, bool padZeros = false);
    void displayScore(int hour, int minute, bool blink = false);
    void displayClear();
};

#endif
