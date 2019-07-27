#include <Arduino.h>
#include <WeatherBlockAP.h>
#include <BtnState.h>

WeatherBlockAP wbAP;

BtnState lwrbr(D8);
BtnState incbr(D7);
BtnState acvdcr(D6);
BtnState acvinc(D5);

void setup() {
  wbAP.init();

  pinMode(LED_DATA, OUTPUT);

  // inputs
  pinMode(D8, INPUT);
  pinMode(D7, INPUT);
  pinMode(D6, INPUT);
  pinMode(D5, INPUT);
}

void loop() {
  wbAP.update();

  uint8_t lwrbrState = lwrbr.get();
  uint8_t incbrState = incbr.get();
  uint8_t acvdcrState = acvdcr.get();
  uint8_t acvincState = acvinc.get();

  if (lwrbrState == S_PRESS) {
    wbAP.decrementBrightness();
  }
  if (incbrState == S_PRESS) {
    wbAP.incrementBrightness();
  }
  if (acvdcrState == S_PRESS) {
    wbAP.decrementActiveCanvas();
  }
  if (acvincState == S_PRESS) {
    wbAP.incrementActiveCanvas();
  }
}
