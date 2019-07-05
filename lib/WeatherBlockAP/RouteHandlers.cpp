#include "RouteHandlers.h"

#include <ESP8266WiFi.h>
#include <FS.h>
#include <ctime>

#include "APIData.h"

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

void RouteHandlers::getWiFiDetails() {
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
  jsonString += "\"}";

  server.send(200, "text/plain", jsonString);
}

void RouteHandlers::postDisconnectWiFi() {
  server.send(200, "text/plain", "true");
  WiFi.disconnect(true);
  Serial.println("Disconnected");
  delay(1000);
  Serial.println("Starting SoftAP");
  WiFi.softAP(WB_SSID, WB_PWD);
}

void RouteHandlers::getAPIInfo(Canvas * canvases) {
  String obj = "[";
  for (uint8_t i = 0; i < API_LIMIT; i++) {
    APIData data = canvases[i].getAPIData();

    char timestr[32];
    std::time_t secsSinceEpoch = (long) data.lastRefreshed;
    struct std::tm ts;
    ts = *std::localtime(&secsSinceEpoch);
    std::strftime(timestr, sizeof(timestr), "%a %b %e, %R%p", &ts);

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

void RouteHandlers::postRemoveAPI(Canvas * canvases) {
  if (!server.hasArg("index") || server.arg("index") == NULL) {
    Serial.println("Invalid index for API");
    server.send(400, "text/plain", "400: Invalid Request");
  } else {
    int i = server.arg("index").toInt();
    Serial.printf("Removing API: %s\n", canvases[i].getAPIData().name.c_str());
    canvases[i].resetAPI();
    server.send(200, "text/plain", "200: Removed API");
  }
}

void RouteHandlers::postToggleAPI(Canvas * canvases) {
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
