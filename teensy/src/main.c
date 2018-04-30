#include "WProgram.h"
#include "usb_rawhid.h"

#include "keydown.h"
#include "toolset.h"
#include "common/compat.h"

#include "common/icd_messages.h"

void animate_error(int count);

void transmit_clicks(toolset_t* toolset, void* __) {
  int keydown1 = is_keydown(toolset->button1);
  int keydown2 = is_keydown(toolset->button2);

  if (!keydown1 && !keydown2) {
    return;
  }

  Buffer buffer = clean_and_get_buffer();
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

int main(void) {
  init_all_led_pins();
  init_buttons();

  animate_error(2);

  while (1) {
    Buffer buffer = get_buffer();
    if (usb_rawhid_recv(buffer, 0)) { // 0 timeout = do not wait
      dispatch_incoming_message(buffer);
    }

    iterate_over_all_toolsets(transmit_clicks, NULL);
    delay(10);
  }
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
