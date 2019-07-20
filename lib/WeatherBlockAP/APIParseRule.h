#ifndef APIPARSERULE_H
#define APIPARSERULE_H

enum APIValueType {
  NUMBER,
  ICON,
  TEXT
};

struct APIParseRule {
  int x;
  int y;
  APIValueType type;
};

#endif
