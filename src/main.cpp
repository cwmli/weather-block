#include <Arduino.h>
#include <WeatherBlockAP.h>
#include <LEDController.h>

WeatherBlockAP wbAP;
LEDController controller;

void setup() {
  wbAP.init();
  controller.init();

  pinMode(LED_DATA, OUTPUT);
}

void loop() {
  wbAP.update();
  controller.update();
}
