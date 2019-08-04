#include <Arduino.h>
#include <WeatherBlockAP.h>
#include <BtnState.h>

WeatherBlockAP wbAP;

BtnState brightnessBtnIncr(D7);
BtnState brightnessBtnDecr(D8);
BtnState activeCanvasBtnIncr(D5);
BtnState activeCanvasBtnDecr(D6);

BtnState subCanvasBtn(D0);

void setup() {
  wbAP.init();

  pinMode(LED_DATA, OUTPUT);

  // inputs
  pinMode(D8, INPUT);
  pinMode(D7, INPUT);
  pinMode(D6, INPUT);
  pinMode(D5, INPUT);

  pinMode(D0, INPUT);
}

void loop() {
  wbAP.update();

  uint8_t brightnessBtnDecrState = brightnessBtnDecr.get();
  uint8_t brightnessBtnIncrState = brightnessBtnIncr.get();
  uint8_t activeCanvasBtnDecrState = activeCanvasBtnDecr.get();
  uint8_t activeCanvasBtnIncrState = activeCanvasBtnIncr.get();

  uint8_t subCanvasBtnState = subCanvasBtn.get();

  if (brightnessBtnDecrState == S_PRESS) {
    wbAP.decrementBrightness();
  }
  if (brightnessBtnIncrState == S_PRESS) {
    wbAP.incrementBrightness();
  }
  if (activeCanvasBtnDecrState == S_PRESS) {
    wbAP.decrementActiveCanvas();
  }
  if (activeCanvasBtnIncrState == S_PRESS) {
    wbAP.incrementActiveCanvas();
  }

  if (subCanvasBtnState == S_PRESS) {
    wbAP.incrementSubCanvas();
  } 
}
