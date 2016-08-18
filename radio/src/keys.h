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

#ifndef _KEYS_H_
#define _KEYS_H_

#define EVT_KEY_MASK(e)      ((e) & 0x1f)

#if defined(PCBHORUS)
#define _MSK_KEY_BREAK       0x0200
#define _MSK_KEY_REPT        0x0400
#define _MSK_KEY_FIRST       0x0600
#define _MSK_KEY_LONG        0x0800
#define _MSK_KEY_FLAGS       0x0e00
#define EVT_ENTRY            0x1000
#define EVT_ENTRY_UP         0x2000
#else
#define _MSK_KEY_BREAK       0x20
#define _MSK_KEY_REPT        0x40
#define _MSK_KEY_FIRST       0x60
#define _MSK_KEY_LONG        0x80
#define _MSK_KEY_FLAGS       0xe0
#define EVT_ENTRY            0xbf
#define EVT_ENTRY_UP         0xbe
#endif

#define EVT_KEY_BREAK(key)   ((key)|_MSK_KEY_BREAK)
#define EVT_KEY_FIRST(key)   ((key)|_MSK_KEY_FIRST)
#define EVT_KEY_REPT(key)    ((key)|_MSK_KEY_REPT)
#define EVT_KEY_LONG(key)    ((key)|_MSK_KEY_LONG)

#define IS_KEY_BREAK(evt)    (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_BREAK)
#define IS_KEY_FIRST(evt)    (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_FIRST)
#define IS_KEY_LONG(evt)     (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_LONG)
#define IS_KEY_REPT(evt)     (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_REPT)

#if defined(PCBHORUS)
  #define EVT_ROTARY_BREAK   EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG    EVT_KEY_LONG(KEY_ENTER)
  #define EVT_ROTARY_LEFT    0xDF00
  #define EVT_ROTARY_RIGHT   0xDE00
#elif defined(PCBTARANIS) || defined(PCBFLAMENCO)
  #define EVT_ROTARY_BREAK   EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG    EVT_KEY_LONG(KEY_ENTER)
#else
  #define EVT_ROTARY_BREAK   0xcf
  #define EVT_ROTARY_LONG    0xce
  #define EVT_ROTARY_LEFT    0xdf
  #define EVT_ROTARY_RIGHT   0xde
#endif

#if defined(COLORLCD)
  #define EVT_REFRESH        0xDD00
#endif

class Key
{
#define FILTERBITS      4

#ifdef SIMU
  #define FFVAL 1
#else
  #define FFVAL          ((1<<FILTERBITS)-1)
#endif

#define KSTATE_OFF      0
#define KSTATE_RPTDELAY 95 // gruvin: delay state before key repeating starts
#define KSTATE_START    97
#define KSTATE_PAUSE    98
#define KSTATE_KILLED   99

  private:
    uint8_t m_vals;   // key debounce?  4 = 40ms
    uint8_t m_cnt;
    uint8_t m_state;
  public:
    void input(bool val);
    bool state()       { return m_vals > 0; }
    void pauseEvents() { m_state = KSTATE_PAUSE; m_cnt = 0; }
    void killEvents()  { m_state = KSTATE_KILLED; }
    uint8_t key() const;
};

extern Key keys[NUM_KEYS];

#if defined(COLORLCD)
typedef uint16_t event_t;
#else
typedef uint8_t event_t;
#endif

extern event_t s_evt;

#define putEvent(evt) s_evt = evt

void pauseEvents(uint8_t index);
void killEvents(uint8_t index);

#if defined(CPUARM)
  bool clearKeyEvents();
  event_t getEvent(bool trim=false);
#else
  void clearKeyEvents();
  event_t getEvent();
#endif

uint8_t keyDown();

#endif // _KEYS_H_
