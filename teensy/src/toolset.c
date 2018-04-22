#include <stdio.h>
#include "WProgram.h"

#include "toolset.h"

#include "toolsets_init.inc"

void iterate_over_all_toolsets(toolset_operator operator, void* extra) {
  for (int i = 0; i < NUM_TOOLSETS; i++) {
    operator(get_toolset(i), extra);
  }
}

void init_toolset_leds(toolset_t* toolset, void* _) {
  pinMode(toolset->led_pin_red, OUTPUT);
  pinMode(toolset->led_pin_green, OUTPUT);
  pinMode(toolset->led_pin_blue, OUTPUT);
}

void init_all_led_pins() {
  iterate_over_all_toolsets(init_toolset_leds, NULL);
}

toolset_t* get_toolset(int index) {
  if (index < NUM_TOOLSETS) {
    return &toolset_all_toolsets[index];
  }
  return NULL;
}

void set_led_color(toolset_t* toolset, unsigned char color) {
  int r = (color & 0b100) ? HIGH : LOW;
  int g = (color & 0b010) ? HIGH : LOW;
  int b = (color & 0b001) ? HIGH : LOW;

  digitalWriteFast(toolset->led_pin_red, r);
  digitalWriteFast(toolset->led_pin_green, g);
  digitalWriteFast(toolset->led_pin_blue, b);
}
