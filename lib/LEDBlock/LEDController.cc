#include "LEDController.h"

void LEDController::init() {
  FastLED.addLeds<LED_TYPE, LED_DATA, CLR_ORDR>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
}

void LEDController::incrementBrightness() {
  if (currentBrightness == 0) {
    currentBrightness = 2;
  } else {
    currentBrightness = currentBrightness * 2 > MAX_BRIGHTNESS ? 0 : currentBrightness * 2;
  }

  setBrightness(currentBrightness);
}

void LEDController::incrementSpeed() {
  if (speed == 1000) {
    speed = 250;
  } else {
    speed += 250;
  }
}

void LEDController::cycleStyle() {
  currentStyle = (currentStyle + 1) % 3;
}

void LEDController::setState(uint8_t state) {
  currentStyle = state;
  if (currentStyle > 0) {
    styleTimer = millis();
  } 
  
  if (currentStyle == 2) {
    cycle = 0;
  }
}

uint8_t LEDController::getState() {
  return currentStyle;
}

void LEDController::setBrightness(uint8_t brightness) {
  if (brightness <= MAX_BRIGHTNESS) {
    FastLED.setBrightness(brightness);
  }
}

void LEDController::setSolidColor(uint8_t r, uint8_t g, uint8_t b) {
  color.setRGB(r, g, b);
}

void LEDController::update() {
  if (currentStyle == 0) {
    fill_solid(leds, NUM_LEDS, color);
  } else if (currentStyle == 1) {
    breathe();
  } else {
    rainbow();
  }

  FastLED.show();
}

void LEDController::breathe() {
  fill_solid(leds, NUM_LEDS, color);
  // ( e ^ ( sin((x * 250) / speed) ) - 1 / e ) * ( currentBrightness / ( e - 1 / e ) )
  float var = (exp(sin((millis() - styleTimer) * 0.39269908 / speed)) - 0.36787944) * (currentBrightness / 2.35040238);
  setBrightness(var);
}

void LEDController::rainbow() {
  if (cycle == 255) {
    cycle = 0;
  }

  fill_solid(leds, NUM_LEDS, wheel(cycle & 255));

  if (millis() - styleTimer > speed / 5) {
    cycle = cycle + 1;
    styleTimer = millis();
  }
}

CRGB LEDController::wheel(byte pos) {
  if(pos < 85) {
    return CRGB(pos * 3, 255 - pos * 3, 0);
  } 
  else if(pos < 170) {
    pos -= 85;
    return CRGB(255 - pos * 3, 0, pos * 3);
  } 
  else {
    pos -= 170;
    return CRGB(0, pos * 3, 255 - pos * 3);
  }
}
