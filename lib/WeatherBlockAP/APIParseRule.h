#ifndef APIPARSERULE_H
#define APIPARSERULE_H

enum APIValueType {
  NUMBER,
  ICON
};

struct APIParseRule {
  int x;
  int y;
  APIValueType type;
};

#endif
