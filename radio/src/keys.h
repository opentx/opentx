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

#define EVT_KEY_MASK(e)                ((e) & 0x1f)

#if defined(PCBHORUS)
#define _MSK_KEY_BREAK                 0x0200
#define _MSK_KEY_REPT                  0x0400
#define _MSK_KEY_FIRST                 0x0600
#define _MSK_KEY_LONG                  0x0800
#define _MSK_KEY_FLAGS                 0x0e00
#define EVT_ENTRY                      0x1000
#define EVT_ENTRY_UP                   0x2000
#else
#define _MSK_KEY_BREAK                 0x20
#define _MSK_KEY_REPT                  0x40
#define _MSK_KEY_FIRST                 0x60
#define _MSK_KEY_LONG                  0x80
#define _MSK_KEY_FLAGS                 0xe0
#define EVT_ENTRY                      0xbf
#define EVT_ENTRY_UP                   0xbe
#endif

// normal order of events is: FIRST, LONG, REPEAT, REPEAT, ..., BREAK
#define EVT_KEY_FIRST(key)             ((key)|_MSK_KEY_FIRST)  // fired when key is pressed
#define EVT_KEY_LONG(key)              ((key)|_MSK_KEY_LONG)   // fired when key is held pressed for a while
#define EVT_KEY_REPT(key)              ((key)|_MSK_KEY_REPT)   // fired when key is held pressed long enough, fires multiple times with increasing speed
#define EVT_KEY_BREAK(key)             ((key)|_MSK_KEY_BREAK)  // fired when key is released (short or long), but only if the event was not killed

#define IS_KEY_FIRST(evt)              (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_FIRST)
#define IS_KEY_LONG(evt)               (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_LONG)
#define IS_KEY_REPT(evt)               (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_REPT)
#define IS_KEY_BREAK(evt)              (((evt) & _MSK_KEY_FLAGS) == _MSK_KEY_BREAK)

#if defined(PCBXLITE)
  typedef uint16_t event_t;
  #define EVT_ROTARY_BREAK             EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG              EVT_KEY_LONG(KEY_ENTER)
  #define EVT_ROTARY_LEFT              0xDF00
  #define EVT_ROTARY_RIGHT             0xDE00
  #define IS_NEXT_EVENT(event)         (event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN))
  #define IS_PREVIOUS_EVENT(event)     (event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP))
#elif (defined(PCBHORUS) || defined(PCBTARANIS)) && defined(ROTARY_ENCODER_NAVIGATION)
  typedef uint16_t event_t;
  #define EVT_ROTARY_BREAK             EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG              EVT_KEY_LONG(KEY_ENTER)
  #define EVT_ROTARY_LEFT              0xDF00
  #define EVT_ROTARY_RIGHT             0xDE00
  #define IS_NEXT_EVENT(event)         (event==EVT_ROTARY_RIGHT)
  #define IS_PREVIOUS_EVENT(event)     (event==EVT_ROTARY_LEFT)
#elif defined(ROTARY_ENCODER_NAVIGATION)
  typedef uint8_t event_t;
  #define EVT_ROTARY_BREAK             0xcf
  #define EVT_ROTARY_LONG              0xce
  #define EVT_ROTARY_LEFT              0xdf
  #define EVT_ROTARY_RIGHT             0xde
  #define IS_NEXT_EVENT(event)         (event==EVT_ROTARY_RIGHT || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN))
  #define IS_PREVIOUS_EVENT(event)     (event==EVT_ROTARY_LEFT || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP))
#else
  typedef uint8_t event_t;
  #define IS_NEXT_EVENT(event)         (event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN))
  #define IS_PREVIOUS_EVENT(event)     (event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP))
#endif

#if defined(COLORLCD)
  #define EVT_REFRESH                  0xDD00
#endif

class Key
{
  private:
    uint8_t m_vals;
    uint8_t m_cnt;
    uint8_t m_state;
  public:
    void input(bool val);
    bool state() const { return m_vals > 0; }
    void pauseEvents();
    void killEvents();
    uint8_t key() const;
};

extern Key keys[NUM_KEYS];
extern event_t s_evt;

#define putEvent(evt) s_evt = evt

void pauseEvents(event_t event);
void killEvents(event_t event);

#if defined(CPUARM)
  bool clearKeyEvents();
  event_t getEvent(bool trim=false);
  bool keyDown();
#else
  void clearKeyEvents();
  event_t getEvent();
  uint8_t keyDown();
#endif



#endif // _KEYS_H_
