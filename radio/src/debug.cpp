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

#include "opentx.h"
#include "stamp.h"
#include <stdarg.h>
#if defined(CPUARM)
  #include <OsConfig.h>
#endif
#if !defined(SIMU) && (defined(STM32F2) || defined(PCBSKY9X))
  #include "dwt.h"    // STM32F2 library and SKY9X SAM do not define DWT registers
#endif

#if defined(SIMU)
traceCallbackFunc traceCallback = 0;
#endif

#if defined(SIMU)
#define PRINTF_BUFFER_SIZE     1024
void debugPrintf(const char * format, ...)
{
  va_list arglist;
  char tmp[PRINTF_BUFFER_SIZE];

  va_start(arglist, format);
  vsnprintf(tmp, PRINTF_BUFFER_SIZE, format, arglist);
  va_end(arglist);
  fputs(tmp, stdout);
  fflush(stdout);
  if (traceCallback) {
    traceCallback(tmp);
  }
}
#endif

#if defined(DEBUG_TRACE_BUFFER)
static struct TraceElement traceBuffer[TRACE_BUFFER_LEN];
static uint8_t traceBufferPos;
extern Fifo<uint8_t, 512> serial2TxFifo;
gtime_t filltm(const gtime_t *t, struct gtm *tp);

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


void dumpTraceBuffer()
{
  TRACE("Dump of Trace Buffer (%s " DATE " " TIME "):", vers_stamp);
  TRACE("#   Time                     Event  Data");
  for(int n = 0; n < TRACE_BUFFER_LEN; ++n) {
    struct gtm tp;
    filltm(&traceBuffer[n].time, &tp);
    TRACE_NOCRLF("%02d  ", n);
    TRACE_NOCRLF("%4d-%02d-%02d,%02d:%02d:%02d.%02d0", tp.tm_year+TM_YEAR_BASE, tp.tm_mon+1, tp.tm_mday, tp.tm_hour, tp.tm_min, tp.tm_sec, traceBuffer[n].time_ms);
    TRACE("  %03d    0x%08x", traceBuffer[n].event, traceBuffer[n].data);
    if (traceBuffer[n].time == 0 && traceBuffer[n].time_ms == 0) break;
#if !defined(SIMU)
    if ((n % 5) == 0) {
      while (!serial2TxFifo.isEmpty()) {
        CoTickDelay(1);
      }
    }
#endif
  }
  TRACE("End of Trace Buffer dump");
}
#endif

#if defined(DEBUG_INTERRUPTS)

#if defined(PCBHORUS)
  const char * const interruptNames[INT_LAST] = {
    "Tick ",   // INT_TICK,
    "1ms  ",   // INT_1MS,
    "Ser2 ",   // INT_SER2,
    "TelDm",   // INT_TELEM_DMA,
    "TelUs",   // INT_TELEM_USART,
    "Sdio ",   // INT_SDIO,
    "SdDma",   // INT_SDIO_DMA,
    "D2S7 ",   // INT_DMA2S7,
    "Tim1 ",   // INT_TIM1CC,
    "Tim2 ",   // INT_TIM2,
    "Tim3 ",   // INT_TIM3,
    "BlueT",   // INT_BLUETOOTH,
    "USB  ",  // INT_OTG_FS,
#if defined(DEBUG_USB_INTERRUPTS)
    " spur",  // INT_OTG_FS_SPURIOUS,
    "  out",  // INT_OTG_FS_OUT_EP,
    "   in",  // INT_OTG_FS_IN_EP,
    " miss",  // INT_OTG_FS_MODEMISMATCH,
    " wake",  // INT_OTG_FS_WAKEUP,
    " susp",  // INT_OTG_FS_SUSPEND,
    "  sof",  // INT_OTG_FS_SOF,
    " rxst",  // INT_OTG_FS_RX_STAT,
    "  rst",  // INT_OTG_FS_RESET,
    " enum",  // INT_OTG_FS_ENUM,
    " inci",  // INT_OTG_FS_INCOMPLETE_IN,
    " inco",  // INT_OTG_FS_INCOMPLETE_OUT,
    " sess",  // INT_OTG_FS_SESSION,
    "  otg",  // INT_OTG_FS_OTG,
    " notd",  // INT_OTG_FS_RX_NOT_DEVICE,
#endif // #if defined(DEBUG_USB_INTERRUPTS)
  };
#elif defined(PCBTARANIS)
  const char * const interruptNames[INT_LAST] = {
    "Tick ",   // INT_TICK,
    "5ms  ",   // INT_5MS,
    "Audio",   // INT_AUDIO,
    "BlueT",   // INT_BLUETOOTH,
    "Lcd  ",   // INT_LCD,
    "T1CC ",   // INT_TIM1CC,
    "Tim1 ",   // INT_TIM1,
    "Tim8 ",   // INT_TIM8,
    "Ser2 ",   // INT_SER2,
    "TelDm",   // INT_TELEM_DMA,
    "TelUs",   // INT_TELEM_USART,
    "Train",   // INT_TRAINER,
    "Usb  ",   // INT_OTG_FS,
#if defined(DEBUG_USB_INTERRUPTS)
    " spur",  // INT_OTG_FS_SPURIOUS,
    "  out",  // INT_OTG_FS_OUT_EP,
    "   in",  // INT_OTG_FS_IN_EP,
    " miss",  // INT_OTG_FS_MODEMISMATCH,
    " wake",  // INT_OTG_FS_WAKEUP,
    " susp",  // INT_OTG_FS_SUSPEND,
    "  sof",  // INT_OTG_FS_SOF,
    " rxst",  // INT_OTG_FS_RX_STAT,
    "  rst",  // INT_OTG_FS_RESET,
    " enum",  // INT_OTG_FS_ENUM,
    " inci",  // INT_OTG_FS_INCOMPLETE_IN,
    " inco",  // INT_OTG_FS_INCOMPLETE_OUT,
    " sess",  // INT_OTG_FS_SESSION,
    "  otg",  // INT_OTG_FS_OTG,
    " notd",  // INT_OTG_FS_RX_NOT_DEVICE,
#endif // #if defined(DEBUG_USB_INTERRUPTS)
  };
#endif

struct InterruptCounters interruptCounters;
#endif //#if defined(DEBUG_INTERRUPTS)

#if defined(DEBUG_TASKS)

uint32_t taskSwitchLog[DEBUG_TASKS_LOG_SIZE] __SDRAM;
uint16_t taskSwitchLogPos;

/**
 *******************************************************************************
 * @brief      Hook for task switch logging
 * @param[in]  taskID Task which is now in RUNNING state
 * @retval     None
 *
 * @par Description
 * @details    This function logs the time when a task entered the RUNNING state.
 *******************************************************************************
 */
void CoTaskSwitchHook(uint8_t taskID)
{
  /* Log task switch here */
  taskSwitchLog[taskSwitchLogPos] = (taskID << 24) + ((uint32_t)CoGetOSTime() & 0xFFFFFF);
  if(++taskSwitchLogPos >= DEBUG_TASKS_LOG_SIZE) {
    taskSwitchLogPos = 0;
  }
}

#endif // #if defined(DEBUG_TASKS)

#if defined(CPUARM)
/*
   DebugTimer
*/

// TODO: Use RTOS abstraction layer once ready.
#define RTOS_GET_TICKS()         CoGetOSTime()
#define RTOS_TICKS_1US           (CFG_CPU_FREQ / CFG_SYSTICK_FREQ / (CFG_CPU_FREQ / 1000000))  // RTOS timer ticks per 1us
#ifdef SIMU
  #define SYSTEM_GET_TICKS()     simuTimerMicros()                // returns 1us increments
  #define SYSTEM_TICKS_1US       (1)
#else
  #define SYSTEM_GET_TICKS()     (DWT->CYCCNT)                    // use cycle counter
  #define SYSTEM_TICKS_1US       (CFG_CPU_FREQ / 1000000)         // number of system ticks in 1us
#endif
#define SYSTEM_TICKS_MAX_US      (0xFFFFFFFF / SYSTEM_TICKS_1US)  // longest possible time span using systicks (must fit into uint32)

void DebugTimer::start()
{
  start_loprec = RTOS_GET_TICKS();
  start_hiprec = SYSTEM_GET_TICKS();
}

debug_timer_t DebugTimer::stop()
{
  // The DWT cycle count timer is 32b with a maximum span depending on CPU frequency (eg. ~25.5s on F4 @ 168MHz, ~35.5s on F2 @ 120MHz).
  // If time difference is larger than that, then we use the lower-resolution 64-bit RTOS timer (currently 2ms per tick, ~99 days total)
  const uint32_t lpdlta = (RTOS_GET_TICKS() - start_loprec) * RTOS_TICKS_1US;
  if (lpdlta > SYSTEM_TICKS_MAX_US)
    last = lpdlta;
  else
    last = (SYSTEM_GET_TICKS() - start_hiprec) / SYSTEM_TICKS_1US;
  evalStats();
  return last;
}

void DebugTimer::evalStats()
{
  ++iter;
  ttl += last;
  if (min > last)
    min = last;
  if (max < last)
    max = last;
}
#endif  // defined(CPUARM)

#if defined(DEBUG_TIMERS)

DebugTimer debugTimers[DEBUG_TIMERS_COUNT];

const char * const debugTimerNames[DEBUG_TIMERS_COUNT] = {
   "Pulses int."   // debugTimerIntPulses,
  ,"Pulses dur."   // debugTimerIntPulsesDuration,
  ,"10ms dur.  "   // debugTimerPer10ms,
  ,"10ms period"   // debugTimerPer10msPeriod,
  ,"Rotary enc."   // debugTimerRotEnc,
  ,"Haptic     "   // debugTimerHaptic,
  ,"Mixer calc "   // debugTimerMixer,
  ,"Tel. wakeup"   // debugTimerTelemetryWakeup,
  ,"perMain dur"   // debugTimerPerMain,
  ," perMain s1"   // debugTimerPerMain1,
  ," guiMain   "   // debugTimerGuiMain,
  ,"  LUA bg   "   // debugTimerLuaBg,
  ,"  LCD wait "   // debugTimerLcdRefreshWait,
  ,"  LUA fg   "   // debugTimerLuaFg,
  ,"  LCD refr."   // debugTimerLcdRefresh,
  ,"  Menus    "   // debugTimerMenus,
  ,"   Menu hnd"   // debugTimerMenuHandlers,
  ,"Menu Vers. "   // debugTimerVersion,
  ,"Menu simple"   // debugTimerSimpleMenu,
  ,"Menu drawte"   // debugTimerDrawText,
  ,"Menu drawt1"   // debugTimerDrawText1,
  ,"Mix ADC    "   // debugTimerGetAdc,
  ,"Mix getsw  "   // debugTimerGetSwitches,
  ,"Mix eval   "   // debugTimerEvalMixes,
  ,"Mix 10ms   "   // debugTimerMixes10ms,
  ,"ADC read   "   // debugTimerAdcRead,
  ,"mix-pulses "   // debugTimerMixerCalcToUsage
  ,"mix-int.   "   // debugTimerMixerIterval
  ,"Audio int. "   // debugTimerAudioIterval
  ,"Audio dur. "   // debugTimerAudioDuration
  ," A. consume"   // debugTimerAudioConsume,

};

#endif
