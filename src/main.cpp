#include <Arduino.h>
#include <SoftAPSetup.h>

SoftAPSetup apSetup;

void setup() {
  apSetup.init();
}

void loop() {
  apSetup.update();
}
