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

uint16_t eeprom_pointer;
const char* eeprom_buffer_data;
volatile int8_t eeprom_buffer_size = 0;

#if not defined(SIMU) and not defined(PCBARM)

inline void eeprom_write_byte()
{
  EEAR = eeprom_pointer;
  EEDR = *eeprom_buffer_data;
#if defined (PCBV4)
  EECR |= 1<<EEMPE;
  EECR |= 1<<EEPE;
#else
  EECR |= 1<<EEMWE;
  EECR |= 1<<EEWE;
#endif
  eeprom_pointer++;
  eeprom_buffer_data++;
}

ISR(EE_READY_vect)
{
  if (--eeprom_buffer_size > 0) {
    eeprom_write_byte();
  }
  else {
#if defined (PCBV4)
    EECR &= ~(1<<EERIE);
#else
    EECR &= ~(1<<EERIE);
#endif
  }
}

#endif

void eeWriteBlockCmp(const void *i_pointer_ram, uint16_t i_pointer_eeprom, size_t size)
{
  assert(!eeprom_buffer_size);

  eeprom_pointer = i_pointer_eeprom;
  eeprom_buffer_data = (const char*)i_pointer_ram;
  eeprom_buffer_size = size+1;

#ifdef SIMU
  sem_post(&eeprom_write_sem);
#elif defined (PCBARM)

#elif defined (PCBV4)
  EECR |= (1<<EERIE);
#else
  EECR |= (1<<EERIE);
#endif

  if (s_sync_write) {
    while (eeprom_buffer_size > 0) wdt_reset();
  }
}

static uint8_t s_evt;
void putEvent(uint8_t evt)
{
  s_evt = evt;
}
uint8_t getEvent()
{
  uint8_t evt = s_evt;
  s_evt=0;
  return evt;
}

Key keys[NUM_KEYS];
void Key::input(bool val, EnumKeys enuk)
{
  //  uint8_t old=m_vals;
  m_vals <<= 1;  if(val) m_vals |= 1; //portbit einschieben
  m_cnt++;

  if(m_state && m_vals==0){  //gerade eben sprung auf 0
    if(m_state!=KSTATE_KILLED) {
      putEvent(EVT_KEY_BREAK(enuk));
      if(!( m_state == 16 && m_cnt<16)){
        m_dblcnt=0;
      }
        //      }
    }
    m_cnt   = 0;
    m_state = KSTATE_OFF;
  }
  switch(m_state){
    case KSTATE_OFF:
      if(m_vals==FFVAL){ //gerade eben sprung auf ff
        m_state = KSTATE_START;
        if(m_cnt>16) m_dblcnt=0; //pause zu lang fuer double
        m_cnt   = 0;
      }
      break;
      //fallthrough
    case KSTATE_START:
      putEvent(EVT_KEY_FIRST(enuk));
      inacCounter = 0;
      m_dblcnt++;
      m_state   = KSTATE_RPTDELAY;
      m_cnt     = 0;
      break;

    case KSTATE_RPTDELAY: // gruvin: delay state before first key repeat
      if(m_cnt == 24) putEvent(EVT_KEY_LONG(enuk)); 
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
      //fallthrough
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
  event=event & EVT_KEY_MASK;
  if(event < (int)DIM(keys))  keys[event].pauseEvents();
}
void killEvents(uint8_t event)
{
  event=event & EVT_KEY_MASK;
  if(event < (int)DIM(keys))  keys[event].killEvents();
}

//uint16_t g_anaIns[8];
volatile uint16_t g_tmr10ms;
volatile uint8_t  g_blinkTmr10ms;


#if defined (PCBV4)
uint8_t g_ms100 = 0; // global to allow time set function to reset to zero
#endif
void per10ms()
{
  g_tmr10ms++;
  g_blinkTmr10ms++;

#if defined (PCBV4)
  /* Update gloabal Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100)
  {
    g_unixTime++; // inc global unix timestamp one second
    g_ms100 = 0;
  }
#endif

  readKeysAndTrims();

#ifdef MAVLINK
  check_mavlink() ;
#endif

#if defined (FRSKY)

  // TODO everything here in check_frsky() ;
  // TODO it would be better in frsky.h / .cpp!

  // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
  static uint8_t FrskyDelay = 5;
  if (FrskyAlarmSendState && (--FrskyDelay == 0))
  {
    FrskyDelay = 5; // 50ms
    FRSKY10mspoll();
  }

#ifndef SIMU
  if (frskyUsrStreaming > 0)
    frskyUsrStreaming--;
  if (frskyStreaming > 0) {
    frskyStreaming--;
  }
  else if (g_eeGeneral.enableTelemetryAlarm && (g_model.frsky.channels[0].ratio || g_model.frsky.channels[1].ratio)) {
#if defined (AUDIO)
    if (!(g_tmr10ms % 30)) {
      audioDefevent(AU_WARNING1);
    }
#else
    if (!(g_tmr10ms % 30)) {
      warble = !(g_tmr10ms % 60);
      AUDIO_WARNING2();
    }
#endif
  }
#endif
#endif

  // These moved here from perOut() to improve beep trigger reliability.
  if(mixWarning & 1) if(((g_tmr10ms&0xFF)==  0)) AUDIO_MIX_WARNING_1();
  if(mixWarning & 2) if(((g_tmr10ms&0xFF)== 64) || ((g_tmr10ms&0xFF)== 72)) AUDIO_MIX_WARNING_2();
  if(mixWarning & 4) if(((g_tmr10ms&0xFF)==128) || ((g_tmr10ms&0xFF)==136) || ((g_tmr10ms&0xFF)==144)) AUDIO_MIX_WARNING_3();

#if defined(FRSKY_HUB) or defined(WS_HOW_HIGH)
  static uint16_t s_varioTmr = 0;

  if (isFunctionActive(FUNC_VARIO)) {
#if defined(AUDIO)
    uint8_t warble = 0;
#endif
    int8_t verticalSpeed = limit((int16_t)-100, (int16_t)(frskyHubData.varioSpeed/10), (int16_t)+100);

    uint16_t interval;
    if (verticalSpeed == 0) {
      interval = 300;
    }
    else {
      if (verticalSpeed < 0) {
        verticalSpeed = -verticalSpeed;
        warble = 1;
      }
      interval = (uint8_t)200 / verticalSpeed;
    }
    if (g_tmr10ms - s_varioTmr > interval) {
      s_varioTmr = g_tmr10ms;
      if (warble)
        AUDIO_VARIO_DOWN();
      else
        AUDIO_VARIO_UP();
    }
  }
#endif
}
