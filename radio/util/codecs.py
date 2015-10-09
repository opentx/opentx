#!/bin/env python

from __future__ import print_function

SIGN_BIT = (0x80)   # Sign bit for a A-law byte.
QUANT_MASK = (0xf)  # Quantization field mask.
SEG_SHIFT = (4)     # Left shift for segment number.
SEG_MASK = (0x70)   # Segment field mask.
BIAS = (0x84)       # Bias for linear code.


def alaw2linear(a_val):
    a_val ^= 0x55

    t = a_val & QUANT_MASK
    seg = (a_val & SEG_MASK) >> SEG_SHIFT
    if (seg):
        t = (t + t + 1 + 32) << (seg + 2)
    else:
        t = (t + t + 1) << 3

    if a_val & SIGN_BIT:
        return t
    else:
        return -t


def ulaw2linear(u_val):
    # Complement to obtain normal u-law value.
    u_val = ~u_val

    # Extract and bias the quantization bits. Then
    # shift up by the segment number and subtract out the bias.
    t = ((u_val & QUANT_MASK) << 3) + BIAS
    t <<= (u_val & SEG_MASK) >> SEG_SHIFT

    if u_val & SIGN_BIT:
        return (BIAS - t)
    else:
        return (t - BIAS)


def pcmTable(fn):
    result = []
    for i in range(256):
        result.append(fn(i))
    return result


def tableToString(name, table):
    result = 'const int16_t ' + name + '[256] = { '
    result += ', '.join(str(i) for i in table)
    result += ' };'
    return result


print(tableToString('alawTable', pcmTable(alaw2linear)))
print(tableToString('ulawTable', pcmTable(ulaw2linear)))
