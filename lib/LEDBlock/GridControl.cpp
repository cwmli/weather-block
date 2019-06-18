#include "GridControl.h"

uint8_t activeLeds[ROWS][COLUMNS];

uint8_t gridIndex(uint8_t row, uint8_t col) {
  // check if valid row col values
  if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
    return pgm_read_byte(&(LED_INDICES[row][col]));
  } else {
    return -1;
  }
}

void resetActiveLeds() {
  memset(activeLeds, ROWS * COLUMNS, sizeof(activeLeds));
} 

void setActiveLed(uint8_t row, uint8_t col) {
  if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
    setActiveLed(row, col, 1);
  }
}

void setActiveLed(uint8_t row, uint8_t col, uint8_t val) {
  if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
    activeLeds[row][col] = val;
  }
}

void removeActiveLed(uint8_t row, uint8_t col) {
  if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
    activeLeds[row][col] = 0;
  }
}

uint8_t getActiveLed(uint8_t row, uint8_t col) {
  if (row >= 0 && row < ROWS && col >= 0 && col < COLUMNS) {
    return activeLeds[row][col];
  }
}

void fillActiveLed(uint8_t row, uint8_t col, uint8_t num, bool vertical, bool lft_dwn) {
  uint8_t yModifier = 0;
  uint8_t xModifier = 0;

  if (vertical) {
    yModifier = 1;
    if (lft_dwn) {
      yModifier *= -1;
    }
  } else {
    xModifier = 1;
    if (lft_dwn) {
      xModifier *= -1;
    }
  }

  for (uint8_t i = 1; i <= num; i++) {
    setActiveLed(row + (xModifier * i), col + (yModifier * i));
  }
}

void fillChar(uint8_t row, uint8_t col, char c) {
  // row and col can be out of index so long as parts of c is still in the 
  // led grid

  const byte base = B10000;
  // retrieve bin representing char c
  byte binChar;

  for (int y = 0; y < FONT_Y; y++) {
    if (c - UCHAR_OFFSET < 26 || c - UCHAR_OFFSET >= 0) {
      binChar = pgm_read_byte(&UCHARACTERS[c - UCHAR_OFFSET][y]);

      for (int x = 0; x < FONT_X; x++) {
        // check if this led should be active
        if ((binChar & base) > 0) {
          setActiveLed(row + y, col + x);    
        }

        binChar = binChar << 1;
      }
    } else if (c - LCHAR_OFFSET < 26 || c - LCHAR_OFFSET >= 0) {
      binChar = pgm_read_byte(&LCHARACTERS[c - LCHAR_OFFSET][y]);

      byte tbase = base;
      if (c - LCHAR_OFFSET == 105) tbase = B1;//i
      if (c - LCHAR_OFFSET == 106) tbase = B100;//j
      if (c - LCHAR_OFFSET == 108) tbase = B10;//l
      int width = log(tbase) / log(2);

      for (int x = 0; x < width; x++) {
        // check if this led should be active
        if ((binChar & tbase) > 0) {
          setActiveLed(row + y, col + x);    
        }

        binChar = binChar << 1;
      }
    } else {
      for (int x = 0; x < FONT_X; x++) {
        removeActiveLed(row + y, col + x);
      }
    }
  }
}

void fillNum(uint8_t row, uint8_t col, char c) {
  // row and col can be out of index so long as parts of c is still in the 
  // led grid

  const byte base = B100;
  // retrieve bin representing int i
  byte binNum;

  for (int y = 0; y < NUM_Y; y++) {
    if (c - NUM_OFFSET < 10 || c - NUM_OFFSET >= 0) {
      binNum = pgm_read_byte(&NUMBERS[c - NUM_OFFSET][y]);
      for (int x = 0; x < NUM_X; x++) {
        // check if this led should be active
        if ((binNum & base) > 0) {
          setActiveLed(row + y, col + x);    
        }

        binNum = binNum << 1;
      }
    } else {
      for (int x = 0; x < NUM_X; x++) {
        removeActiveLed(row + y, col + x);
      }
    }
  }
}
