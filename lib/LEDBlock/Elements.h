#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <Arduino.h>

#include "LEDController.h"
#include "Icons.h"

namespace Elements {

  struct Generic {
    int x;
    int y;
    bool scroll;
    int scrollSpeed;
    unsigned long lastScrollUpdate;

    Generic(int x, int y, bool s, int spd) : x(x), y(y), scroll(s), scrollSpeed(spd) {};

    virtual ~Generic() {};

    virtual void draw(LEDController * controller) {};
  };

  struct Text : Generic {
    String string;

    Text(String str, int x, int y, bool s, int spd) : Generic(x, y, s, spd), string(str) {};

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
