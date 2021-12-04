#include "ClockCanvas.h"

#include <TimeLib.h>

void ClockCanvas::update() {
    Canvas::update();

    char tm[24]; 
    snprintf(tm, sizeof(tm), "%02d:%02d", hourFormat12(), minute());

    resetElements();

    addElement(new Elements::Text(tm, CRGB::Red, 4, 1, false, 0));
}