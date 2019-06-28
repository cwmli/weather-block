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


void LEDController::setBrightness(uint8_t brightness) {
  if (brightness <= MAX_BRIGHTNESS) {
    FastLED.setBrightness(brightness);
  }
}

void LEDController::setSolidColor(uint8_t r, uint8_t g, uint8_t b) {
  color.setRGB(r, g, b);
}

void LEDController::update() {
  FastLED.show();
}

void LEDController::reset() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  resetActiveLeds();
}

void LEDController::text(String& string, int& x, int& y, bool& scroll, int& scrollSpeed) {
  static int startCol = scroll ? COLUMNS : x;
	static unsigned long lastScrollUpdate;

  int len = string.length();
  int offset = startCol;
	for (int i = 0; i < len; i++) {
		if (string[i] == 32) {
      offset += SPACE_W;
			continue;
		}

    if (string[i] >= 48 && string[i] <= 57) {
      fillNum(y, offset, string[i]);
      offset += NUM_X + 1;
    } else {
		  fillChar(y, offset, string[i]);
      offset += FONT_X + 1;
    }
  }

  if (scroll && millis() - lastScrollUpdate > scrollSpeed) {
		lastScrollUpdate = millis();

		startCol -= 1;
	}

  for (int c = 0; c < COLUMNS; c++) {
    for (int r = 0; r < ROWS; r++) {
      if (getActiveLed(r, c) == 1) {
        leds[gridIndex(r, c)] = color;
      }
      else {
        leds[gridIndex(r, c)] = CRGB::Black;
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
