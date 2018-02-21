#include "WProgram.h"
#include "usb_rawhid.h"


#define YELLOW 13
#define RED 3
#define GREEN 5
#define BUTTON 12

void aviv_debug_number(uint8_t number);



int main(void)
{
  int n;
// RawHID packets are always 64 bytes
  byte buffer[64];
  unsigned long next_send;
  unsigned int packetCount = 0;

  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLDOWN);
  next_send = millis() + 2000;
  aviv_debug_number(7);

  while (1) {
    n = usb_rawhid_recv(buffer, 0); // 0 timeout = do not wait
    if (n > 0) {
      // the computer sent a message.
      n = buffer[0] & 0b111;
      aviv_debug_number(n);
    }

    // every 2 seconds, send a packet to the computer
    if (next_send < millis()) {
      next_send = millis() + 2000;

      memset(buffer, 0, 64);

      // first 2 bytes are a signature
      buffer[0] = 0xAB;
      buffer[1] = 0xCD;

      buffer[3] = digitalRead(BUTTON);

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
