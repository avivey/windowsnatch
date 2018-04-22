#include "WProgram.h"
#include "usb_rawhid.h"

#include "keydown.h"
#include "toolset.h"

#define YELLOW 13
#define RED 3
#define GREEN 4
#define BLUE 5

void aviv_debug_number(uint8_t number);


int main(void)
{
  int n;
// RawHID packets are always 64 bytes
  byte buffer[64];
  unsigned int packetCount = 0;

  pinMode(YELLOW, OUTPUT);

  init_all_led_pins();
  init_buttons();
  aviv_debug_number(7);

  memset(buffer, 0, 64);

  while (1) {
    n = usb_rawhid_recv(buffer, 0); // 0 timeout = do not wait
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
