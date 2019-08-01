#include "Elements.h"

namespace Elements {

  void Text::draw(LEDController * controller) {
    if (scroll && millis() - lastScrollUpdate > scrollSpeed) {
      lastScrollUpdate = millis();

      x -= 1;
    }
    controller->text(string, x, y, color);
  }

  void Icon::draw(LEDController * controller){
    Icons::Base resolvedIcon;

    if (iconStr == "partly-cloudy-night")      { memcpy_P(&resolvedIcon, &Icons::Weather[8], sizeof(Icons::Base)); }
    else if (iconStr == "clear-night")         { memcpy_P(&resolvedIcon, &Icons::Weather[1], sizeof(Icons::Base)); }
    else if (iconStr == "rain")                { memcpy_P(&resolvedIcon, &Icons::Weather[2], sizeof(Icons::Base)); }
    else if (iconStr == "snow")                { memcpy_P(&resolvedIcon, &Icons::Weather[3], sizeof(Icons::Base)); }
    else if (iconStr == "sleet")               { memcpy_P(&resolvedIcon, &Icons::Weather[3], sizeof(Icons::Base)); }
    else if (iconStr == "wind")                { memcpy_P(&resolvedIcon, &Icons::Weather[4], sizeof(Icons::Base)); }
    else if (iconStr == "fog")                 { memcpy_P(&resolvedIcon, &Icons::Weather[5], sizeof(Icons::Base)); }
    else if (iconStr == "cloudy")              { memcpy_P(&resolvedIcon, &Icons::Weather[6], sizeof(Icons::Base)); }
    else if (iconStr == "partly-cloudy-day")   { memcpy_P(&resolvedIcon, &Icons::Weather[7], sizeof(Icons::Base)); }
    else                                       { memcpy_P(&resolvedIcon, &Icons::Weather[0], sizeof(Icons::Base)); }

    controller->icon(&resolvedIcon, x, y);
  }  
}
