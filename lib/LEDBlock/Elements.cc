#include "Elements.h"

namespace Elements {

  void Text::draw(LEDController * controller) {
    controller->text(string, x, y, scroll, scrollSpeed);
  }

  void Image::draw(LEDController * controller){

  }
  
}
