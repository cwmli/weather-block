#include "RouteHandlers.h"

#include <ESP8266WiFi.h>

#include "Server.h"
#include "config.h"

// ContentType helper
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool RouteHandlers::getDefault(String path) {
  if (path.endsWith("/")) path += "index.html"; 

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
      || server.arg("ssid") == NULL || servr.arg("password") == NULL) {
    server.send(400, "text/plain", "400: Invalid Request"); 
  } else {
    WiFi.begin(server.arg("ssid"), server.arg("password"));
    server.send(200, "text/plain", "200: Set WiFi SSID and PW");
  }
}

void RouteHandlers::getPollWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    server.send(200, "text/plain", "Connecting to WiFi");
  } else {
    server.send(200, "text/plain", "Connected");
  }
}