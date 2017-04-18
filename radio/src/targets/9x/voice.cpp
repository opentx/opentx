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

struct t_voice Voice ;

void pushPrompt(uint16_t value)
{
#ifdef SIMU
  TRACE("playFile(\"%04d.ad4\")", value);
#endif

  struct t_voice *vptr;
  vptr = voiceaddress();

  if (vptr->VoiceQueueCount < VOICE_Q_LENGTH) {
    vptr->VoiceQueue[vptr->VoiceQueueInIndex++] = value;
    vptr->VoiceQueueInIndex &= (VOICE_Q_LENGTH - 1);
    vptr->VoiceQueueCount += 1;
  }
}

void pushCustomPrompt(uint8_t value)
{
  pushPrompt(PROMPT_CUSTOM_BASE + value);
}

void pushNumberPrompt(uint8_t value)
{
  pushPrompt(PROMPT_I18N_BASE + value);
}

struct t_voice *voiceaddress()
{
  return &Voice ;
}

void t_voice::voice_process(void)
{
  if (Backlight)
    VoiceLatch |= BACKLIGHT_BIT;
  else
    VoiceLatch &= ~BACKLIGHT_BIT;

  if (VoiceState == V_IDLE) {
    PORTB |= (1 << OUT_B_LIGHT); // Latch clock high
    if (VoiceQueueCount) {
      VoiceSerial = VoiceQueue[VoiceQueueOutIndex++];
      VoiceQueueOutIndex &= (VOICE_Q_LENGTH - 1);
      VoiceQueueCount -= 1;
      VoiceTimer = 17;
      if (VoiceSerial & 0x8000) { // Looking for sound volume 1F0-1F7
        VoiceTimer = 40;
      }
      VoiceLatch &= ~VOICE_CLOCK_BIT & ~VOICE_DATA_BIT ;
      if (VoiceSerial & 0x8000) {
        VoiceLatch |= VOICE_DATA_BIT;
      }
      PORTA_LCD_DAT = VoiceLatch ;                    // Latch data set
      PORTB &= ~(1<<OUT_B_LIGHT) ;                    // Latch clock low
      VoiceCounter = 31;
      VoiceState = V_CLOCKING;
    }
    else {
      PORTA_LCD_DAT = VoiceLatch; // Latch data set
      PORTB &= ~(1 << OUT_B_LIGHT); // Latch clock low
    }
  }
  else if (VoiceState == V_STARTUP) {
    PORTB |= (1<<OUT_B_LIGHT) ;      // Latch clock high
    VoiceLatch |= VOICE_CLOCK_BIT | VOICE_DATA_BIT ;
    PORTA_LCD_DAT = VoiceLatch ;     // Latch data set
    if (g_blinkTmr10ms > 60) { // Give module 1.4 secs to initialise
      VoiceState = V_WAIT_START_BUSY_OFF ;
    }
    PORTB &= ~(1<<OUT_B_LIGHT) ;     // Latch clock low
  }
  else if (VoiceState != V_CLOCKING) {
    uint8_t busy;
    PORTA_LCD_DAT = VoiceLatch; // Latch data set
    PORTB |= (1 << OUT_B_LIGHT); // Drive high,pullup enabled
    DDRB &= ~(1 << OUT_B_LIGHT); // Change to input
    // delay to allow input to settle
    asm(" rjmp 1f");
    asm("1:");
    asm(" nop");
    asm(" rjmp 1f");
    asm("1:");
    busy = PINB & 0x80;
    DDRB |= (1 << OUT_B_LIGHT); // Change to output
    // The next bit guarantees the backlight output gets clocked out
    if (VoiceState == V_WAIT_BUSY_ON) { // check for busy processing here
      if (busy == 0) { // Busy is active
        VoiceState = V_WAIT_BUSY_OFF;
      }
      else {
        if (--VoiceTimer == 0) {
          VoiceState = V_WAIT_BUSY_OFF;
        }
      }
    }
    else if (VoiceState == V_WAIT_BUSY_OFF) { // check for busy processing here
      if (busy) { // Busy is inactive
        VoiceTimer = 3;
        VoiceState = V_WAIT_BUSY_DELAY;
      }
    }
    else if (VoiceState == V_WAIT_BUSY_DELAY) {
      if (--VoiceTimer == 0) {
        VoiceState = V_IDLE;
      }
    }
    else if (VoiceState == V_WAIT_START_BUSY_OFF) {   // check for busy processing here
      if (busy) {                                     // Busy is inactive
        VoiceTimer = 20 ;
        VoiceState = V_WAIT_BUSY_DELAY ;
      }
    }
    PORTB &= ~(1 << OUT_B_LIGHT); // Latch clock low
  }
}
