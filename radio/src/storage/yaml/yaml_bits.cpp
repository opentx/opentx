#include <stdio.h>
#include "yaml_bits.h"

#define MASK_LOWER(bits) ((1 << (bits)) - 1)
#define MASK_UPPER(bits) (0xFF << bits)

void yaml_put_bits(uint8_t* dst, uint32_t i, uint32_t bit_ofs, uint8_t bits)
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


uint32_t yaml_get_bits(uint8_t* src, uint32_t bit_ofs, uint8_t bits)
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

bool yaml_is_zero(uint8_t* data, uint8_t bits)
{
    uint8_t res = 0;

    // assume bits is a multiple of 8
    while(bits >= 8) {
        res |= *(data++);
        bits -= 8;
    }

    return !res;
}
