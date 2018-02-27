#include "WProgram.h"
#include "usb_rawhid.h"

#include "keydown.h"

#define YELLOW 13
#define RED 3
#define GREEN 5

void aviv_debug_number(uint8_t number);


int main(void)
{
  int n;
// RawHID packets are always 64 bytes
  byte buffer[64];
  unsigned int packetCount = 0;

  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
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

    if (is_keydown(BUTTON1)) {
      // first 2 bytes are a signature
      buffer[0] = 0xAB;
      buffer[1] = 0xCD;

      buffer[3] = 1;

      // and put a count of packets sent at the end
      buffer[62] = highByte(packetCount);
      buffer[63] = lowByte(packetCount);

      // actually send the packet
      n = usb_rawhid_send(buffer, 1000);
      if (n > 0) {
        packetCount = packetCount + 1;
      } else {
        // Serial.println(F("Unable to transmit packet"));
        analogWrite(RED, 20);
      }
    }

    delay(10);
  }
}


void aviv_debug_number(uint8_t number) {
  int y, r, g;
  if (number > 7) number = 7;
  y = (number & 0b001) ? HIGH : LOW;
  r = (number & 0b010) ? HIGH : LOW;
  g = (number & 0b100) ? HIGH : LOW;

  digitalWriteFast(YELLOW, y);
  digitalWriteFast(RED, r);
  digitalWriteFast(GREEN, g);
}
