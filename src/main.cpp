#include <Arduino.h>
#include <SoftAPSetup.h>
#include <LEDController.h>

SoftAPSetup apSetup;
LEDController controller;

void setup() {
  apSetup.init();
  controller.init();

  pinMode(LED_DATA, OUTPUT);
}

void loop() {
  apSetup.update();
  controller.update();
}
