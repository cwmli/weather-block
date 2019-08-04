
#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <FastLED.h>
#include <Icons.h>

#define NUM_LEDS           192
#define LED_DATA           0
#define LED_TYPE           WS2811
#define CLR_ORDR           GRB

#define DEFAULT_BRIGHTNESS 16
#define MAX_BRIGHTNESS     64

#define MAX_ELEMENTS 10

class CRGB;

class LEDController {
  CRGB leds[NUM_LEDS];

  uint8_t currentBrightness;

public:
  LEDController() {
    currentBrightness = DEFAULT_BRIGHTNESS;
  }

  void init();

  void incrementBrightness();

  void decrementBrightness();

  uint8_t getBrightness();

  void setBrightness(uint8_t brightness);

  void update();

  void reset();

  void text(String& string, int x, int y, CRGB& color);

  void icon(const Icons::Base * icon, int x, int y);

private:

  CRGB wheel(byte pos);
};
#endif
