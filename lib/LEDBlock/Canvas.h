#ifndef CANVAS_H
#define CANVAS_H

#define SUBCANVAS_LIMIT 3

#include <Arduino.h>
#include "Elements.h"
#include "APIData.h"

class Canvas {

  byte activeSubCanvas = 0;
  std::vector<Elements::Text *> elements[SUBCANVAS_LIMIT];

  std::vector<Elements::Generic *> APIElements[SUBCANVAS_LIMIT];
  APIData apiobj;

public:

  Canvas() {}

  // refresh elements generated by API data
  void update();

  void draw(LEDController * controller);

  void incrementActiveSubCanvas();

  void addElement(Elements::Text * e);

  void resetElements();

  std::vector<Elements::Text *> getElements();

  String getElementsString();

  void setElements(char * content);

  void setAPI(String name, String url, long refresh, bool active, std::map<String, APIParseRule> parseRules);

  void resetAPI();

  void toggleAPI();

  APIData getAPIData();

  void updateAPI(unsigned long curtime);
};

#endif
