#ifndef TOOLSET_H
#define TOOLSET_H

#include "keydown.h"

typedef struct {
  unsigned char id;
  unsigned char light_index;
  button_t button1;
  button_t button2;
} toolset_t;

typedef void (*toolset_operator)(toolset_t*, void*);
const int NUM_TOOLSETS;

void init_all_led_pins();
void iterate_over_all_toolsets(toolset_operator, void* extra);
toolset_t* get_toolset(int index);

void set_led_color(toolset_t*, unsigned char color);
#endif // TOOLSET_H
