#include "common/compat.h"
#include "common/icd_messages.h"
#include "toolset.h"
#include "core_pins.h" // for reboot

// TODO move declaration to somewhere
void transmit_version_info();

void handle_message_GET_VERSION(Buffer buffer) {
  transmit_version_info();
}

void handle_message_SET_LED(Buffer buffer) {
  uint8_t pair_count = buffer[2];
  uint8_t ptr = 3;
  while (pair_count-- > 0 && ptr < RAW_HID_BUFFER_SIZE - 1) {
    uint8_t led = buffer[ptr++];
    uint8_t color = buffer[ptr++];

    set_led_color(get_toolset(led), color);
  }
}

void handle_message_ENTER_PROGRAMMING_MODE(Buffer buffer) {
  if (buffer[0] == ICD_MAGIC_NUMBER &&
      buffer[1] == MSG_CODE_ENTER_PROGRAMMING_MODE &&
      buffer[17] == 42) {
    _reboot_Teensyduino_();
  } else {
    signal_error(6);
  }
}

void handle_message_VERSION_STRING(Buffer buffer) {}
void handle_message_BUTTON_PRESS(Buffer buffer) {}
