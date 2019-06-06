#ifndef GRID_CONTROL_H_
#define GRID_CONTROL_H_

#include <Arduino.h>
#include <avr/pgmspace.h>

#define ROWS        8
#define COLUMNS     8

// LED GRID INDICES
static const PROGMEM uint8_t LED_INDICES[ROWS][COLUMNS] = {{ 0,  1,  2,  3,  4,  5,  6,  7},
                                                           { 8,  9, 10, 11, 11, 12, 14, 15},
                                                           {16, 17, 18, 19, 20, 21, 22, 23},
                                                           {24, 25, 26, 27, 28, 29, 30, 31},
                                                           {32, 33, 34, 35, 36, 37, 38, 39},
                                                           {40, 41, 42, 43, 44, 45, 46, 47},
                                                           {48, 49, 50, 51, 52, 53, 54, 55}};

#define FONT_X        3
#define FONT_Y        4
#define CHAR_OFFSET  65

// A..Z CAPS ONLY BINARY REPRESENTATION
static const PROGMEM byte CHARACTERS[26][FONT_Y] = {{B010, B101, B111, B101}, {B110, B110, B101, B110}, {B011, B100, B100, B011},
                                                    {B110, B101, B101, B110}, {B111, B100, B110, B111}, {B111, B100, B110, B100},
                                                    {B011, B100, B101, B011}, {B101, B101, B111, B101}, {B111, B010, B010, B111},
                                                    {B001, B001, B101, B010}, {B101, B101, B110, B101}, {B100, B100, B100, B111},
                                                    {B111, B111, B111, B101}, {B101, B111, B111, B101}, {B111, B101, B101, B111},
                                                    {B110, B101, B110, B100}, {B010, B101, B111, B011}, {B110, B101, B110, B101},
                                                    {B011, B100, B001, B110}, {B111, B010, B010, B010}, {B101, B101, B101, B111},
                                                    {B101, B101, B101, B010}, {B101, B111, B111, B111}, {B101, B010, B010, B101},
                                                    {B101, B101, B010, B010}, {B111, B001, B100, B111}};

#define NUM_X        2
#define NUM_Y        4

// 0..9 and special char :
static const PROGMEM byte NUMBERS[10][NUM_Y] = {{B11, B11, B11, B11}, {B01, B11, B01, B01}, {B11, B01, B10, B11}, 
                                                {B11, B01, B11, B11}, {B01, B11, B11, B01}, {B11, B10, B01, B11},
                                                {B11, B10, B11, B11}, {B11, B01, B01, B01}, {B11, B11, B00, B11},
                                                {B11, B11, B01, B01}};

// index(row, col) returns the corresponding index given
// a specified row and col returns -1 if invalid row col vals
uint8_t gridIndex(uint8_t row, uint8_t col);

// resetActiveLeds() sets all led indices to 0 (OFF)
void resetActiveLeds();

// setActiveLed(row, col) sets an led index to be on where row
// col are valid
void setActiveLed(uint8_t row, uint8_t col);

// setActiveLed(row, col) sets an led index to be a specific value where row
// col are valid
void setActiveLed(uint8_t row, uint8_t col, uint8_t val);

// removeActiveLed(row, col) sets an led index to be inactive
void removeActiveLed(uint8_t row, uint8_t col);

// getActiveLed(row, col) returns 1 if the led at row col is active, 0
// otherwise
uint8_t getActiveLed(uint8_t row, uint8_t col);

// fillActiveLed(row, col, num, vertical, left) set a column or row
// of leds to active given a specified number and orientation (vertical/left)
void fillActiveLed(uint8_t row, uint8_t col, uint8_t num, bool vertical, bool lft_down);

// fillChar(c) sets active leds at target location defined by row
// and col to display char c, row and col do not have to be valid
void fillChar(uint8_t row, uint8_t col, char c);

// fillNum(c) sets active leds at target location defined by row
// and col to display int i, row and col do not have to be valid
void fillNum(uint8_t row, uint8_t col, uint8_t i);
#endif
