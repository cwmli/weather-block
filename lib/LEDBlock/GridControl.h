#ifndef GRID_CONTROL_H_
#define GRID_CONTROL_H_

#include <Arduino.h>
#include <avr/pgmspace.h>

#define ROWS        8
#define COLUMNS     8

// LED GRID INDICES
static const PROGMEM uint8_t LED_INDICES[ROWS][COLUMNS] = {{ 0,  1,  2,  3,  4,  5,  6,  7},
                                                           { 8,  9, 10, 11, 12, 13, 14, 15},
                                                           {16, 17, 18, 19, 20, 21, 22, 23},
                                                           {24, 25, 26, 27, 28, 29, 30, 31},
                                                           {32, 33, 34, 35, 36, 37, 38, 39},
                                                           {40, 41, 42, 43, 44, 45, 46, 47},
                                                           {48, 49, 50, 51, 52, 53, 54, 55},
                                                           {56, 57, 58, 59, 60, 61, 62, 63}};

#define FONT_X        5
#define FONT_Y        6
#define CHAR_OFFSET  65

// A..Z CAPS ONLY BINARY REPRESENTATION
static const PROGMEM byte CHARACTERS[26][FONT_Y] = {
  {B01110, B10001, B11111, B10001, B10001, B10001}, {B11110, B10001, B11110, B10001, B10001, B11110},//A, B
  {B01111, B10000, B10000, B10000, B10000, B01111}, {B11110, B10001, B10001, B10001, B10001, B11110},//C, D
  {B11111, B10000, B11110, B10000, B10000, B11111}, {B11111, B10000, B11110, B10000, B10000, B10000},//E, F
  {B01111, B10000, B10111, B10001, B10001, B01110}, {B10001, B10001, B11111, B10001, B10001, B10001},//G, H
  {B01110, B00100, B00100, B00100, B00100, B01110}, {B00011, B00010, B00010, B00010, B00010, B11100},//I, J
  {B10001, B10010, B10100, B11010, B10001, B10001}, {B10000, B10000, B10000, B10000, B10000, B11111},//K, L
  {B10001, B11011, B10101, B10001, B10001, B10001}, {B10001, B11001, B10101, B10011, B10001, B10001},//M, N
  {B01110, B10001, B10001, B10001, B10001, B01110}, {B11110, B10001, B11110, B10000, B10000, B10000},//O, P
  {B01110, B10001, B10001, B10101, B10010, B01101}, {B11110, B10001, B11110, B10001, B10001, B10001},//Q, R
  {B01111, B10000, B01110, B00001, B00001, B11110}, {B11111, B00100, B00100, B00100, B00100, B00100},//S, T
  {B10001, B10001, B10001, B10001, B10001, B01110}, {B10001, B10001, B10001, B01010, B01010, B00100},//U, V
  {B10001, B10001, B10101, B10101, B10101, B01010}, {B10001, B01010, B00100, B01010, B10001, B10001},//W, X
  {B10001, B01010, B00100, B00100, B00100, B00100}, {B11111, B00010, B00100, B01000, B10000, B11111}};//Y, Z

#define NUM_X        3
#define NUM_Y        6
#define NUM_OFFSET   48

// 0..9 and special char :
static const PROGMEM byte NUMBERS[10][NUM_Y] = {
  {B010, B101, B101, B101, B101, B010}, {B010, B110, B010, B010, B010, B111},//0, 1
  {B110, B001, B010, B100, B100, B111}, {B110, B001, B110, B001, B001, B110},//2, 3
  {B101, B101, B101, B111, B001, B001}, {B111, B100, B110, B001, B001, B110},//4, 5
  {B011, B100, B110, B101, B101, B010}, {B111, B001, B001, B010, B010, B100},//6, 7
  {B010, B101, B010, B101, B101, B010}, {B010, B101, B101, B011, B001, B110}};//8, 9

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
void fillNum(uint8_t row, uint8_t col, char i);
#endif
