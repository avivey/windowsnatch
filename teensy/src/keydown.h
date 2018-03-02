#ifndef KEYDOWN_H
#define KEYDOWN_H

////// Config section

#define BUTTON1_PIN 12
#define BUTTON2_PIN 11


/////// code section

typedef enum BUTTONS {
  BUTTON1 = 1001,
#ifdef BUTTON2_PIN
  BUTTON2,
#endif
  BUTTON3,
  BUTTON4,
  BUTTON5,
} button_t;

#define BUTTON1_MASK 0b1
#define BUTTON2_MASK 0b10
#define BUTTON3_MASK 0b100
#define BUTTON4_MASK 0b1000
#define BUTTON5_MASK 0b10000

void init_buttons();
int is_keydown(button_t button);

#endif // KEYDOWN_H
