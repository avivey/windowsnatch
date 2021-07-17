#include "WProgram.h"

#include "toolset.h"
#include "lights.h"

#include "toolsets_init.inc"

void iterate_over_all_toolsets(toolset_operator operator, void* extra) {
  for (int i = 0; i < NUM_TOOLSETS; i++) {
    operator(get_toolset(i), extra);
  }
}

void init_all_led_pins() {
  lights_init();
}

toolset_t* get_toolset(int index) {
  if (index < NUM_TOOLSETS) {
    return &toolset_all_toolsets[index];
  }
  return NULL;
}

void set_led_color(toolset_t* toolset, unsigned char color) {
  if (toolset == NULL) return;

  lights_set_pixel_1bit(toolset->light_index, color);
}
