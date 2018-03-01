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


int __is_keydown(int pin, unsigned char mask) {
    int is_pressed, last;

    is_pressed = BOOL(digitalRead(pin));
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

  return FALSE;
}

int is_keydown(button_t button) {
#ifdef BUTTON1_PIN
  if (button == BUTTON1)
    return __is_keydown(BUTTON1_PIN, BUTTON1_MASK);
#endif
#ifdef BUTTON2_PIN
  if (button == BUTTON2)
    return __is_keydown(BUTTON2_PIN, BUTTON2_MASK);
#endif
  return FALSE;
}

