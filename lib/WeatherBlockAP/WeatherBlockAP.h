#ifndef WEATHERBLOCKAP_H
#define WEATHERBLOCKAP_H

#define IS_IDLING     0
#define IS_CONNECTING 1
#define IS_READY      2

#include <Arduino.h>
#include <LEDController.h>
#include <Canvas.h>

#include "config.h"

class WeatherBlockAP {
  bool isTimeclientRunning = false;

  uint8_t currentState = IS_IDLING;
  long stateTimeout;

  LEDController controller;
  uint8_t activeCanvas = 0;
  Canvas canvas[API_LIMIT];

public:

  WeatherBlockAP();

  void init();

  void update();

  void incrementBrightness();

  void decrementBrightness();

  void incrementActiveCanvas();

  void decrementActiveCanvas();

  void incrementSubCanvas();
};

#endif
