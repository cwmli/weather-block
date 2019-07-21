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

  server.onNotFound([]() {
    if (!RouteHandlers::getDefault(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });

  // --- TEST SETUP API ---
  canvas[0].setAPI("WorldclockAPI", 
         "https://worldclockapi.com/api/json/est/now", 
         60 * 60,
         true,
         std::map<String, APIParseRule> {
           {"currentDateTime", {1, 1, APIValueType::TIME}},
         });
  // canvas[0].setAPI("DarkSky", 
  //        "https://api.darksky.net/forecast/cd243cdb3889f8ada3fa607612e1ae47/43.5799475,-79.6614369?units=ca&exclude=minutely,hourly,daily,alerts,flags", 
  //        60 * 60,
  //        true,
  //        std::map<String, APIParseRule> {
  //          {"temperature", {11, 1, APIValueType::NUMBER}},
  //          {"icon", {0, 0, APIValueType::ICON}}
  //        });
  // canvas[0].addElement(new Elements::Text("c", 19, 1, false, 0));
  
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
