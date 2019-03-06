#include "rle.h"
#include "debug.h"

#include <string.h>

int rle_decode_8bit(unsigned char* dest, unsigned int dest_size, const unsigned char* src)
{
  //unsigned char count = 0;
  unsigned char prev_byte = 0;
  bool prev_byte_valid = false;

  const unsigned char* dest_end = dest + dest_size;
  unsigned char* d = dest;

  while(d < dest_end) {
        
    *d++ = *src;
    if (prev_byte_valid && (*src == prev_byte)) {

      src++;

      if (d + *src > dest + dest_size) {
        TRACE("rle_decode_8bit: destination overflow!\n");
        return -1;
      }

      memset(d, prev_byte, *src);
      d += *src++;
      prev_byte_valid = false;
    }
    else {
      prev_byte = *src++;
      prev_byte_valid = true;
    }
  }

  return d - dest;
}
