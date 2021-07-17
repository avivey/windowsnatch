#include "WProgram.h"
#include "usb_rawhid.h"

#include "keydown.h"
#include "toolset.h"
#include "common/compat.h"

#include "lights.h"

#include "common/icd_messages.h"
#include "common/configuration.h"

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

void set_led_color_operator(toolset_t* toolset, void* pcolor) {
  unsigned char color = *(unsigned char*)pcolor;
  set_led_color(toolset, color);
}

#define ITERATION_TIME_MS 10
int _keepalive_next = 0;
int _keepalive_timer = 0;

void animate_bar(int count);

int main(void) {
  init_all_led_pins();
  init_buttons();

  animate_bar(3);

  while (true) {
    Buffer buffer = get_buffer();
    if (usb_rawhid_recv(buffer, 0)) { // 0 timeout = do not wait
      dispatch_incoming_message(buffer);
      _keepalive_next = _keepalive_timer + KEEPALIVE_TIMER_SECONDS * 1000 * 2;
    } else if (_keepalive_next < _keepalive_timer) {
      unsigned char black = 0;
      iterate_over_all_toolsets(set_led_color_operator, &black);
    }

    iterate_over_all_toolsets(transmit_clicks, NULL);
    lights_render();
    delay(ITERATION_TIME_MS);
    _keepalive_timer += ITERATION_TIME_MS;
  }
}

void transmit_version_info() {
  Buffer buffer = clean_and_get_buffer();
  buffer[0] = ICD_MAGIC_NUMBER;
  buffer[1] = MSG_CODE_VERSION_STRING;
  strncpy((char*)(buffer + 2), CODE_VERSION_STR, 61);
  buffer[63] = 0;

  int bytes = usb_rawhid_send(buffer, 200);
  if (bytes <= 0) {
    animate_error(3);
  }
}


unsigned char animation[] = {
  0, 1, 2, 3, 4, 5, 6, 7
};

void animate_bar_operator(toolset_t* toolset, void* poffset) {
  int offset = *(int*)poffset;
  unsigned char color = animation[(toolset->id + offset) % 8];
  set_led_color(toolset, color);
}


void animate_bar(int count) {
  int offset = 3;

  while (count >= 0) {
    iterate_over_all_toolsets(animate_bar_operator, &offset);
    lights_render();
    delay(1000);
    offset ++;
    count --;
  }
}

void signal_error(int count) {
  animate_error(count);
}

void animate_error(int count) {
  unsigned char red = 4;
  unsigned char black = 0;

  while (count > 0) {
    iterate_over_all_toolsets(set_led_color_operator, &red);
    lights_render();
    delay(200);
    iterate_over_all_toolsets(set_led_color_operator, &black);
    lights_render();
    delay(200);
    count --;
  }
}
