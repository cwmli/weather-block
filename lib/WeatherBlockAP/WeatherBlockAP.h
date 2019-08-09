#ifndef WEATHERBLOCKAP_H
#define WEATHERBLOCKAP_H

#include <Arduino.h>
#include <LEDController.h>
#include <Canvas.h>

#include "config.h"

class WeatherBlockAP {
  bool isTimeclientRunning = false;
  bool isConnected = false;
  bool isBusy = true;

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
