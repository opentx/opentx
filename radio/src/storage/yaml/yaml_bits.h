#ifndef _yaml_bits_h_
#define _yaml_bits_h_

#include <stdint.h>

void yaml_put_bits(uint8_t* dst, uint32_t i, uint32_t bit_ofs, uint8_t bits);
uint32_t yaml_get_bits(uint8_t* src, uint32_t bit_ofs, uint8_t bits);

// assumes bits is a multiple of 8
bool yaml_is_zero(uint8_t* data, uint8_t bits);

#endif
