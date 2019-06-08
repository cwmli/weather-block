#ifndef ROUTE_HANDLERS_H
#define ROUTE_HANDLERS_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

namespace RouteHandlers {

  bool getDefault(String path);

  // WiFi setup routes
  void postWiFi();
  void getPollWiFi();
  void getWiFiDetails();
  void postDisconnectWiFi();

  // API control routes
}

#endif
