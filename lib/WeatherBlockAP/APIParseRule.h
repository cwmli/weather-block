#ifndef APIPARSERULE_H
#define APIPARSERULE_H

enum APIValueType {
  NUMBER,
  ICON,
  TEXT,
  TIME
};

struct APIParseRule {
  int x;
  int y;
  // color is not used for APIValueType::ICON
  CRGB color;
  APIValueType type;
};

#endif
