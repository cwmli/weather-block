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
  WiFi.mode(WIFI_AP_STA);
  delay(1000);
  WiFi.softAP(WB_SSID, WB_PWD, 11);

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

  server.on("/setwifi", HTTP_POST, [&](){
    RouteHandlers::postWiFi();
    currentState = IS_CONNECTING;
  });
  server.on("/wifistatus", HTTP_GET, RouteHandlers::getPollWiFi);
  server.on("/wifidisconnect", HTTP_POST, [&](){ 
    RouteHandlers::postDisconnectWiFi();
    currentState = IS_IDLING;
  });

  server.on("/networkscan", HTTP_GET, RouteHandlers::getWiFiScan);
  server.on("/networkstatus", HTTP_GET, RouteHandlers::getPollWiFiScan);

  server.on("/canvasinfoall", HTTP_GET, [&](){ RouteHandlers::getAllCanvasInfo(canvas); });
  server.on("/canvasinfo", HTTP_GET, [&](){ RouteHandlers::getCanvasInfo(canvas); });
  server.on("/canvasreset", HTTP_POST, [&](){ RouteHandlers::postResetCanvas(canvas); });
  server.on("/canvastoggle", HTTP_POST, [&](){ RouteHandlers::postToggleCanvas(canvas); });
  server.on("/canvasset", HTTP_POST, [&](){ RouteHandlers::postSetCanvas(canvas); });
  server.on("/canvasfupdate", HTTP_POST, [&](){
    if (!server.hasArg("index") || server.arg("index") == NULL) {
      Serial.println("Invalid index for API");
      server.send(400, "text/plain", "400: Invalid Request");
    } else {
      int i = server.arg("index").toInt();
      Serial.printf("Forcing update for API: %s\n", canvas[i].getAPIData().name.c_str());
      canvas[i].updateAPI(timeClient.getEpochTime(), true);
      server.send(200, "text/plain", "200: Force updated API");
    }
  });

  server.on("/wbdetails", HTTP_GET, [&]() {
    String ssid = WiFi.SSID();
    String ipaddr = WiFi.localIP().toString();
    String subnet = WiFi.subnetMask().toString();
    String gateway = WiFi.gatewayIP().toString();

    String jsonString = "{\"ssid\": \"";
    jsonString += ssid;
    jsonString += "\", \"ipaddr\": \"";
    jsonString += ipaddr;
    jsonString += "\", \"subnet\": \"";
    jsonString += subnet;
    jsonString += "\", \"gateway\": \"";
    jsonString += gateway;
    jsonString += "\", \"brightness\": \"";
    jsonString += controller.getBrightness();
    jsonString += "\", \"activecanvas\": \"";
    jsonString += activeCanvas;
    jsonString += "\"}";

    server.send(200, "text/plain", jsonString);
  });
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
         "currentDateTime 1 1 ffffff 3");
  canvas[1].setAPI("DarkSky", 
         "https://api.darksky.net/forecast/cd243cdb3889f8ada3fa607612e1ae47/43.5799475,-79.6614369?units=ca&exclude=minutely,hourly,daily,alerts,flags", 
         60 * 60,
         true,
         "temperature 11 1 ffffff 0, icon 0 0 ffffff 1");
  canvas[1].addElement(new Elements::Text("c", 19, 1, false, 0));
  
  server.begin();
  Serial.println("HTTP server started");

  controller.init();
}

void WeatherBlockAP::update() {
  server.handleClient();
  MDNS.update();

  byte wStatus = WiFi.status();
  if (wStatus == WL_NO_SSID_AVAIL || wStatus == WL_CONNECT_FAILED) {
    Serial.println("WiFi disconnected: SSID unavailable or password is incorrect, entering SoftAP mode");
    RouteHandlers::postDisconnectWiFi();
    currentState = IS_IDLING;
  }

  if (currentState == IS_IDLING) {
    // This is the startup state, waiting to be setup with a network
    controller.idle();
    controller.update();
  } else if (currentState == IS_CONNECTING) {
    controller.busy();
    controller.update();
  } else {
    canvas[activeCanvas].update();
    canvas[activeCanvas].draw(&controller);
    controller.update();
    controller.reset();
  }

  /* Break off point, do not proceed if we don't have wifi connection */
  if (wStatus != WL_CONNECTED) {
    return;
  } else {
    currentState = IS_READY;
  }

  if (!isTimeclientRunning) {
    Serial.printf("WiFi connected: local ip is %s\n",  WiFi.localIP().toString().c_str());
    Serial.println("starting timeclient...");
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
  Serial.println("[INFO] Increasing brightness");
}

void WeatherBlockAP::decrementBrightness() {
  controller.decrementBrightness();
  Serial.println("[INFO] decreasing brightness");
}

void WeatherBlockAP::incrementActiveCanvas() {
  canvas[activeCanvas].resetSubCanvasOffset();
  activeCanvas = min(activeCanvas + 1, API_LIMIT - 1);
  Serial.printf("[INFO] Setting activeCanvas to: %d\n", activeCanvas);
}

void WeatherBlockAP::decrementActiveCanvas() {
  canvas[activeCanvas].resetSubCanvasOffset();
  activeCanvas = max(activeCanvas - 1, 0);
  Serial.printf("[INFO] Setting activeCanvas to: %d\n", activeCanvas);
}

void WeatherBlockAP::incrementSubCanvas() {
  canvas[activeCanvas].incrementSubCanvasOffset();
}