#ifndef _yaml_bits_h_
#define _yaml_bits_h_

#include <stdint.h>

void yaml_put_bits(uint8_t* dst, uint32_t i, uint32_t bit_ofs, uint32_t bits);
uint32_t yaml_get_bits(uint8_t* src, uint32_t bit_ofs, uint32_t bits);

// assumes bits is a multiple of 8
bool yaml_is_zero(uint8_t* data, uint32_t bitoffs, uint32_t bits);

int32_t  yaml_str2int(const char* val, uint8_t val_len);
uint32_t yaml_str2uint(const char* val, uint8_t val_len);

char* yaml_unsigned2str(uint32_t i);
char* yaml_signed2str(int32_t i);

int32_t yaml_to_signed(uint32_t i, uint32_t bits);

#endif
