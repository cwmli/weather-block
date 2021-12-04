#ifndef APICANVAS_H
#define APICANVAS_H

#include "Canvas.h"
#include "APIData.h"
#include "Elements.h"

class APICanvas : public Canvas {

  std::vector<Elements::Generic *> APIElements;
  APIData apiobj;

public:

  APICanvas() {
    setAPI("notset", "", 60 * 60, false, "");
    addElement(new Elements::Text("unset", 1, 1, false, 0));
  };

  APICanvas(String name, String url, long refresh, bool active, char * parseRules) {
    setAPI(name, url, refresh, active, parseRules);
  };

  CanvasType type() override { return CanvasType::API; };

  void update() override;

  void draw(LEDController * controller) override;

  void resetSubCanvasOffset() override;

  void setAPI(String name, String url, long refresh, bool active, char * parseRules);

  void resetAPI();

  void toggleAPI();

  APIData getAPIData();

  void updateAPI(unsigned long curtime, bool force = false);
};

#endif