#ifndef ICONS_H
#define ICONS_H

#include <FastLED.h>

namespace Icons {

  struct Base {
    int w;
    int h;
    int** cdata;
    CRGB* palette;
  };

  static const Base ClearDay {8, 8, 
    new int*[8] {new int [8] {0,1,0,1,0,1,0,0}, new int [8] {0,0,2,2,2,2,0,1}, new int [8] {1,2,2,2,2,2,2,0}, new int [8] {0,2,2,2,2,2,2,1},
                 new int [8] {1,2,2,2,2,2,2,0}, new int [8] {0,2,2,2,2,2,2,1}, new int [8] {1,0,2,2,2,2,0,0}, new int [8] {0,0,1,0,1,0,1,0}},
    new CRGB[2] {CRGB::CRGB(230, 163, 31), CRGB::CRGB(230, 187, 31)}};

  static const Base CloudyDay {8, 8,
    new int*[8] {new int [8] {0,1,0,1,0,1,0,0}, new int [8] {0,0,2,2,2,2,0,1}, new int [8] {1,2,2,2,2,2,2,0}, new int [8] {0,2,2,2,2,2,2,1},
                 new int [8] {1,2,2,2,3,3,3,0}, new int [8] {0,2,2,3,3,3,3,3}, new int [8] {1,0,3,3,3,3,3,3}, new int [8] {0,0,1,0,1,0,1,0}},
    new CRGB[3] {CRGB::CRGB(230, 163, 31), CRGB::CRGB(230, 187, 31), CRGB::CRGB(191, 191, 191)}};

  static const Base ClearNight {8, 8,
    new int*[8] {new int [8] {0,0,1,2,0,0,0,0}, new int [8] {0,1,2,0,0,0,1,0}, new int [8] {1,2,0,0,0,1,1,1}, new int [8] {1,2,0,0,0,0,1,0},
                 new int [8] {1,2,0,0,0,0,0,0}, new int [8] {1,1,2,0,0,0,0,1}, new int [8] {0,1,1,2,2,2,1,0}, new int [8] {0,0,1,1,1,1,0,0}},
    new CRGB[2] {CRGB::CRGB(236, 198, 0), CRGB::CRGB(230, 163, 31)}};

  static const Base CloudyNight {8, 8,
    new int*[8] {new int [8] {0,0,1,2,0,0,0,0}, new int [8] {0,1,2,0,0,0,1,0}, new int [8] {1,2,0,0,0,1,1,1}, new int [8] {1,2,0,0,0,0,1,0},
                 new int [8] {1,2,0,0,0,3,3,0}, new int [8] {1,1,2,0,3,3,3,1}, new int [8] {0,1,1,3,3,3,3,3}, new int [8] {0,0,1,1,1,1,0,0}},
    new CRGB[3] {CRGB::CRGB(236, 198, 0), CRGB::CRGB(230, 163, 31), CRGB::CRGB(191, 191, 191)}};

  static const Base Rain {8, 8,
    new int*[8] {new int [8] {0,0,1,1,2,0,0,0}, new int [8] {0,1,1,1,1,2,2,0}, new int [8] {1,1,2,2,1,1,1,2}, new int [8] {1,1,1,1,2,1,1,1},
                 new int [8] {0,1,1,1,1,1,1,0}, new int [8] {0,3,0,3,0,3,0,0}, new int [8] {0,0,3,0,3,0,3,0}, new int [8] {0,0,0,3,0,3,0,0}},
    new CRGB[3] {CRGB::CRGB(118, 146, 160), CRGB::CRGB(132, 168, 188), CRGB::CRGB(134, 198, 230)}};
  
  static const Base Cloudy {8, 8,
    new int*[8] {new int [8] {0,0,0,0,0,0,0,0}, new int [8] {0,0,1,1,1,0,0,0}, new int [8] {0,1,1,1,1,1,0,0}, new int [8] {1,1,1,1,2,2,1,0},
                 new int [8] {1,1,1,2,2,2,2,1}, new int [8] {0,0,2,2,2,2,2,2}, new int [8] {0,0,0,0,0,0,0,0}, new int [8] {0,0,0,0,0,0,0,0}},
    new CRGB[2] {CRGB::CRGB(191, 191, 191), CRGB::CRGB(166, 166, 166)}};

  static const Base Fog {8, 8,
    new int*[8] {new int [8] {0,0,0,0,0,0,0,0}, new int [8] {0,1,1,1,1,1,1,0}, new int [8] {0,0,0,0,0,0,0,0}, new int [8] {0,1,1,1,1,1,1,0},
                 new int [8] {0,0,0,0,0,0,0,0}, new int [8] {0,1,1,1,1,1,1,0}, new int [8] {0,0,0,0,0,0,0,0}, new int [8] {0,0,0,0,0,0,0,0}},
    new CRGB[1] {CRGB::CRGB(191, 191, 191)}};
  
  static const Base Wind {8, 8,
    new int*[8] {new int [8] {0,0,0,0,0,0,0,0}, new int [8] {1,0,0,0,0,0,1,0}, new int [8] {0,1,1,1,0,0,0,1}, new int [8] {0,0,0,0,1,1,1,0},
                 new int [8] {0,0,0,0,0,0,0,0}, new int [8] {1,0,0,0,0,0,1,0}, new int [8] {0,1,1,1,0,0,0,1}, new int [8] {0,0,0,0,1,1,1,0}},
    new CRGB[1] {CRGB::CRGB(150, 208, 124)}};

  static const Base Snow {8, 8,
    new int*[8] {new int [8] {1,0,0,0,0,0,0,1}, new int [8] {0,1,1,0,0,1,1,0}, new int [8] {0,1,0,1,1,0,1,0}, new int [8] {0,0,1,1,1,1,0,0},
                 new int [8] {0,0,1,1,1,1,0,0}, new int [8] {0,1,0,1,1,0,1,0}, new int [8] {0,1,1,0,0,1,1,0}, new int [8] {1,0,0,0,0,0,0,1}},
    new CRGB[1] {CRGB::White}};
}
#endif
