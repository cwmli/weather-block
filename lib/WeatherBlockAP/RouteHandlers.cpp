#include "RouteHandlers.h"

#include <ESP8266WiFi.h>
#include <FS.h>
#include <ctime>

#include "APIData.h"
#include "URLDecode.h"

#include "config.h"

ESP8266WebServer server(80);

// ContentType helper
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool RouteHandlers::getDefault(String path) {
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
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;
}

void RouteHandlers::postWiFi() {
  if (!server.hasArg("ssid") || !server.hasArg("password")
      || server.arg("ssid") == NULL || server.arg("password") == NULL) {
    Serial.println("Invalid set network connection request");
    server.send(400, "text/plain", "400: Invalid Request"); 
  } else {
    Serial.println("Connecting to network");
    server.send(200, "text/plain", "200: Set WiFi SSID and PW");
    WiFi.begin(server.arg("ssid"), server.arg("password"));
  }
}

void RouteHandlers::getPollWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Not connected");
    server.send(200, "text/plain", "false");
  } else {
    Serial.println("Connected");
    server.send(200, "text/plain", "true");
  }
}

void RouteHandlers::postDisconnectWiFi() {
  server.send(200, "text/plain", "true");
  WiFi.disconnect(true);
  Serial.println("Disconnected");
  delay(1000);
  Serial.println("Starting SoftAP");
  WiFi.softAP(WB_SSID, WB_PWD);
}

void RouteHandlers::getWiFiScan() {
  WiFi.scanDelete();
  delay(1000);
  WiFi.scanNetworks(true);
  server.send(200, "text/plain", "200: Starting network scan");
}

void RouteHandlers::getPollWiFiScan() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED || n == WIFI_SCAN_RUNNING) {
    server.send(200, "text/plain", "'{\"scanComplete\": \"false\"}'");
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
    server.send(200, "text/plain", "{\"scanComplete\": \"true\", \"data\": " + obj + "}");
  }
}

void RouteHandlers::getAllCanvasInfo(Canvas * canvases) {
  String obj = "[";
  for (uint8_t i = 0; i < API_LIMIT; i++) {
    APIData data = canvases[i].getAPIData();

    char timestr[32];
    std::time_t secsSinceEpoch = (long) data.lastRefreshed;
    struct std::tm ts;
    ts = *std::localtime(&secsSinceEpoch);
    std::strftime(timestr, sizeof(timestr), "%a %b %e, %R", &ts);

    String jsonString = i > 0 ? "," : "";
    jsonString += "{\"name\": \"";
    jsonString += data.name;
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
  obj += "]";
  server.send(200, "text/plain", obj);
}

void RouteHandlers::getCanvasInfo(Canvas * canvases) {
  if (!server.hasArg("index") || server.arg("index") == NULL) {
    Serial.println("Invalid index for API");
    server.send(400, "text/plain", "400: Invalid Request");
  } else {
    int index = server.arg("index").toInt();
    APIData data = canvases[index].getAPIData();

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
    jsonString += canvases[index].getElementsString();
    jsonString += "\"}";

    server.send(200, "text/plain", jsonString);
  }
}

void RouteHandlers::postResetCanvas(Canvas * canvases) {
  if (!server.hasArg("index") || server.arg("index") == NULL) {
    Serial.println("Invalid index for API");
    server.send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = server.arg("index").toInt();
    Serial.printf("Removing API and Elements: %s\n", canvases[i].getAPIData().name.c_str());
    canvases[i].resetAPI();
    canvases[i].resetElements();
    server.send(200, "text/plain", "200: Removed API");
  }
}

void RouteHandlers::postToggleCanvas(Canvas * canvases) {
  if (!server.hasArg("index") || server.arg("index") == NULL) {
    Serial.println("Invalid index for API");
    server.send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = server.arg("index").toInt();
    Serial.printf("Toggling active status for API: %s\n", canvases[i].getAPIData().name.c_str());
    canvases[i].toggleAPI();
    server.send(200, "text/plain", "200: Toggled active status for API");
  }
}

void RouteHandlers::postSetCanvas(Canvas * canvases) {
  if (!server.hasArg("index") || server.arg("index") == NULL) {
    Serial.println("Invalid index for API");
    server.send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = server.arg("index").toInt();
    Serial.printf("Setting data for API: %s\n", canvases[i].getAPIData().name.c_str());

    char allrules[500]; 
    urldecode(server.arg("parserules")).toCharArray(allrules, 500);
    std::map<String, APIParseRule> ruleset;
    char * endparserule;
    char * parserule = strtok_r(allrules, ",", &endparserule);
    while(parserule != NULL) {
      char * values[5];
      byte index = 0; 
      char * endrule;
      char * rule = strtok_r(parserule, " ", &endrule);
      while (rule != NULL) {
        values[index++] = rule;
        rule = strtok_r(NULL, " ", &endrule);
      }

      ruleset.insert(std::make_pair(
        values[0],
        APIParseRule{
          atoi(values[1]),
          atoi(values[2]),
          CRGB(strtoul(values[3], NULL, HEX)),
          static_cast<APIValueType>(atoi(values[4]))
        }
      ));

      // Serial.printf("Parserule: %s | Tokens: %s, %s, %s, %s\n", parserule, values[0], values[1], values[2], values[3]);
      parserule = strtok_r(NULL, ",", &endparserule);
    }

    canvases[i].setAPI(
      urldecode(server.arg("name")),
      urldecode(server.arg("url")),
      (long) server.arg("refresh").toInt(),
      server.arg("active") == "true",
      ruleset
    );

    char elementset[500]; 
    urldecode(server.arg("additionalelements")).toCharArray(elementset, 500);
    canvases[i].setElements(elementset);

    server.send(200, "text/plain", "200: Set data for API");
  }
}