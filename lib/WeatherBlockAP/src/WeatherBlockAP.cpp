#include "WeatherBlockAP.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <TimeLib.h>
#include <map>

#include "config.h"

WiFiUDP ntpUDP;
NTPClient timeClient = NTPClient(ntpUDP, NTP_POOL_URL, NTP_DEFAULT_TIMEOFFSET, NTP_DEFAULT_UPDATEINT);

time_t ntpProvider() {
  return timeClient.getEpochTime();
}

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

  server->on("/setwifi", HTTP_POST, [&](){ wbAPI.postWiFi(); });
  server->on("/wifistatus", HTTP_GET, [&](){ wbAPI.getPollWiFi(); });
  server->on("/wifidisconnect", HTTP_POST, [&](){ wbAPI.postDisconnectWiFi(); });

  server->on("/networkscan", HTTP_GET, [&](){ wbAPI.getWiFiScan(); });
  server->on("/networkstatus", HTTP_GET, [&](){ wbAPI.getPollWiFiScan(); });

  server->on("/canvasinfoall", HTTP_GET, [&](){ wbAPI.getAllCanvasInfo(); });
  server->on("/canvasinfo", HTTP_GET, [&](){ wbAPI.getCanvasInfo(); });
  server->on("/canvasreset", HTTP_POST, [&](){ wbAPI.postResetCanvas(); });
  server->on("/canvastoggle", HTTP_POST, [&](){ wbAPI.postToggleCanvas(); });
  server->on("/canvasset", HTTP_POST, [&](){ wbAPI.postSetCanvas(); });
  server->on("/canvasfupdate", HTTP_POST, [&](){ wbAPI.postUpdateCanvas(); });

  server->on("/wbdetails", HTTP_GET, [&]() { wbAPI.getWBDetails(); });

  server->on("/setbrightness", HTTP_POST, [&](){ wbAPI.postSetBrightness(); });
  server->on("/setactivecanvas", HTTP_POST, [&](){ wbAPI.postSetActiveCanvas(); });

  server->onNotFound([&]() { wbAPI.getDefault(); });

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
  
  server->begin();
  Serial.println("HTTP server started");

  // clock set up
  setSyncProvider(ntpProvider);
  setSyncInterval(3600);

  controller.init();
}

void WeatherBlockAP::update() {
  server->handleClient();
  MDNS.update();

  byte wStatus = WiFi.status();
  if (wStatus == WL_NO_SSID_AVAIL || wStatus == WL_CONNECT_FAILED) {
    Serial.println("WiFi disconnected: SSID unavailable or password is incorrect, entering SoftAP mode");
    wbAPI.postDisconnectWiFi();
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
    if (isTimeclientRunning) {
      timeClient.end();
      isTimeclientRunning = false;
    }

    return;
  } else {
    currentState = IS_READY;

    Serial.printf("WiFi connected: local ip is %s\n",  WiFi.localIP().toString().c_str());
    Serial.println("starting timeclient...");
    timeClient.begin();
    isTimeclientRunning = true;

    if (timeStatus() == timeNotSet) {
      setTime(timeClient.getEpochTime());
    }
  }

  timeClient.update();

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