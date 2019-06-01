#include "SoftAPSetup.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include "RouteHandlers.h"
#include "config.h"

void SoftAPSetup::init() {
  delay(1000);
  Serial.begin(115200);
  WiFi.disconnect(true);
  delay(1000);
  WiFi.softAP(WB_SSID, WB_PWD);

  IPAddress wbIP = WiFi.softAPIP();
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(wbIP);

  if (MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.print("MDNS started: ");
    Serial.print(MDNS_NAME);
    Serial.println(".local");
  }
  
  SPIFFS.begin();

  server.on("/setwifi", HTTP_POST, RouteHandlers::postWiFi);
  server.on("/wifistatus", HTTP_GET, RouteHandlers::getPollWiFi);
  server.on("/wifidetails", HTTP_GET, RouteHandlers::getWiFiDetails);
  server.on("/wifidisconnect", HTTP_POST, RouteHandlers::postDisconnectWiFi);

  server.onNotFound([]() {
    if (!RouteHandlers::getDefault(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void SoftAPSetup::update() {
  server.handleClient();
  MDNS.update();
}
