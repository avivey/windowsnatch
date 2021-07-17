#ifndef LIGHTS_H
#define LIGHTS_H

#ifdef __cplusplus
extern "C" {
#endif

void lights_init(void);
void lights_set_pixel(
  uint32_t index,
  uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
void lights_set_pixel_1bit(uint32_t index, unsigned char color);

// Will only actually apply render if the buffer anything changed since
// last call.
void lights_render(void);

#ifdef __cplusplus
}
#endif

#endif // LIGHTS_H
