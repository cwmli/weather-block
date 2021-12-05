#include "WeatherBlockAP.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <TimeLib.h>
#include <map>

#include "APICanvas.h"
#include "ClockCanvas.h"
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
  server->on("/setntpoffset", HTTP_POST, [&](){ wbAPI.postSetNTPOffset(); });

  server->on("/setbrightness", HTTP_POST, [&](){ wbAPI.postSetBrightness(); });
  server->on("/setactivecanvas", HTTP_POST, [&](){ wbAPI.postSetActiveCanvas(); });

  server->onNotFound([&]() { wbAPI.getDefault(); });

  // --- TEST SETUP API ---
  canvas[0] = new ClockCanvas();
  canvas[1] = new APICanvas();
  canvas[2] = new APICanvas();
  
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
    canvas[activeCanvas]->update();
    canvas[activeCanvas]->draw(&controller);
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
  } else if (currentState != IS_READY) {
    currentState = IS_READY;

    Serial.printf("WiFi connected: local ip is %s\n",  WiFi.localIP().toString().c_str());
    Serial.println("Starting timeclient...");
    timeClient.begin();
    isTimeclientRunning = true;

    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    setTime(timeClient.getEpochTime());
  }

  /* Update Canvas API Data */
  for (uint8_t i = 0; i < CANVAS_LIMIT; ++i) {
    if (canvas[i]->type() == CanvasType::API) {
      ((APICanvas *)canvas[i])->updateAPI(timeClient.getEpochTime());
    }
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
  canvas[activeCanvas]->resetSubCanvasOffset();
  activeCanvas = min(activeCanvas + 1, CANVAS_LIMIT - 1);
  Serial.printf("[INFO] Setting activeCanvas to: %d\n", activeCanvas);
}

void WeatherBlockAP::decrementActiveCanvas() {
  canvas[activeCanvas]->resetSubCanvasOffset();
  activeCanvas = max(activeCanvas - 1, 0);
  Serial.printf("[INFO] Setting activeCanvas to: %d\n", activeCanvas);
}

void WeatherBlockAP::incrementSubCanvas() {
  canvas[activeCanvas]->incrementSubCanvasOffset();
}