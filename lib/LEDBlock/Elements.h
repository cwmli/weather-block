#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <Arduino.h>

#include "LEDController.h"
#include "Icons.h"

namespace Elements {

  struct Generic {
    int x;
    int y;
    int xOffset;
    int yOffset;
    bool scroll;
    unsigned int scrollSpeed;
    unsigned long lastScrollUpdate;

    Generic(int x, int y, bool s, unsigned int spd) : x(x), y(y), scroll(s), scrollSpeed(spd) {
      xOffset = 0;
      yOffset = 0;
    };

    virtual ~Generic() {};

    virtual void draw(LEDController * controller) {};
  };

  struct Text : Generic {
    String string;
    CRGB color;

    Text(String str, int x, int y, bool s, int spd) : Generic(x, y, s, spd), string(str) {
      color = CRGB::White;
    };

    Text(String str, CRGB color, int x, int y, bool s, int spd) : Generic(x, y, s, spd), string(str), color(color) {};

    ~Text() {};

    void draw(LEDController * controller);
  };

  struct Icon : Generic {
    String iconStr;

    Icon(int x, int y, bool s, int spd, String iconStr) : Generic(x, y, s, spd), iconStr(iconStr) {};

    ~Icon() {};

    void draw(LEDController * controller);
  };
}

#endif
