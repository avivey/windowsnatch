#include <stdint.h>
#include <stdio.h>
#include "../src/toolset.h"

void pinMode(uint8_t pin, uint8_t mode) {}
void digitalWriteFast(uint8_t pin, uint8_t val) {}

int is_keydown(button_t button) {
  return 0;
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
  void* ptr = NULL;
  iterate_over_all_toolsets(test_toolset, ptr);
}
