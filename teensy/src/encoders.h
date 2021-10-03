#ifndef ENCODERS_H
#define ENCODERS_H

#include "button_names.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t encoder_value_t ;

typedef struct {
  encoder_value_t old_value;
  encoder_value_t new_value;
} encoder_read_result_t;

typedef struct {
  unsigned char id;
  button_t button;
  void* encoder;
  encoder_value_t last_read_value;
} encoder_t;

typedef struct {
  unsigned char id;
  int8_t old_value;
  int8_t new_value;
  button_t button;
} encoder_temp_info_t;

encoder_read_result_t encoder_get_and_reset(encoder_t*);

typedef void (*encoder_operator)(encoder_t*, void*);
void iterate_over_all_encoders(encoder_operator, void* extra);


#ifdef __cplusplus
}
#endif

#endif // ENCODERS_H
