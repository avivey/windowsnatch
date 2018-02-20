#include "WProgram.h"
#include "usb_rawhid.h"

// this is arduion-ism
void setup();
void loop();

int main(void)
{
	setup();

	while (1) {
    loop();
	}
}




// RawHID packets are always 64 bytes
byte buffer[64];
unsigned long next_send;
unsigned int packetCount = 0;

void setup() {
  // Serial.begin(9600);
  // Serial.println(F("RawHID Example"));
  for (int i=0; i<7; i++) {
    pinMode(i, OUTPUT);
  }
    pinMode(13, OUTPUT);
    pinMode(15, OUTPUT);
    next_send = millis() + 2000;
    pinMode(12, INPUT_PULLDOWN);
}


void loop() {
  int n;
  n = usb_rawhid_recv(buffer, 0); // 0 timeout = do not wait
  if (n > 0) {
    // the computer sent a message.  Display the bits
    // of the first byte on pin 0 to 7.  Ignore the
    // other 63 bytes!
    // Serial.print(F("Received packet, first byte: "));
    // Serial.println((int)buffer[0]);
    for (int i=0; i<8; i++) {
      int b = buffer[0] & (1 << i);
      digitalWrite(i, b);
    }
  }
  // every 2 seconds, send a packet to the computer
  if (next_send < millis()) {
    next_send = millis() + 2000;
    // first 2 bytes are a signature
    buffer[0] = 0xAB;
    buffer[1] = 0xCD;
    // next 24 bytes are analog measurements
    for (int i=0; i<12; i++) {
      int val = analogRead(i);
      buffer[i * 2 + 2] = highByte(val);
      buffer[i * 2 + 3] = lowByte(val);
    }
    buffer[3] = digitalRead(12);
    // fill the rest with zeros
    for (int i=26; i<62; i++) {
      buffer[i] = 0;
    }
    // and put a count of packets sent at the end
    buffer[62] = highByte(packetCount);
    buffer[63] = lowByte(packetCount);
    // actually send the packet
    n = usb_rawhid_send(buffer, 1000);
    if (n > 0) {
      // Serial.print(F("Transmit packet "));
      // Serial.println(packetCount);
      packetCount = packetCount + 1;
      digitalWrite(15, HIGH);
    } else {
      // Serial.println(F("Unable to transmit packet"));
      digitalWrite(13, HIGH);
    }
  }
}
