/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <inttypes.h>
#include "rtc.h"
#include "dump.h"
#if defined(CLI)
#include "cli.h"
#elif defined(CPUARM)
#include "serial.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(SIMU)
typedef void (*traceCallbackFunc)(const char * text);
extern traceCallbackFunc traceCallback;
void debugPrintf(const char * format, ...);
#elif defined(DEBUG) && defined(CLI) && defined(USB_SERIAL)
#define debugPrintf(...) do { if (cliTracesEnabled) serialPrintf(__VA_ARGS__); } while(0)
#elif defined(DEBUG) && defined(CLI)
uint8_t serial2TracesEnabled();
#define debugPrintf(...) do { if (serial2TracesEnabled() && cliTracesEnabled) serialPrintf(__VA_ARGS__); } while(0)
#elif defined(DEBUG) && defined(CPUARM)
uint8_t serial2TracesEnabled();
#define debugPrintf(...) do { if (serial2TracesEnabled()) serialPrintf(__VA_ARGS__); } while(0)
#else
#define debugPrintf(...)
#endif

#if defined(__cplusplus)
}
#endif

#define TRACE_PING(x)         do { debugPrintf(x); } while(0)
#define TRACE(...)            do { debugPrintf(__VA_ARGS__); debugPrintf("\r\n"); } while(0)
#define DUMP(data, size)      dump(data, size)
#define TRACE_DEBUG(...)      debugPrintf("-D- " __VA_ARGS__)
#define TRACE_DEBUG_WP(...)   debugPrintf(__VA_ARGS__)
#define TRACE_INFO(...)       debugPrintf("-I- " __VA_ARGS__)
#define TRACE_INFO_WP(...)    debugPrintf(__VA_ARGS__)
#define TRACE_WARNING(...)    debugPrintf("-W- " __VA_ARGS__)
#define TRACE_WARNING_WP(...) debugPrintf(__VA_ARGS__)
#define TRACE_ERROR(...)      debugPrintf("-E- " __VA_ARGS__)

#if defined(DEBUG) && !defined(SIMU)
#define TIME_MEASURE_START(id) uint16_t t0 ## id = getTmr2MHz()
#define TIME_MEASURE_STOP(id)  TRACE("Measure(" # id ") = %.1fus", float((uint16_t)(getTmr2MHz() - t0 ## id))/2)
#else
#define TIME_MEASURE_START(id)
#define TIME_MEASURE_STOP(id)
#endif

#if defined(DEBUG_TRACE_BUFFER)

#define TRACE_BUFFER_LEN  50

enum TraceEvent {
  trace_start = 1,

  sd_wait_ready = 10,
  sd_rcvr_datablock,
  sd_xmit_datablock_wait_ready,
  sd_xmit_datablock_rcvr_spi,
  sd_send_cmd_wait_ready,
  sd_send_cmd_rcvr_spi,

  sd_SD_ReadSectors = 16,
  sd_disk_read,
  sd_SD_WriteSectors,
  sd_disk_write,

  sd_disk_ioctl_CTRL_SYNC = 20,
  sd_disk_ioctl_GET_SECTOR_COUNT,
  sd_disk_ioctl_MMC_GET_CSD,
  sd_disk_ioctl_MMC_GET_CID,
  sd_disk_ioctl_MMC_GET_OCR,
  sd_disk_ioctl_MMC_GET_SDSTAT_1,
  sd_disk_ioctl_MMC_GET_SDSTAT_2,
  sd_spi_reset,

  ff_f_write_validate = 30,
  ff_f_write_flag,
  ff_f_write_clst,
  ff_f_write_sync_window,
  ff_f_write_disk_write_dirty,
  ff_f_write_clust2sect,
  ff_f_write_disk_write,
  ff_f_write_disk_read,
  ff_f_write_move_window,

  audio_getNextFilledBuffer_skip = 50,
};

struct TraceElement {
  gtime_t time;
  uint8_t time_ms;
  uint8_t event;
  uint32_t data;
};

void trace_event(enum TraceEvent event, uint32_t data);
void trace_event_i(enum TraceEvent event, uint32_t data);
const struct TraceElement * getTraceElement(uint16_t idx);
void dumpTraceBuffer();

#define TRACE_EVENT(condition, event, data)   if (condition) { trace_event(event, data); }
#define TRACEI_EVENT(condition, event, data)  if (condition) { trace_event_i(event, data); }

#else  // #if defined(DEBUG_TRACE_BUFFER)

#define TRACE_EVENT(condition, event, data)  
#define TRACEI_EVENT(condition, event, data)  

#endif // #if defined(DEBUG_TRACE_BUFFER)

#if defined(TRACE_SD_CARD)
  #define TRACE_SD_CARD_EVENT(condition, event, data)  TRACE_EVENT(condition, event, data)
#else
  #define TRACE_SD_CARD_EVENT(condition, event, data)  
#endif
#if defined(TRACE_FATFS)
  #define TRACE_FATFS_EVENT(condition, event, data)  TRACE_EVENT(condition, event, data)
#else
  #define TRACE_FATFS_EVENT(condition, event, data)  
#endif
#if defined(TRACE_AUDIO)
  #define TRACE_AUDIO_EVENT(condition, event, data)  TRACE_EVENT(condition, event, data)
  #define TRACEI_AUDIO_EVENT(condition, event, data) TRACEI_EVENT(condition, event, data)
#else
  #define TRACE_AUDIO_EVENT(condition, event, data)  
  #define TRACEI_AUDIO_EVENT(condition, event, data)  
#endif


#if defined(JITTER_MEASURE)  && defined(__cplusplus)

template<class T> class JitterMeter {
public:
  T min;
  T max;
  T measured;

  JitterMeter() : min(~(T)0), max(0), measured(0) {};

  void reset() { 
    // store mesaurement
    measured = max - min;
    //reset - begin new measurement
    min = ~(T)0; 
    max = 0; 
  };

  void measure(T value) { 
    if (value > max) max = value;
    if (value < min) min = value;
  };

  T get() const { 
    return measured; 
  };
};

#endif  // defined(JITTER_MEASURE)

#endif // _DEBUG_H_

