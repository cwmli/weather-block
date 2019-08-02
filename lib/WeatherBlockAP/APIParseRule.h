#ifndef APIPARSERULE_H
#define APIPARSERULE_H

enum APIValueType {
  NUMBER,
  ICON,
  TEXT,
  TIME
};

struct APIParseRule {
  APIValueType type;
};

#endif
