#ifndef KEYDOWN_H
#define KEYDOWN_H

////// Config section

#define BUTTON1_PIN 12
// #define BUTTON2_PIN 11


/////// code section

typedef enum BUTTONS {
  BUTTON1 = 1001,
  // todo: ifefs?
  BUTTON2, BUTTON3, BUTTON4, BUTTON5,
} button_t;

#define BUTTON1_MASK 1
#define BUTTON2_MASK 2
#define BUTTON3_MASK 4
#define BUTTON4_MASK 8
#define BUTTON5_MASK 16

void init_buttons();
int is_keydown(button_t button);

#endif // KEYDOWN_H
