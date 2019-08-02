#ifndef CANVAS_H
#define CANVAS_H

#define SUBCANVAS_LIMIT 3

#include <Arduino.h>
#include "Elements.h"
#include "APIData.h"
#include "APIPlacementRule.h"

class Canvas {

  byte activeSubCanvas = 0;
  std::vector<Elements::Text *> elements[SUBCANVAS_LIMIT];
  std::map<String, APIPlacementRule> placementRules[SUBCANVAS_LIMIT];

  std::vector<Elements::Generic *> APIElements;
  APIData apiobj;

public:

  Canvas() {}

  // refresh elements generated by API data
  void update();

  void draw(LEDController * controller);

  void setSubCanvasParseRule(byte index, char * content);

  void incrementActiveSubCanvas();

  void addElement(Elements::Text * e);

  void resetElements();

  std::vector<Elements::Text *> getElements();

  String getElementsString();

  void setElements(char * content);

  void setAPI(String name, String url, long refresh, bool active, char * parseRules);

  void resetAPI();

  void toggleAPI();

  APIData getAPIData();

  void updateAPI(unsigned long curtime);
};

#endif
