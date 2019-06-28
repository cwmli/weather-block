#include <Arduino.h>
#include <WeatherBlockAP.h>

WeatherBlockAP wbAP;

void setup() {
  wbAP.init();

  pinMode(LED_DATA, OUTPUT);
}

void loop() {
  wbAP.update();
}
