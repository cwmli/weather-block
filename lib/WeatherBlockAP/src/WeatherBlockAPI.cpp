#include "WeatherBlockAPI.h"

#include <ESP8266WiFi.h>
#include <FS.h>
#include <ctime>

#include "APIData.h"
#include "APICanvas.h"
#include "WeatherBlockAP.h"
#include "URLDecode.h"

#include "config.h"

// ContentType helper
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

void WeatherBlockAPI::getDefault() {
  String path = ap->server->uri();

  if (path.endsWith("/")) {
    if (WiFi.status() == WL_CONNECTED) {
      path += "main.html";
    } else {
      path += "index.html";
    }
  }

  String targetpath = WWW_DIR;
  targetpath += path;
  
  String contentType = getContentType(path);
  Serial.println("\tLooking for: " + targetpath);
  if (SPIFFS.exists(targetpath)) {
    File file = SPIFFS.open(targetpath, "r");
    size_t sent = ap->server->streamFile(file, contentType);
    file.close();
    return;
  }
  Serial.println("\tFile Not Found");
  ap->server->send(404, "text/plain", "404: Not Found");
}

void WeatherBlockAPI::postWiFi() {
  if (!ap->server->hasArg("ssid") || !ap->server->hasArg("password")
      || ap->server->arg("ssid") == NULL || ap->server->arg("password") == NULL) {
    Serial.println("Invalid set network connection request");
    ap->server->send(400, "text/plain", "400: Invalid Request"); 
  } else {
    Serial.println("Connecting to network");
    ap->server->send(200, "text/plain", "200: Set WiFi SSID and PW");
    WiFi.begin(ap->server->arg("ssid"), ap->server->arg("password"));
    ap->currentState = IS_CONNECTING;
  }
}

void WeatherBlockAPI::getPollWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected");
    ap->server->send(200, "text/plain", "false");
  } else {
    Serial.println("Connected");
    ap->server->send(200, "text/plain", "true");
  }
}

void WeatherBlockAPI::postDisconnectWiFi() {
  ap->server->send(200, "text/plain", "true");
  WiFi.disconnect();
  Serial.println("Disconnected");
  ap->currentState = IS_IDLING;
}

void WeatherBlockAPI::getWiFiScan() {
  WiFi.scanDelete();
  delay(1000);
  WiFi.scanNetworks(true);
  ap->server->send(200, "text/plain", "200: Starting network scan");
}

void WeatherBlockAPI::getPollWiFiScan() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED || n == WIFI_SCAN_RUNNING) {
    ap->server->send(200, "text/plain", "'{\"scanComplete\": \"false\"}'");
  } else {
    String obj = "[";
    for (int i = 0; i < n; i++) {
      String jsonString = i > 0 ? "," : "";
      jsonString += "{\"ssid\": \"";
      jsonString += WiFi.SSID(i);
      jsonString += "\", \"rssi\": \"";
      jsonString += WiFi.RSSI(i);
      jsonString += "\"}";

      obj += jsonString;
    }
    obj += "]";
    ap->server->send(200, "text/plain", "{\"scanComplete\": \"true\", \"data\": " + obj + "}");
  }
}

void WeatherBlockAPI::getWBDetails() {
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
  jsonString += ap->controller.getBrightness();
  jsonString += "\", \"activecanvas\": \"";
  jsonString += ap->activeCanvas;
  jsonString += "\", \"ntpoffset\": \"";
  jsonString += ap->timeClientOffset;
  jsonString += "\"}";

  ap->server->send(200, "text/plain", jsonString);
}

void WeatherBlockAPI::postSetNTPOffset() {
  if (!ap->server->hasArg("offset") || ap->server->arg("offset") == NULL) {
    Serial.println("Invalid value for NTP offset");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("offset").toInt();
    ap->timeClientOffset = i;
    timeClient.setTimeOffset(i);
    timeClient.forceUpdate();
    ap->server->send(200, "text/plain", "200: Set NTP offset for clock");
  }
}

void WeatherBlockAPI::postSetBrightness() {
  if (!ap->server->hasArg("val") || ap->server->arg("val") == NULL) {
    Serial.println("Invalid value for brightness");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("val").toInt();
    Serial.printf("Setting brightness: %d\n", i);
    ap->controller.setBrightness(i);
    ap->server->send(200, "text/plain", "200: Toggled active status for API");
  }
}

void WeatherBlockAPI::getAllCanvasInfo() {
  String obj = "[";
  for (uint8_t i = 0; i < CANVAS_LIMIT; i++) {

    if (ap->canvas[i]->type() == CanvasType::API) {
      APICanvas * canvas = (APICanvas *)(ap->canvas[i]);
      APIData data = canvas->getAPIData();

      char timestr[32];
      std::time_t secsSinceEpoch = (long) data.lastRefreshed;
      struct std::tm ts;
      ts = *std::localtime(&secsSinceEpoch);
      std::strftime(timestr, sizeof(timestr), "%a %b %e, %R", &ts);

      String jsonString = obj.startsWith("[{") ? "," : "";
      jsonString += "{\"name\": \"";
      jsonString += data.name;
      jsonString += "\", \"index\": \"";
      jsonString += i;
      jsonString += "\", \"url\": \"";
      jsonString += data.url;
      jsonString += "\", \"isActive\": \"";
      jsonString += data.isActive;
      jsonString += "\", \"refreshes\": \"";
      jsonString += data.refreshTime / 60; //in minutes
      jsonString += "\", \"lastRefresh\": \"";
      jsonString += timestr;
      jsonString += "\"}";

      obj += jsonString;
    }
  }
  obj += "]";
  ap->server->send(200, "text/plain", obj);
}

void WeatherBlockAPI::getCanvasInfo() {
  if (!ap->server->hasArg("index") || ap->server->arg("index") == NULL) {
    Serial.println("Invalid index for API");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int index = ap->server->arg("index").toInt();

    if (ap->canvas[index]->type() != CanvasType::API) {
      ap->server->send(400, "text/plain", "400: Canvas is not API type");
      return;
    }

    APICanvas * canvas = (APICanvas *)(ap->canvas[index]);
    APIData data = canvas->getAPIData();

    char timestr[32];
    std::time_t secsSinceEpoch = (long) data.lastRefreshed;
    struct std::tm ts;
    ts = *std::localtime(&secsSinceEpoch);
    std::strftime(timestr, sizeof(timestr), "%a %b %e, %R", &ts);

    String jsonString;
    jsonString += "{\"name\": \"";
    jsonString += data.name;
    jsonString += "\", \"url\": \"";
    jsonString += data.url;
    jsonString += "\", \"isActive\": \"";
    jsonString += data.isActive;
    jsonString += "\", \"refreshes\": \"";
    jsonString += data.refreshTime; //in minutes
    jsonString += "\", \"lastRefresh\": \"";
    jsonString += timestr;
    jsonString += "\", \"parseRules\": \"";
    jsonString += data.parseRulesString();
    jsonString += "\", \"additionalElements\": \"";
    jsonString += canvas->getElementsString();
    jsonString += "\"}";

    ap->server->send(200, "text/plain", jsonString);
  }
}

void WeatherBlockAPI::postSetActiveCanvas() {
  if (!ap->server->hasArg("val") || ap->server->arg("val") == NULL) {
    Serial.println("Invalid value for active canvas");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("val").toInt();
    Serial.printf("Setting active canvas: %d\n", i);
    ap->activeCanvas = i;
    ap->server->send(200, "text/plain", "200: Toggled active status for API");
  }
}

void WeatherBlockAPI::postUpdateCanvas() {
  if (!ap->server->hasArg("index") || ap->server->arg("index") == NULL) {
    Serial.println("Invalid index for API");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("index").toInt();

    if (ap->canvas[i]->type() != CanvasType::API) {
      ap->server->send(400, "text/plain", "400: Canvas is not API type");
      return;
    }

    APICanvas * canvas = (APICanvas *)(ap->canvas[i]);

    Serial.printf("Forcing update for API: %s\n", canvas->getAPIData().name.c_str());
    canvas->updateAPI(timeClient.getEpochTime(), true);
    ap->server->send(200, "text/plain", "200: Force updated API");
  }
}

void WeatherBlockAPI::postResetCanvas() {
  if (!ap->server->hasArg("index") || ap->server->arg("index") == NULL) {
    Serial.println("Invalid index for API");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("index").toInt();

    if (ap->canvas[i]->type() != CanvasType::API) {
      ap->server->send(400, "text/plain", "400: Canvas is not API type");
      return;
    }

    APICanvas * canvas = (APICanvas *)(ap->canvas[i]);

    Serial.printf("Removing API and Elements: %s\n", canvas->getAPIData().name.c_str());
    canvas->resetAPI();
    canvas->resetElements();
    ap->server->send(200, "text/plain", "200: Removed API");
  }
}

void WeatherBlockAPI::postToggleCanvas() {
  if (!ap->server->hasArg("index") || ap->server->arg("index") == NULL) {
    Serial.println("Invalid index for API");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("index").toInt();

    if (ap->canvas[i]->type() != CanvasType::API) {
      ap->server->send(400, "text/plain", "400: Canvas is not API type");
      return;
    }

    APICanvas * canvas = (APICanvas *)(ap->canvas[i]);

    Serial.printf("Toggling active status for API: %s\n", canvas->getAPIData().name.c_str());
    canvas->toggleAPI();
    ap->server->send(200, "text/plain", "200: Toggled active status for API");
  }
}

void WeatherBlockAPI::postSetCanvas() {
  if (!ap->server->hasArg("index") || ap->server->arg("index") == NULL) {
    Serial.println("Invalid index for API");
    ap->server->send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = ap->server->arg("index").toInt();

    if (ap->canvas[i]->type() != CanvasType::API) {
      ap->server->send(400, "text/plain", "400: Canvas is not API type");
      return;
    }

    APICanvas * canvas = (APICanvas *)(ap->canvas[i]);

    Serial.printf("Setting data for API: %s\n", canvas->getAPIData().name.c_str());

    char allrules[500]; 
    urldecode(ap->server->arg("parserules")).toCharArray(allrules, 500);

    canvas->setAPI(
      urldecode(ap->server->arg("name")),
      urldecode(ap->server->arg("url")),
      (long) ap->server->arg("refresh").toInt(),
      ap->server->arg("active") == "true",
      allrules
    );

    char elementset[500]; 
    urldecode(ap->server->arg("additionalelements")).toCharArray(elementset, 500);
    canvas->setElements(elementset);

    ap->server->send(200, "text/plain", "200: Set data for API");
  }
}