#include "keydown.h"
#include "encoders.h"

#include "common/icd_messages.h"
#include "common/configuration.h"

#define TEENSYDUINO
#include <Encoder.h>

// By adding this line here, I don't actually need to compile/link Encoder.h
// from the libraries, which saves me some messing around with configuration.
// That file only contains this line.
Encoder_internal_state_t * Encoder::interruptArgs[];

#include "encoders_init.inc"

encoder_read_result_t encoder_get_and_reset(encoder_t* encoder) {
  // We get 4 software ticks for each tactile tick.
  Encoder* hardware = (Encoder*)(encoder->encoder);
  int8_t new_val = hardware->read() / 4;

  encoder_read_result_t result;
  result.old_value = encoder->last_read_value;
  result.new_value = new_val;

  encoder->last_read_value = new_val;
  return result;
}

void iterate_over_all_encoders(encoder_operator oper, void* extra) {
  for (int i = 0; i < NUM_ENCODERS; i++) {
    oper(&all_encoders[i], extra);
  }
}
