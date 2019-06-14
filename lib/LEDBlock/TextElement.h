#ifndef TEXT_ELEMENT_H
#define TEXT_ELEMENT_H

struct TextElement {
  String string;
  int x;
  int y;
  bool scroll;
  int scrollSpeed;

  TextElement() {}

  TextElement(String str, int x, int y, bool s, int spd) : string(str), x(x), y(y), scroll(s), scrollSpeed(spd) {}
};

#endif
