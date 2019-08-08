#ifndef ROUTE_HANDLERS_H
#define ROUTE_HANDLERS_H

#include <ESP8266WebServer.h>

#include "Canvas.h"
#include "config.h"

extern ESP8266WebServer server;

namespace RouteHandlers {

  bool getDefault(String path);

  // WiFi setup routes
  void postWiFi();
  void getPollWiFi();
  void postDisconnectWiFi();

  void getWiFiScan();
  void getPollWiFiScan();

  // Canvas control routes
  void getAllCanvasInfo(Canvas * canvases);
  void getCanvasInfo(Canvas * canvases);
  void postResetCanvas(Canvas * canvases);
  void postToggleCanvas(Canvas * canvases);
  void postSetCanvas(Canvas * canvases);
}

#endif
