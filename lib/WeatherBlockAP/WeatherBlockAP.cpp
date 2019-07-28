#include "WeatherBlockAP.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <FS.h>
#include <map>

#include "RouteHandlers.h"
#include "config.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_POOL_URL, NTP_DEFAULT_TIMEOFFSET, NTP_DEFAULT_UPDATEINT);

WeatherBlockAP::WeatherBlockAP() {}

void WeatherBlockAP::init() {
  delay(1000);
  Serial.begin(115200);
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

  server.on("/apiinfoall", HTTP_GET, [&](){ RouteHandlers::getAllAPIInfo(canvas); });
  server.on("/apiinfo", HTTP_GET, [&](){ RouteHandlers::getAPIInfo(canvas); });
  server.on("/apireset", HTTP_POST, [&](){ RouteHandlers::postRemoveAPI(canvas); });
  server.on("/apitoggle", HTTP_POST, [&](){ RouteHandlers::postToggleAPI(canvas); });
  server.on("/apiset", HTTP_POST, [&](){ RouteHandlers::postSetAPI(canvas); });

  server.on("/setbrightness", HTTP_POST, [&](){ 
    if (!server.hasArg("val") || server.arg("val") == NULL) {
      Serial.println("Invalid value for brightness");
      server.send(400, "text/plain", "400: Invalid Request");
    } else {
      int i = server.arg("val").toInt();
      Serial.printf("Setting brightness: %d\n", i);
      controller.setBrightness(i);
      server.send(200, "text/plain", "200: Toggled active status for API");
    }
  });
  server.on("/setactivecanvas", HTTP_POST, [&](){ 
    if (!server.hasArg("val") || server.arg("val") == NULL) {
      Serial.println("Invalid value for active canvas");
      server.send(400, "text/plain", "400: Invalid Request");
    } else {
      int i = server.arg("val").toInt();
      Serial.printf("Setting active canvas: %d\n", i);
      activeCanvas = i;
      server.send(200, "text/plain", "200: Toggled active status for API");
    }
  });

  server.onNotFound([]() {
    if (!RouteHandlers::getDefault(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });

  // --- TEST SETUP API ---
  canvas[0].setAPI("WorldclockAPI", 
         "https://worldclockapi.com/api/json/est/now", 
         30,
         true,
         std::map<String, APIParseRule> {
           {"currentDateTime", {1, 1, APIValueType::TIME}},
         });
  canvas[1].setAPI("DarkSky", 
         "https://api.darksky.net/forecast/cd243cdb3889f8ada3fa607612e1ae47/43.5799475,-79.6614369?units=ca&exclude=minutely,hourly,daily,alerts,flags", 
         60 * 60,
         true,
         std::map<String, APIParseRule> {
           {"temperature", {11, 1, APIValueType::NUMBER}},
           {"icon", {0, 0, APIValueType::ICON}}
         });
  canvas[1].addElement(new Elements::Text("c", 19, 1, false, 0));
  
  server.begin();
  Serial.println("HTTP server started");

  controller.init();
}

void WeatherBlockAP::update() {
  server.handleClient();
  MDNS.update();

  canvas[activeCanvas].update();
  canvas[activeCanvas].draw(&controller);
  controller.update();
  controller.reset();

  isConnected = WiFi.status() == WL_CONNECTED;

  if (!isConnected) {
    return;
  }

  if (!isTimeclientRunning) {
    Serial.println("WiFI connected: starting timeclient");
    timeClient.begin();
    isTimeclientRunning = true;
  }

  if (isTimeclientRunning) {
    timeClient.update();
  }

  /* Update Canvas API Data */
  for (uint8_t i = 0; i < API_LIMIT; ++i) {
    canvas[i].updateAPI(timeClient.getEpochTime());
  }

}

void WeatherBlockAP::incrementBrightness() {
  controller.incrementBrightness();
}

void WeatherBlockAP::decrementBrightness() {
  controller.decrementBrightness();
}

void WeatherBlockAP::incrementActiveCanvas() {
  activeCanvas = min(activeCanvas + 1, API_LIMIT - 1);
}

void WeatherBlockAP::decrementActiveCanvas() {
  activeCanvas = max(activeCanvas - 1, 0);
}