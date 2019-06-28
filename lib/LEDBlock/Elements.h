#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <Arduino.h>

#include "LEDController.h"

namespace Elements {

  struct Generic {
    int x;
    int y;
    bool scroll;
    int scrollSpeed;

    Generic(int x, int y, bool s, int spd) : x(x), y(y), scroll(s), scrollSpeed(spd) {};

    virtual void draw(LEDController * controller) {};
  };

  struct Text : Generic {
    String string;

    Text(String str, int x, int y, bool s, int spd) : Generic(x, y, s, spd), string(str) {};

    void draw(LEDController * controller);
  };

  struct Image : Generic {
    int w;
    int h;
    int** img;

    Image(int x, int y, bool s, int spd, int w, int h, int** img) : Generic(x, y, s, spd), w(w), h(h), img(img) {};

    void draw(LEDController * controller);
  };
}

#endif
