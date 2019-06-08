#ifndef WEATHERBLOCKAP_H
#define WEATHERBLOCKAP_H

#include <Arduino.h>

#include "config.h"
#include "APIData.h"

class WeatherBlockAP {
  bool isTimeclientRunning = false;
  bool isConnected = false;

  uint8_t APICount = 0;
  APIData APIList [API_LIMIT];

public:

  WeatherBlockAP();

  void init();

  void update();

  void addAPI(String name, String url, long refresh);

  void removeAPI(uint8_t index);

  void tryUpdateAPI();
};

#endif
