#pragma once

#include <stdint.h>
#include <stddef.h>

#define OUTPUT 1
#define LOW 0
#define HIGH 1

void pinMode(uint8_t pin, uint8_t mode);
void digitalWriteFast(uint8_t pin, uint8_t val);
