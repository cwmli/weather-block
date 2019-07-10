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
  void getWiFiDetails();
  void postDisconnectWiFi();

  // API control routes
  void getAllAPIInfo(Canvas * canvases);
  void getAPIInfo(Canvas * canvases);
  void postRemoveAPI(Canvas * canvases);
  void postToggleAPI(Canvas * canvases);
}

#endif
