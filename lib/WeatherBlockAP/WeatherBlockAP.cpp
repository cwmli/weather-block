#include "WeatherBlockAP.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
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

  server.onNotFound([]() {
    if (!RouteHandlers::getDefault(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });

  // addAPI("DarkSky", 
  //        "https://api.darksky.net/forecast/API_KEY/43.5799475,-79.6614369?units=ca&exclude=minutely,hourly,daily,alerts,flags", 
  //        60 * 60,
  //        std::map<String, std::array<int, 4>> {
  //          {"temperature", std::array<int, 4>{5, 1, 1, 1}},
  //        });
  // addAPI("OpenWeatherMap", 
  //        "https://api.openweathermap.org/data/2.5/weather?q=mississauga&APPID=07b1e3b39857252f4324e71d9fad58ff&units=metric", 
  //        60 * 60,
  //        std::map<String, int> {
  //           {"temp", 5},
  //        });
  
  server.begin();
  Serial.println("HTTP server started");

  controller.init();
}

void WeatherBlockAP::update() {
  server.handleClient();
  MDNS.update();
  controller.update();

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

void WeatherBlockAP::addAPI(String name, String url, long refresh, std::map<String, std::array<int, 4>> parseRules) {
  APIList[APICount] = APIData(name, url, refresh);
  APIList[APICount].parseRules = parseRules;
  APICount++;
}

void WeatherBlockAP::removeAPI(uint8_t index) {
  for (uint8_t i = index; i < API_LIMIT - 1; ++i) {
    APIList[i] = APIList[i + 1];
  }
  APICount--;
}

APIData * WeatherBlockAP::getAPIList() {
  return APIList;
}

void WeatherBlockAP::tryUpdateAPI() {
  for (uint8_t i = 0; i < APICount; ++i) {
    if (!APIList[i].isActive || 
      timeClient.getEpochTime() - APIList[i].lastRefreshed < APIList[i].refreshTime) {
      continue;
    }

    BearSSL::WiFiClientSecure bsslClient;
    bsslClient.setInsecure();

    HTTPClient https;

    Serial.printf("Updating %s\n", APIList[i].name.c_str());
    https.begin(bsslClient, APIList[i].url);

    int httpcode = https.GET();

    if (httpcode > 0) {
      Serial.printf("[HTTPSRequest] code: %d\n", httpcode);
      String payload = https.getString();
      Serial.printf("[HTTPSRequest] payload: \n%s\n", payload.c_str());
      // Have to do some ghetto stuff
      controller.resetTextElements();
      std::map<String, std::array<int, 4>> rules = APIList[i].parseRules;
      for (auto it = rules.begin(); it != rules.end(); it++) {
        int pos = payload.indexOf(it->first);
        String temp = payload.substring(pos + it->first.length() + 2, pos + it->first.length() + 2 + it->second[0]);
        Serial.printf("[INFO] %s: %s\n", it->first.c_str(), temp.c_str());
        if (it->second[3]) {
          int itemp = temp.toInt();
          APIList[i].data[it->first] = itemp;
        } else {
          APIList[i].data[it->first] = temp;
        }
      }
    } else {
      Serial.printf("An error occurred while updating %s\n", APIList[i].name.c_str());
    }
    APIList[i].lastRefreshed = timeClient.getEpochTime();
    https.end();
  }
}
