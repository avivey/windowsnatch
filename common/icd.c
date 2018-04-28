#include <string.h>

#include "icd.h"

uint8_t buffer[RAW_HID_BUFFER_SIZE];

Buffer get_buffer() {
  return buffer;
}

Buffer clean_and_get_buffer() {
  memset(buffer, 0, RAW_HID_BUFFER_SIZE);
  return buffer;
}
