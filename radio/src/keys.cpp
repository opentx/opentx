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

evt_t s_evt;
struct t_inactivity inactivity = {0};

#if defined(CPUARM)
evt_t getEvent(bool trim)
{
  evt_t evt = s_evt;
  int8_t k = EVT_KEY_MASK(s_evt) - TRM_BASE;
  bool trim_evt = (k>=0 && k<TRM_LAST-TRM_BASE+1);

  if (trim == trim_evt) {
    s_evt = 0;
    return evt;
  }
  else {
    return 0;
  }
}
#else
evt_t getEvent()
{
  evt_t evt = s_evt;
  s_evt = 0;
  return evt;
}
#endif

#define KEY_LONG_DELAY 32

Key keys[NUM_KEYS];
void Key::input(bool val)
{
  uint8_t t_vals = m_vals ;
  t_vals <<= 1 ;
  if (val) t_vals |= 1;
  m_vals = t_vals ;

  m_cnt++;

  if (m_state && m_vals==0) {
    if (m_state != KSTATE_KILLED) {
      putEvent(EVT_KEY_BREAK(key()));
    }
    m_cnt   = 0;
    m_state = KSTATE_OFF;
  }
  switch (m_state) {
    case KSTATE_OFF:
      if (m_vals == FFVAL) {
        m_state = KSTATE_START;
        m_cnt   = 0;
      }
      break;
    case KSTATE_START:
      putEvent(EVT_KEY_FIRST(key()));
      inactivity.counter = 0;
      m_state   = KSTATE_RPTDELAY;
      m_cnt     = 0;
      break;

    case KSTATE_RPTDELAY: // gruvin: delay state before first key repeat
      if (m_cnt == KEY_LONG_DELAY) {
        putEvent(EVT_KEY_LONG(key()));
      }
      if (m_cnt == 40) {
        m_state = 16;
        m_cnt = 0;
      }
      break;

    case 16:
    case 8:
    case 4:
    case 2:
      if (m_cnt >= 48)  { //3 6 12 24 48 pulses in every 480ms
        m_state >>= 1;
        m_cnt     = 0;
      }
      // no break
    case 1:
      if ((m_cnt & (m_state-1)) == 0) {
        putEvent(EVT_KEY_REPT(key()));
      }
      break;

    case KSTATE_PAUSE: //pause
      if (m_cnt >= 64)      {
        m_state = 8;
        m_cnt   = 0;
      }
      break;

    case KSTATE_KILLED: //killed
      break;
  }
}

EnumKeys Key::key() const 
{ 
  return static_cast<EnumKeys>(this - keys);
}

void pauseEvents(uint8_t event)
{
  event = EVT_KEY_MASK(event);
  if (event < (int)DIM(keys)) keys[event].pauseEvents();
}

void killEvents(uint8_t event)
{
#if defined(ROTARY_ENCODER_NAVIGATION)
  if (event == EVT_ROTARY_LONG) {
    killEvents(BTN_REa + NAVIGATION_RE_IDX());
  }
  else
#endif
  {
    event = EVT_KEY_MASK(event);
    if (event < (int)DIM(keys)) keys[event].killEvents();
  }
}

#if defined(CPUARM)
bool clearKeyEvents()
{
#if defined(PCBSKY9X)
  CoTickDelay(100);  // 200ms
#endif

  // loop until all keys are up
#if !defined(BOOT)
  tmr10ms_t start = get_tmr10ms();
#endif

  while (keyDown()) {

#if defined(SIMU)
    SIMU_SLEEP_NORET(1/*ms*/);
#else
    wdt_reset();
#endif

#if !defined(BOOT)
    if ((get_tmr10ms() - start) >= 300) {  // wait no more than 3 seconds
      //timeout expired, at least one key stuck
      return false;
    }
#endif
  }

  memclear(keys, sizeof(keys));
  putEvent(0);
  return true;
}
#else    // #if defined(CPUARM)
void clearKeyEvents()
{
  // loop until all keys are up
  while (keyDown()) {

#if defined(SIMU)
    SIMU_SLEEP(1/*ms*/);
#else
    wdt_reset();
#endif

#if defined(PCBSTD) && defined(ROTARY_ENCODER_NAVIGATION) && !defined(TELEMETREZ)
    rotencPoll();
#endif
  }

  memclear(keys, sizeof(keys));
  putEvent(0);
}
#endif   // #if defined(CPUARM)


