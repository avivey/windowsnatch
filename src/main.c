#include "WProgram.h"

#include "hacks.h"

#define YELLOW 13
#define GREEN 14
#define RED 15

#define BUTTON 12

int main(void)
{
// start:
	pinMode(YELLOW, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BUTTON, INPUT);

  int i = 3;
	while (i--) {
		digitalWriteFast(YELLOW, HIGH);
    delay(100);
		digitalWriteFast(RED, HIGH);
    delay(100);
		digitalWriteFast(GREEN, HIGH);
		delay(100);

		digitalWriteFast(YELLOW, LOW);
		delay(100);
		digitalWriteFast(RED, LOW);
		delay(100);
		digitalWriteFast(GREEN, LOW);
		delay(100);
	}
  digitalWriteFast(YELLOW, HIGH);

return 0;
  while(1) {
    if (digitalReadFast(BUTTON)) {
      digitalWriteFast(GREEN, HIGH);
      delay(10);
      digitalWriteFast(GREEN, LOW);
    } else {
      digitalWriteFast(RED, HIGH);
      delay(10);
      digitalWriteFast(RED, LOW);
    }
     delay(30);
  }
  // goto start;
}

// hack debug lights.
void aviv_debug_on(uint8_t led)
{
  digitalWriteFast(led, HIGH);
}
void aviv_debug_off(uint8_t led)
{
  digitalWriteFast(led, LOW);
}
