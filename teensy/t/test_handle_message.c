#include "test_handle_message.h"
#include "../src/common/icd_messages.h"

int print_high_pins();
void clear_all_outpins();
void _reboot_Teensyduino_() {}
void signal_error(int _) {}

int test_handle_message() {
  clear_all_outpins();

  uint8_t buffer[RAW_HID_BUFFER_SIZE];
  buffer[1] = MSG_CODE_SET_LED;
  buffer[2] = 2;  // count of pins
  // Pair 1
  buffer[3] = 5;  // Set 5
  buffer[4] = 4;  // R  - pin 18
  // Pair 2
  buffer[5] = 0;  // Set 0
  buffer[6] = 7;  // R+G+B - pins 0, 1, 2

  dispatch_incoming_message(buffer);

  int count = print_high_pins();
  if (count != 4) return 1;

  return 0;
}
