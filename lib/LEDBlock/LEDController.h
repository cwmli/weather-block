
#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <FastLED.h>

#define NUM_LEDS           64
#define LED_DATA           0
#define LED_TYPE           WS2811
#define CLR_ORDR           GRB

#define DEFAULT_BRIGHTNESS 16
#define MAX_BRIGHTNESS     64

class CRGB;

class LEDController {
  CRGB leds[NUM_LEDS];
  // LET 0 be SOLID
  //     1 be BREATHING
  //     2 be COLOR CYCLE
  uint8_t currentStyle;
  uint8_t currentBrightness;

  CRGB color;

  // breatheTimer, cycleTimer
  unsigned long styleTimer;
  uint8_t cycle;
  uint16_t speed;

  int testIndex = 0;

public:
  LEDController() {
    currentBrightness = DEFAULT_BRIGHTNESS;
    currentStyle = 3;
    color = CRGB::White;
    speed = 1000;
  }

  void init();

  void incrementBrightness();

  void incrementSpeed();

  void cycleStyle();

  void setState(uint8_t state);

  uint8_t getState();

  void setBrightness(uint8_t brightness);

  void setSolidColor(uint8_t r, uint8_t g, uint8_t b);

  void update();

private:

  void breathe();

  void rainbow();

  void test();

  void text(char* string, bool scroll, int scrollSpeed);

  CRGB wheel(byte pos);
};
#endif