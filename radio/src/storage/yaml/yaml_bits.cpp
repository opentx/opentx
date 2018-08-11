#include <stdio.h>
#include "yaml_bits.h"
#include "yaml_parser.h"

#define MASK_LOWER(bits) ((1 << (bits)) - 1)
#define MASK_UPPER(bits) (0xFF << bits)

void yaml_put_bits(uint8_t* dst, uint32_t i, uint32_t bit_ofs, uint32_t bits)
{
    i &= ((1UL << bits) - 1);

    if (bit_ofs) {

        *dst &= MASK_LOWER(bit_ofs);
        *(dst++) |= (i << bit_ofs) & 0xFF;

        if (bits <= 8 - bit_ofs)
            return;

        bits -= 8 - bit_ofs;
        i = i >> (8 - bit_ofs);
    }

    while(bits >= 8) {
        *(dst++) = i & 0xFF;
        bits -= 8;
        i = i >> 8;
    }

    if (bits) {
        uint8_t mask = MASK_UPPER(bits);
        *dst &= mask;
        *dst |= i & ~mask;
    }
}


uint32_t yaml_get_bits(uint8_t* src, uint32_t bit_ofs, uint32_t bits)
{
    uint32_t i = 0;
    uint32_t bit_shift = 0;

    if (bit_ofs) {
        i = (*(src++) & MASK_UPPER(bit_ofs)) >> bit_ofs;

        if (bits <= 8 - bit_ofs) {
            i &= MASK_LOWER(bits);
            return i;
        }

        bit_shift = 8 - bit_ofs;
        bits     -= bit_shift;
    }
    
    while(bits >= 8) {

        i |= (uint32_t)*(src++) << bit_shift;

        bits      -= 8;
        bit_shift += 8;
    }

    if (bits) {
        i |= ((uint32_t)*src & MASK_LOWER(bits)) << bit_shift;
    }

    return i;
}

bool yaml_is_zero(uint8_t* data, uint32_t bits)
{
    uint8_t res = 0;

    // assume bits is a multiple of 8
    while(bits >= 8) {
        res |= *(data++);
        bits -= 8;
    }

    return !res;
}

int32_t yaml_str2int(const char* val, uint8_t val_len)
{
    bool  neg = false;
    int i_val = 0;
    
    for(uint8_t i=0; i < val_len; i++) {
        if (val[i] == '-')
            neg = true;
        else if (val[i] >= '0' && val[i] <= '9') {
            i_val = i_val * 10 + (val[i] - '0');
        }
    }

    return neg ? -i_val : i_val;
}

uint32_t yaml_str2uint(const char* val, uint8_t val_len)
{
    uint32_t i_val = 0;
    
    for(uint8_t i=0; i < val_len; i++) {
        if (val[i] >= '0' && val[i] <= '9') {
            i_val = i_val * 10 + (val[i] - '0');
        }
    }

    return i_val;
}

static char int2str_buffer[MAX_STR] = {0};
static const char _int2str_lookup[] = { '0', '1', '2', '3', '4', '5', '6' , '7', '8', '9' };

char* yaml_unsigned2str(uint32_t i)
{
    char* c = &(int2str_buffer[MAX_STR-2]);
    do {
        *(c--) = _int2str_lookup[i % 10];
        i = i / 10;
    } while((c > int2str_buffer) && i);

    return (c + 1);
}

char* yaml_signed2str(int32_t i)
{
    if (i < 0) {
        char* c = yaml_unsigned2str(-i);
        *(--c) = '-';
        return c;
    }

    return yaml_unsigned2str((uint32_t)i);
}

int32_t yaml_to_signed(uint32_t i, uint32_t bits)
{
    if (i & (1 << (bits-1))) {
        i |= 0xFFFFFFFF << bits;
    }

    return i;
}

