#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test_handle_message.h"

#include "../src/toolset.h"

#define TOTAL_PINS  39
uint8_t output_pins[TOTAL_PINS];
// Stubs
void pinMode(uint8_t pin, uint8_t mode) {}
void digitalWriteFast(uint8_t pin, uint8_t val) {
  output_pins[pin] = val;
}
int is_keydown(button_t button) {
  return 0;
}
void animate_error(int count) {}

void clear_all_outpins() {
  memset(output_pins, 0, TOTAL_PINS);
}

int print_high_pins() {
  int count = 0;
  for (int i = 0; i < TOTAL_PINS; i++) {
    if (output_pins[i] > 0) {
      count ++;
      printf("Pin high: %d\n", i);
    }
  }
  if (count == 0) {
    printf("No pins are high\n");
  }
  return count;
}

void print_led_color(toolset_t* toolset, unsigned char color) {
  printf("setting toolset %d to color %d\n", toolset->id, color);
}

int counts[] = {2, 3, 4, 5, 6, 7};
void test_toolset(toolset_t* toolset, void* ptr) {
  printf("%d %d %d\n", toolset->id, toolset->button1, toolset->button2);

  int n = counts[toolset->id];

  if (is_keydown(toolset->button1)) {
    n += 1;
  }
  if (is_keydown(toolset->button2)) {
    n -= 1;
  }

  n = (n + 8  % 8);

  print_led_color(toolset, n);

  counts[toolset->id] = n;
}

int main() {
  // void* ptr = NULL;
  // iterate_over_all_toolsets(test_toolset, ptr);

  return test_handle_message();
}
