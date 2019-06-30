#include "Elements.h"

namespace Elements {

  void Text::draw(LEDController * controller) {
    if (scroll && millis() - lastScrollUpdate > scrollSpeed) {
      lastScrollUpdate = millis();

      x -= 1;
    }
    controller->text(string, x, y);
  }

  void Icon::draw(LEDController * controller){

  }
  
}
