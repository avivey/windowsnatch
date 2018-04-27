#include "WProgram.h"
#include "usb_rawhid.h"

#include "keydown.h"
#include "toolset.h"

#define YELLOW 13

void aviv_debug_number(uint8_t number);

int counts[] = {2, 3, 4, 5, 6, 7};

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

int main(void) {
  pinMode(YELLOW, OUTPUT);
  digitalWriteFast(YELLOW, HIGH);

  init_all_led_pins();
  init_buttons();

  while (1) {
    iterate_over_all_toolsets(counter, NULL);
    delay(10);
  }
}

int previous_main(void) {
// RawHID packets are always 64 bytes
  byte buffer[64];
  memset(buffer, 0, 64);
  unsigned int packetCount = 0;
  while (1) {
    int n = usb_rawhid_recv(buffer, 0); // 0 timeout = do not wait
    if (n > 0) {
      // the computer sent a message.
      n = buffer[0] & 0b111;
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
      n = usb_rawhid_send(buffer, 1000);
      if (n > 0) {
        packetCount = packetCount + 1;
        digitalWriteFast(YELLOW, LOW);
      } else {
        // Serial.println(F("Unable to transmit packet"));
        digitalWriteFast(YELLOW, HIGH);
        // analogWrite(YELLOW, 80);
      }
    }

    delay(10);
  }
}

void aviv_debug_number(uint8_t number) {
  if (number > 7) number = 7;
  set_led_color(get_toolset(0), number);
}
