#include "SoftAPSetup.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include "config.h"

ESP8266WebServer server(80);

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleRoute(String path) {
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

void SoftAPSetup::init() {
  delay(1000);
  Serial.begin(115200);

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

  server.onNotFound([]() {
    if (!handleRoute(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });
  
  server.begin();
  Serial.println("HTTP server started");
}

void SoftAPSetup::update() {
  server.handleClient();
  MDNS.update();
}
