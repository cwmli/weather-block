#ifndef BTNSTATE_H
#define BTNSTATE_H

#include <Arduino.h>

// Numbers are in milliseconds
#define DB_TIME       25

#define XL_PRESS_TIME 1500
#define L_PRESS_TIME  250
#define S_PRESS_TIME  25

#define XL_PRESS      3
#define L_PRESS       2
#define S_PRESS       1
#define N_PRESS       0

class BtnState {

  uint8_t pin;
  uint8_t lastPinState;

  unsigned long initialPressTime;
  unsigned long pressedTime;
  unsigned long debounceStartTime;

  public:
    BtnState(uint8_t pin) : pin(pin) {
      lastPinState = LOW;
      pressedTime = 0;
      debounceStartTime = 0;
    }

    uint8_t get();
};

#endif
