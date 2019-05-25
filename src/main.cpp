#include <ESP8266WiFi.h>

const char * SSID = "dlink-8898";
const char * password = "dbjxk22065";

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(SSID, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP Addr: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
}
