#include "LEDController.h"
#include "GridControl.h"

void LEDController::init() {
  FastLED.addLeds<LED_TYPE, LED_DATA, CLR_ORDR>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
}

void LEDController::incrementBrightness() {
  if (currentBrightness == 0) {
    currentBrightness = 2;
  } else {
    currentBrightness = min(currentBrightness * 2, MAX_BRIGHTNESS);
  }

  setBrightness(currentBrightness);
}

void LEDController::decrementBrightness() {
  if (currentBrightness == 1) {
    currentBrightness = 0;
  } else {
    currentBrightness = currentBrightness / 2;
  }

  setBrightness(currentBrightness);
}

uint8_t LEDController::getBrightness() {
  return currentBrightness;
}

void LEDController::setBrightness(uint8_t brightness) {
  float fbrightness = brightness;
  float scale = fbrightness / 100.0f;
  brightness = MAX_BRIGHTNESS * scale;

  FastLED.setBrightness(brightness);
}

void LEDController::update() {
  FastLED.show();
}

void LEDController::reset() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  resetActiveLeds();
}

void LEDController::text(String& string, int x, int y, CRGB& color) {
  int len = string.length();
  int offset = x;
	for (int i = 0; i < len; i++) {
		if (string[i] == 32) {
      offset += SPACE_W;
			continue;
		}

    if (string[i] >= 48 && string[i] <= 57) {
      fillNum(y, offset, string[i]);
      offset += NUM_X + 1;
    } else {
		  int spc = fillChar(y, offset, string[i]);
      offset += spc + 1;
    }
  }

  for (int c = x; c < offset; c++) {
    for (int r = y; r < FONT_Y + 1; r++) {
      if (getActiveLed(r, c) == 1) {
        leds[gridIndex(r, c)] = color;
      }
    }
  }
}

void LEDController::icon(const Icons::Base * icon, int x, int y) {
  for (int h = 0; h < ICON_HEIGHT; h++) {
    for (int w = 0; w < ICON_WIDTH; w++) {
      if (icon->cdata[h][w] == 0) {
        continue;
      }

      setActiveLed(y + h, x + w);
      leds[gridIndex(y + h, x + w)] = icon->palette[icon->cdata[h][w] - 1];
    }
  }
}

void LEDController::idle() {
  static unsigned long lastUpdate;
  static unsigned long lastFadeUpdate;
  static unsigned long lastCycleUpdate;
  static uint8_t cycle = 0;
  static uint8_t lastDot = 0;

  unsigned long time = millis();

  if (time - lastUpdate > 1600) {
    CRGB clr = wheel(cycle);
    for (byte i = 0; i < 3; i++) {
      leds[gridIndex(3, 7 + (i * 4))] = clr; 
      leds[gridIndex(4, 7 + (i * 4))] = clr;
      leds[gridIndex(3, 8 + (i * 4))] = clr; 
      leds[gridIndex(4, 8 + (i * 4))] = clr;
    }
    lastUpdate = time;
  }

  if (time - lastCycleUpdate > 50) {
    cycle = (cycle + 1) % 255;
    lastCycleUpdate = time;
  }

  if (time - lastFadeUpdate > 15) {
    lastFadeUpdate = time;
    for (int h = 0; h < ROWS; h++) {
      for (int w = 0; w < COLUMNS; w++) {
        leds[gridIndex(h, w)].fadeToBlackBy(10);
      }
    }
  }
}

void LEDController::busy() {
  static unsigned long lastUpdate;
  static unsigned long lastFadeUpdate;
  static uint8_t cycle = 0;
  static uint8_t lastDot = 0;

  unsigned long time = millis();

  if (time - lastUpdate > 300) {
    CRGB clr = wheel(cycle);
    leds[gridIndex(3, 7 + (lastDot * 4))] = clr; 
    leds[gridIndex(4, 7 + (lastDot * 4))] = clr;
    leds[gridIndex(3, 8 + (lastDot * 4))] = clr; 
    leds[gridIndex(4, 8 + (lastDot * 4))] = clr;
    lastDot = (lastDot + 1) % 3;
    cycle = (cycle + 60) % 255;
    lastUpdate = time;
  }

  if (time - lastFadeUpdate > 15) {
    lastFadeUpdate = time;
    for (int h = 0; h < ROWS; h++) {
      for (int w = 0; w < COLUMNS; w++) {
        leds[gridIndex(h, w)].fadeToBlackBy(10);
      }
    }
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
