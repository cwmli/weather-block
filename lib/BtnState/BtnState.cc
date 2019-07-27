#include "BtnState.h"

uint8_t BtnState::get() {
  uint8_t currentPinState = digitalRead(pin);

  if (currentPinState == HIGH && lastPinState == LOW) {
    debounceStartTime = millis();
    lastPinState = currentPinState;
  }

  if ((millis() - debounceStartTime) > DB_TIME) {

    if (currentPinState == HIGH) {
      if (initialPressTime == 0) {
        initialPressTime = millis();
      }
      pressedTime = millis() - initialPressTime;
    }
    
    uint8_t result = N_PRESS;
    if (lastPinState == HIGH && currentPinState == LOW) {
      // released
      if (pressedTime > S_PRESS_TIME && pressedTime < L_PRESS_TIME) {
        result = S_PRESS;
      } else if (pressedTime > L_PRESS_TIME && pressedTime < XL_PRESS_TIME) {
        result = L_PRESS;
      } else if (pressedTime > XL_PRESS_TIME) {
        result = XL_PRESS;
      }

      initialPressTime = 0;
      pressedTime = 0;
    }
    
    lastPinState = currentPinState;
    return result;
  }
}
