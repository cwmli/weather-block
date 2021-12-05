#ifndef WBAPI_H
#define WBAPI_H

#include "config.h"

class WeatherBlockAP;

class WeatherBlockAPI {

  WeatherBlockAP * ap;

public:
  WeatherBlockAPI(WeatherBlockAP * ap) : ap(ap) {};

  void getDefault();

  // WiFi setup routes
  void postWiFi();
  void getPollWiFi();
  void postDisconnectWiFi();

  void getWiFiScan();
  void getPollWiFiScan();

  // General controls
  void getWBDetails();
  void postSetNTPOffset();

  // LED controller
  void postSetBrightness();

  // Canvas control routes
  void getAllCanvasInfo();
  void getCanvasInfo();
  void postSetActiveCanvas();
  void postUpdateCanvas();
  void postResetCanvas();
  void postToggleCanvas();
  void postSetCanvas();
};

#endif
