#include <WS2812Serial.h>

#include "lights.h"

#include "common/configuration.h"

#define PIN_NEOPIXEL  24
#define NUM_NEOPIXEL  NUMBER_OF_TARGETS
#define COLORS_IN_LED 4

byte drawingMemory[NUM_NEOPIXEL * COLORS_IN_LED];
DMAMEM byte displayMemory[NUM_NEOPIXEL * COLORS_IN_LED * 4];
bool shouldRender = false;

WS2812Serial leds(NUM_NEOPIXEL, displayMemory, drawingMemory,
                  PIN_NEOPIXEL, WS2812_WGRB);

void lights_init(void) {
  leds.begin();
  for (int i = 0; i < NUM_NEOPIXEL; i++)
    lights_set_pixel(i, 0, 0, 0, 0);
}

void lights_set_pixel(
  uint32_t index,
  uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {

  leds.setPixel(index, red, green, blue, white);
  shouldRender = true;
}

void lights_render(void) {
  if (shouldRender) {
    leds.show();
  }
  shouldRender = false;
}


void lights_set_pixel_1bit(uint32_t index, unsigned char color) {
  uint8_t r, g, b, w;
  w = 0;

  if (color <= 0b111) {
    r = (color & 0b100) ? 200 : 0;
    g = (color & 0b010) ? 200 : 0;
    b = (color & 0b001) ? 200 : 0;
  }
  else {
    r = 100;
    g = 100;
    b = 100;
  }

  lights_set_pixel(index, r, g, b, w);
}
