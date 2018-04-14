#include "keydown.h"
#include "WProgram.h"

#include "button_setup.h"

int __is_keydown(int pin, button_mask_t mask);
#include "button_setup.inc"

static button_mask_t button_buffer = 0;

#define TRUE 1
#define FALSE 0
#define BOOL(X) X ? TRUE : FALSE

int __is_keydown(int pin, button_mask_t mask) {
  int is_pressed = BOOL(digitalRead(pin));
  int last = BOOL(button_buffer & mask);

  if (last == is_pressed) {
    return FALSE;
  }
  if (last && !is_pressed) {
    // Key up event
    button_buffer &= (~mask);  // Clear flag
    return FALSE;
  }
  if (is_pressed && !last) {
    //key down event
    button_buffer |= mask;  // set flag
    return TRUE;
  }

  return FALSE;
}
