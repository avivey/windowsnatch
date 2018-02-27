#include "keydown.h"
#include "WProgram.h"
// TODO genereate this from python?
static unsigned char button_buffer = 0;

void init_buttons() {
#ifdef BUTTON1_PIN
  pinMode(BUTTON1_PIN, INPUT_PULLDOWN);
#endif
#ifdef BUTTON2_PIN
  pinMode(BUTTON2_PIN, INPUT_PULLDOWN);
#endif
#ifdef BUTTON3_PIN
  pinMode(BUTTON3_PIN, INPUT_PULLDOWN);
#endif
#ifdef BUTTON4_PIN
  pinMode(BUTTON4_PIN, INPUT_PULLDOWN);
#endif
#ifdef BUTTON5_PIN
  pinMode(BUTTON5_PIN, INPUT_PULLDOWN);
#endif
}

#define TRUE 1
#define FALSE 0
#define BOOL(X) X ? TRUE : FALSE
int is_keydown(button_t button) {
  int is_pressed, last;
  unsigned char mask;

#ifdef BUTTON1_PIN
// TODO make code more generic
  if (button == BUTTON1) {
    is_pressed = BOOL(digitalRead(BUTTON1_PIN));
    mask = BUTTON1_MASK;

    last = BOOL(button_buffer & mask);

    if (last == is_pressed) {
      return FALSE;
    }
    if (last && !is_pressed) {
      // Key up event
      button_buffer &= (!mask);  // Clear flag
      return FALSE;
    }
    if (is_pressed && !last) {
      //key down event
      button_buffer |= mask;  // set flag
      return TRUE;
    }
  }
#endif
#ifdef BUTTON2_PIN
if (button == BUTTON2) {
    is_pressed = BOOL(digitalRead(BUTTON2_PIN));
    mask = BUTTON2_MASK;

    last = BOOL(button_buffer & mask);

    if (last == is_pressed) {
      return FALSE;
    }
    if (last && !is_pressed) {
      // Key up event
      button_buffer &= (!mask);  // Clear flag
      return FALSE;
    }
    if (is_pressed && !last) {
      //key down event
      button_buffer |= mask;  // set flag
      return TRUE;
    }
  }
#endif
  return FALSE;
}

