#include "WProgram.h"
#include "usb_rawhid.h"

#include "keydown.h"
#include "toolset.h"

#include "common/icd_messages.h"


void aviv_debug_number(uint8_t number);
void animate_error(int count);

int counts[] = {2, 3, 4, 5, 6, 0};

void counter(toolset_t* toolset, void* __) {
  int n = counts[toolset->id];

  if (is_keydown(toolset->button1)) {
    n += 1;
  }
  if (is_keydown(toolset->button2)) {
    n -= 1;
  }

  n = (n + 8  % 8);

  set_led_color(toolset, n);

  counts[toolset->id] = n;
}


void transmit_clicks(toolset_t* toolset) {
  int keydown1 = is_keydown(toolset->button1);
  int keydown2 = is_keydown(toolset->button2);

  if (!keydown1 && !keydown2) {
    return;
  }

  Buffer buffer = clean_and_get_buffer();
  memset(buffer, 0, RAW_HID_BUFFER_SIZE);
  buffer[0] = ICD_MAGIC_NUMBER;
  buffer[1] = MSG_CODE_BUTTON_PRESS;
  if (keydown1 && keydown2) {
    buffer[2] = 2;
  } else {
    buffer[2] = 1;
  }

  int n = 3;
  if (keydown1) {
    buffer[n] = toolset->id;
    n++;
    buffer[n] = 1;
    n++;
  }
  if (keydown2) {
    buffer[n] = toolset->id;
    n++;
    buffer[n] = 2;
    n++;
  }

  int bytes = usb_rawhid_send(buffer, 200);
  if (bytes <= 0) {
    animate_error(3);
  }
}

void reset_if_click(toolset_t* toolset) {
  if (is_keydown(toolset->button1)) {
    animate_error(2);
  }
  if (is_keydown(toolset->button2)) {
    _reboot_Teensyduino_();
  }
}

void selective_action(toolset_t* toolset, void* p) {
  switch (toolset->id) {
  case 5:
    return reset_if_click(toolset);

  default:
    return transmit_clicks(toolset);
  }
}

int main(void) {
  init_all_led_pins();
  init_buttons();

  animate_error(3);

  while (1) {
    Buffer buffer = get_buffer();
    if (usb_rawhid_recv(buffer, 0)) { // 0 timeout = do not wait
      dispatch_incoming_message(buffer);
    }

    if (is_keydown(BUTTON62)) {
      _reboot_Teensyduino_();
    }

    if (is_keydown(BUTTON61)) {
      animate_error(2);
    }

    iterate_over_all_toolsets(selective_action, NULL);
    delay(10);
  }
}

int prev_main(void) {
  Buffer buffer = clean_and_get_buffer();
  unsigned int packetCount = 0;
  while (1) {
    if (is_keydown(BUTTON62)) {
      _reboot_Teensyduino_();
    }

    int bytes = usb_rawhid_recv(buffer, 0); // 0 timeout = do not wait
    if (bytes > 0) {
      // the computer sent a message.
      int n = buffer[0] & 0b111;
      aviv_debug_number(n);
    }

    int keydown1 = is_keydown(BUTTON11);
    int keydown2 = is_keydown(BUTTON12);

    if (keydown1 || keydown2) {
      // first 2 bytes are a signature
      buffer[0] = 0xAB;
      buffer[1] = 0xCD;

      buffer[3] = keydown1;
      buffer[4] = keydown2;

      // and put a count of packets sent at the end
      buffer[62] = highByte(packetCount);
      buffer[63] = lowByte(packetCount);

      // actually send the packet
      bytes = usb_rawhid_send(buffer, 1000);
      if (bytes > 0) {
        packetCount = packetCount + 1;
      } else {
        animate_error(5);
      }
    }

    delay(10);
  }
}

void aviv_debug_number(uint8_t number) {
  if (number > 7) number = 7;
  set_led_color(get_toolset(0), number);
}

void set_led_color_operator(toolset_t* toolset, void* pcolor) {
  unsigned char color = *(unsigned char*)pcolor;
  set_led_color(toolset, color);
}

void signal_error(int count) {
  animate_error(count);
}

void animate_error(int count) {
  unsigned char red = 4;
  unsigned char black = 0;

  while (count > 0) {
    iterate_over_all_toolsets(set_led_color_operator, &red);
    delay(200);
    iterate_over_all_toolsets(set_led_color_operator, &black);
    delay(200);
    count --;
  }
}
