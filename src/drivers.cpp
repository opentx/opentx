/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
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

#include "open9x.h"

static uint8_t s_evt;
void putEvent(uint8_t evt)
{
  s_evt = evt;
}

#if defined(PCBSTD)
uint8_t getEvent()
{
  uint8_t evt = s_evt;
  s_evt = 0;
  return evt;
}
#else
uint8_t getEvent(bool trim)
{
  uint8_t evt = s_evt;
  int8_t k = (s_evt & EVT_KEY_MASK) - TRM_BASE;
  bool trim_evt = (k>=0 && k<8);

  if (trim == trim_evt) {
    s_evt = 0;
    return evt;
  }
  else {
    return 0;
  }
}
#endif

#if defined(PCBSKY9X)
#define KEY_LONG_DELAY 32
#else
#define KEY_LONG_DELAY 24
#endif

Key keys[NUM_KEYS];
void Key::input(bool val, EnumKeys enuk)
{
  uint8_t t_vals = m_vals ;
  t_vals <<= 1 ;
  if (val) t_vals |= 1; //portbit einschieben
  m_vals = t_vals ;

  m_cnt++;

  if (m_state && m_vals==0) {  //gerade eben sprung auf 0
    if (m_state != KSTATE_KILLED) {
      putEvent(EVT_KEY_BREAK(enuk));
    }
    m_cnt   = 0;
    m_state = KSTATE_OFF;
  }
  switch(m_state){
    case KSTATE_OFF:
      if (m_vals == FFVAL) { //gerade eben sprung auf ff
        m_state = KSTATE_START;
        m_cnt   = 0;
      }
      break;
      //fallthrough
    case KSTATE_START:
      putEvent(EVT_KEY_FIRST(enuk));
      inacCounter = 0;
      m_state   = KSTATE_RPTDELAY;
      m_cnt     = 0;
      break;

    case KSTATE_RPTDELAY: // gruvin: delay state before first key repeat
      if(m_cnt == KEY_LONG_DELAY) putEvent(EVT_KEY_LONG(enuk));
      if (m_cnt == 40) {
        m_state = 16;
        m_cnt = 0;
      }
      break;

    case 16:
    case 8:
    case 4:
    case 2:
      if(m_cnt >= 48)  { //3 6 12 24 48 pulses in every 480ms
        m_state >>= 1;
        m_cnt     = 0;
      }
      // no break
    case 1:
      if( (m_cnt & (m_state-1)) == 0)  putEvent(EVT_KEY_REPT(enuk));
      break;

    case KSTATE_PAUSE: //pause
      if(m_cnt >= 64)      {
        m_state = 8;
        m_cnt   = 0;
      }
      break;

    case KSTATE_KILLED: //killed
      break;
  }
}

void pauseEvents(uint8_t event)
{
  event = event & EVT_KEY_MASK;
  if (event < (int)DIM(keys)) keys[event].pauseEvents();
}

void killEvents(uint8_t event)
{
  event = event & EVT_KEY_MASK;
  if (event < (int)DIM(keys)) keys[event].killEvents();
}

volatile tmr10ms_t g_tmr10ms;
volatile uint8_t   g_blinkTmr10ms;

void per10ms()
{
  g_tmr10ms++;
  g_blinkTmr10ms++;
  
  if (s_noHi) s_noHi--;
  if (trimsCheckTimer) trimsCheckTimer --;

#if defined (PCBSKY9X)
  Tenms |= 1 ;                    // 10 mS has passed
#endif

#if defined(RTCLOCK)
  /* Update global Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100) {
    g_rtcTime++;   // inc global unix timestamp one second
#if defined (PCBSKY9X)
    if (g_rtcTime < 120) { 
      rtc_init();
    } else {
      read_coprocessor(true);
    }
#endif    
    g_ms100 = 0;
  }
#endif

  readKeysAndTrims();

#if defined(MAVLINK) && !defined(PCBSKY9X)
  check_mavlink();
#endif

#if defined (FRSKY) && !defined(PCBSKY9X) && !(defined(PCBSTD) && (defined(AUDIO) || defined(VOICE)))
  check_frsky();
#endif

  // These moved here from perOut() to improve beep trigger reliability.
  if(mixWarning & 1) if(((g_tmr10ms&0xFF)==  0)) AUDIO_MIX_WARNING_1();
  if(mixWarning & 2) if(((g_tmr10ms&0xFF)== 64) || ((g_tmr10ms&0xFF)== 72)) AUDIO_MIX_WARNING_2();
  if(mixWarning & 4) if(((g_tmr10ms&0xFF)==128) || ((g_tmr10ms&0xFF)==136) || ((g_tmr10ms&0xFF)==144)) AUDIO_MIX_WARNING_3();

}
