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
 * opentx is based on code named
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

#include "opentx.h"
#include <stdarg.h>

#if defined(SIMU)
traceCallbackFunc traceCallback = 0;
#endif

#if defined(SIMU)
void debugPrintf(const char * format, ...)
{
  va_list arglist;
  char * str;
  va_start(arglist, format);
  vasprintf(&str, format, arglist);
  va_end(arglist);
  fputs(str, stdout);
  fflush(stdout);
  if (traceCallback) {
    traceCallback(str);
  }
  free(str);
}
#endif

#if defined(DEBUG_TRACE_BUFFER)
static struct TraceElement traceBuffer[TRACE_BUFFER_LEN];
static uint8_t traceBufferPos;
gtime_t filltm(gtime_t *t, struct gtm *tp);

void trace_event(enum TraceEvent event, uint32_t data)
{
  if (traceBufferPos >= TRACE_BUFFER_LEN) return;
  __disable_irq();
  struct TraceElement * p = &traceBuffer[traceBufferPos++];
  __enable_irq();
  p->time = g_rtcTime;
  p->time_ms = g_ms100;
  p->event = event;
  p->data = data;
}

void trace_event_i(enum TraceEvent event, uint32_t data)
{
  if (traceBufferPos >= TRACE_BUFFER_LEN) return;
  struct TraceElement * p = &traceBuffer[traceBufferPos++];
  p->time = g_rtcTime;
  p->time_ms = g_ms100;
  p->event = event;
  p->data = data;
}


const struct TraceElement * getTraceElement(uint16_t idx)
{
  if (idx < TRACE_BUFFER_LEN) return &traceBuffer[idx];
  return 0;
}

#include "stamp-opentx.h"

void dumpTraceBuffer()
{
  TRACE("Dump of Trace Buffer (" VERS_STR " " DATE_STR " " TIME_STR "):");
  TRACE("#   Time                     Event  Data");
  for(int n = 0; n < TRACE_BUFFER_LEN; ++n) {
    struct gtm tp;
    filltm(&traceBuffer[n].time, &tp);
    TRACE_INFO_WP("%02d  ", n);
    TRACE_INFO_WP("%4d-%02d-%02d,%02d:%02d:%02d.%02d0", tp.tm_year+1900, tp.tm_mon+1, tp.tm_mday, tp.tm_hour, tp.tm_min, tp.tm_sec, traceBuffer[n].time_ms);
    TRACE("  %03d    0x%08x", traceBuffer[n].event, traceBuffer[n].data);  
    if (traceBuffer[n].time == 0 && traceBuffer[n].time_ms == 0) break;
    if ((n % 5) == 0) {
      while (!serial2TxFifo.empty()) { 
        CoTickDelay(1);
      }
    }
  }
  TRACE("End of Trace Buffer dump");
}
#endif
