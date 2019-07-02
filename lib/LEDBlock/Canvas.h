#ifndef CANVAS_H
#define CANVAS_H

#include <Arduino.h>
#include "Elements.h"
#include "APIData.h"

class Canvas {

  std::vector<Elements::Generic *> elements;

  std::vector<Elements::Generic *> APIElements;
  APIData apiobj;

public:

  Canvas() {}

  // refresh elements generated by API data
  void update();

  void addElement(Elements::Generic * e);

  void removeElement(int index);

  std::vector<Elements::Generic *> getElements();

  void draw(LEDController * controller);

  void setAPI(String name, String url, long refresh, std::map<String, APIParseRule> parseRules);

  void resetAPI();

  APIData getAPIData();

  void updateAPI(long curtime);
};

#endif