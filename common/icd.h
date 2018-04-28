#pragma once
#include <stddef.h>
#include <stdint.h>

#define RAW_HID_BUFFER_SIZE 64

#define COLOR_BLACK   0b000
#define COLOR_BLUE    0b001
#define COLOR_GREEN   0b010
#define COLOR_CYAN    0b011
#define COLOR_RED     0b100
#define COLOR_PURPLE  0b101
#define COLOR_YELLOW  0b110
#define COLOR_WHITE   0b111

typedef uint8_t* Buffer ;

Buffer get_buffer();
Buffer clean_and_get_buffer();

void dispatch_incoming_message(Buffer);

typedef struct {
  uint8_t a;
  uint8_t b;
}* pair_t;

#define FOR_PAIRS_IN_MESSAGE(BUFFER, PAIR_ARG, START_INDEX) \
  PAIR_ARG = (pair_t)(BUFFER + START_INDEX);  \
  while(PAIR_ARG)

#define NEXT_PAIR_IN_MESSAGE(BUFFER, PAIR_ARG) \
  PAIR_ARG += sizeof(PAIR_ARG); \
  if PAIR_ARG > BUFFER + RAW_HID_BUFFER_SIZE - sizeof(PAIR_ARG) \
    PAIR_ARG = NULL;
