#include "Canvas.h"

#include "Icons.h"

void Canvas::update() {
  for (auto i = elements.begin(); i != elements.end(); i++) {
    (*i)->xOffset = -subCanvasOffset;
  }
}

void Canvas::draw(LEDController * controller) {
  for (auto i = elements.begin(); i != elements.end(); i++) {
    (*i)->draw(controller);
  }
}

void Canvas::incrementSubCanvasOffset() {
  subCanvasOffset += SUBCANVAS_OFFSET;
  if (subCanvasOffset > SUBCANVAS_OFFSET_LIMIT) {
    subCanvasOffset = 0;
  }
  Serial.printf("[INFO] Setting subCanvasOffset to: %d\n", subCanvasOffset);
}

void Canvas::resetSubCanvasOffset() {
  subCanvasOffset = 0;

  for (auto i = elements.begin(); i != elements.end(); i++) {
    (*i)->xOffset = subCanvasOffset;
  }
}

void Canvas::addElement(Elements::Text * e) {
  elements.push_back(e);
}

void Canvas::resetElements() {
  for (auto i = elements.begin(); i != elements.end(); i++) {
    delete *i;
  }
  elements.clear();
}

std::vector<Elements::Text *> Canvas::getElements() {
  return elements;
}

String Canvas::getElementsString() {
  String res;
  for (auto it = elements.begin(); it != elements.end(); it++) {
    unsigned long hex = ((*it)->color.r << 16) | ((*it)->color.g << 8) | (*it)->color.b;
    res += (*it)->string + " " + String(hex, HEX) + " " + (*it)->x + " " + (*it)->y + " " + (*it)->scroll + " " + (*it)->scrollSpeed;
    if (next(it) != elements.end()) {
      res += ",";
    }
  }

  return res;
}


void Canvas::setElements(char * content) {
  resetElements();

  char * endelementset;
  char * elementset = strtok_r(content, ",", &endelementset);
  while(elementset != NULL) {
    char * values[6];
    byte index = 0; 
    char * endelemopt;
    char * elemopt = strtok_r(elementset, " ", &endelemopt);
    while (elemopt != NULL) {
      values[index++] = elemopt;
      elemopt = strtok_r(NULL, " ", &endelemopt);
    }
    
    addElement(
      new Elements::Text(
        values[0],
        CRGB(strtoul(values[1], NULL, HEX)),
        atoi(values[2]),
        atoi(values[3]),
        (atoi(values[4]) > 0),
        atoi(values[5])
      )
    );

    elementset = strtok_r(NULL, ",", &endelementset);
  }
}

