/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef debug_h
#define debug_h

#include <inttypes.h>

#if defined(SIMU)

  #define TRACE_DEBUG(...) { printf("-D- " __VA_ARGS__); fflush(stdout); }
  #define TRACE_DEBUG_WP(...) { printf(__VA_ARGS__); fflush(stdout); }
  #define TRACE_INFO(...) { printf("-I- " __VA_ARGS__); fflush(stdout); }
  #define TRACE_INFO_WP(...) { printf(__VA_ARGS__); fflush(stdout); }
  #define TRACE_WARNING(...) { printf("-W- " __VA_ARGS__); fflush(stdout); }
  #define TRACE_WARNING_WP(...) { printf(__VA_ARGS__); fflush(stdout); }
  #define TRACE_ERROR(...) { printf("-E- " __VA_ARGS__); fflush(stdout); }
  inline void dump(unsigned char *data, unsigned int size)
  {
    printf("DUMP %d bytes ...\n\r", size);
    unsigned int i = 0, j=0;
    while (i*32+j < size) {
      printf("%.2X ", data[i*32+j]);
      j++;
      if (j==32) {
        i++; j=0;
        printf("\n\r");
      }
    }
    printf("\n\r");
  }
  #define DUMP(data, size) dump(data, size)

#elif defined(DEBUG)

#ifdef __cplusplus
extern "C" {
#endif

void debugPutc(const char c);
void debugPuts(const char *string, ...);
void dump(unsigned char *data, unsigned int size);

#ifdef __cplusplus
}
#endif

#define TRACE_DEBUG(...) debugPuts("-D- " __VA_ARGS__)
#define TRACE_DEBUG_WP(...) debugPuts(__VA_ARGS__)
#define TRACE_INFO(...) debugPuts("-I- " __VA_ARGS__)
#define TRACE_INFO_WP(...) debugPuts(__VA_ARGS__)
#define TRACE_WARNING(...) debugPuts("-W- " __VA_ARGS__)
#define TRACE_WARNING_WP(...) debugPuts(__VA_ARGS__)
#define TRACE_ERROR(...) debugPuts("-E- " __VA_ARGS__)
#define DUMP(data, size) dump(data, size)

void debugTask(void* pdata);
void debugTx(void);

#else

  #define TRACE_DEBUG(...) { }
  #define TRACE_DEBUG_WP(...) { }
  #define TRACE_INFO(...) { }
  #define TRACE_INFO_WP(...) { }
  #define TRACE_WARNING(...) { }
  #define TRACE_WARNING_WP(...) { }
  #define TRACE_ERROR(...) { }
  #define DUMP(...) { }

#endif

#endif

