#include "WeatherBlockAP.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <FS.h>

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

  server.onNotFound([]() {
    if (!RouteHandlers::getDefault(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });

  addAPI("OpenWeatherMap", "http://api.openweathermap.org/data/2.5/weather?q=mississauga&APPID=c93b4a667c8c9d1d1eb941621f899bb8&units=metric", 60);
  
  server.begin();
  Serial.println("HTTP server started");
}

void WeatherBlockAP::update() {
  server.handleClient();
  MDNS.update();

  isConnected = WiFi.status() == WL_CONNECTED;

  if (!isConnected) {
    return;
  }

  if (!isTimeclientRunning) {
    Serial.println("WiFI connected: starting timeclient");
    timeClient.begin();
    isTimeclientRunning = true;
  }

  tryUpdateAPI();
}

void WeatherBlockAP::addAPI(String name, String url, long refresh) {
  APIList[APICount] = APIData(name, url, refresh);
  APICount++;
}

void WeatherBlockAP::removeAPI(uint8_t index) {
  for (uint8_t i = index; i < API_LIMIT - 1; ++i) {
    APIList[i] = APIList[i + 1];
  }
  APICount--;
}

void WeatherBlockAP::tryUpdateAPI() {
  for (uint8_t i = 0; i < APICount; ++i) {
    if (timeClient.getEpochTime() - APIList[i].lastRefreshed < APIList[i].refreshTime) {
      continue;
    }

    HTTPClient http;

    Serial.print("Updating ");
    Serial.println(APIList[i].name);
    http.begin(APIList[i].url);

    int httpcode = http.GET();

    if (httpcode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("An error occurred while updating ");
      Serial.print(APIList[i].name);
    }
    APIList[i].lastRefreshed = timeClient.getEpochTime();
    http.end();
  }
}
