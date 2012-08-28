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

#include "../open9x.h"

struct t_voice Voice ;
/* TODO why? static uint8_t v_ctr ;
uint8_t v_first[8] ; */

/*void putVoiceQueueUpper( uint8_t value )
{
  struct t_voice *vptr;
  vptr = voiceaddress();

  if (vptr->VoiceQueueCount < VOICE_Q_LENGTH - 1) {
    pushPrompt(0xFF);
    pushPrompt(value);
  }
}*/

void pushCustomPrompt(uint8_t value)
{
  struct t_voice *vptr ;
  vptr = voiceaddress() ;

  if (vptr->VoiceQueueCount < VOICE_Q_LENGTH-1)
  {
    pushPrompt( 0xFF ) ;
    pushPrompt( value ) ;
  }
}

void pushPrompt(uint8_t value)
{
#ifdef SIMU
  printf("playFile(\"%04d.ad4\")\n", value); fflush(stdout);
#endif

  struct t_voice *vptr;
  vptr = voiceaddress();

  if (vptr->VoiceQueueCount < VOICE_Q_LENGTH) {
    vptr->VoiceQueue[vptr->VoiceQueueInIndex++] = value;
    vptr->VoiceQueueInIndex &= (VOICE_Q_LENGTH - 1);
    vptr->VoiceQueueCount += 1;
  }
  /* TODO why?
  if (v_ctr < 8) {
    v_first[v_ctr] = value;
    v_ctr += 1;
  } */
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
      VoiceLatch &= ~VOICE_CLOCK_BIT;
      PORTA_LCD_DAT = VoiceLatch; // Latch data set
      PORTB &= ~(1 << OUT_B_LIGHT); // Latch clock low
      VoiceSerial = VoiceQueue[VoiceQueueOutIndex++];
      VoiceQueueOutIndex &= (VOICE_Q_LENGTH - 1);
      VoiceQueueCount -= 1;
      if (VoiceShift) {
        VoiceShift = 0;
        VoiceSerial += 256;
      }
      VoiceTimer = 16;
      if (VoiceSerial == 0xFF) { // Looking for Shift FF
          VoiceShift = 1;
          return;
        }
      if ((VoiceSerial & 0x1F8) == 0x1F0) { // Looking for sound volume 1F0-1F7
        VoiceSerial |= 0xFF00;
        VoiceTimer = 40;
      }
      VoiceCounter = 31;
      VoiceState = V_CLOCKING;
    }
    else {
      PORTA_LCD_DAT = VoiceLatch; // Latch data set
      PORTB &= ~(1 << OUT_B_LIGHT); // Latch clock low
    }
  }
  else if (VoiceState == V_STARTUP) {
    if (g_blinkTmr10ms > 200) // Give module 2 secs to initialise
    {
      VoiceState = V_IDLE;
    }
  }
  else if (VoiceState != V_CLOCKING) {
    uint8_t busy;
    PORTA_LCD_DAT = VoiceLatch; // Latch data set
    PORTB |= (1 << OUT_B_LIGHT); // Drive high,pullup enabled
    DDRB &= ~(1 << OUT_B_LIGHT); // Change to input
#if !defined(SIMU)
    asm(" nop");
    asm(" nop");
#endif
    // delay to allow input to settle
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
        VoiceTimer = 2;
        VoiceState = V_WAIT_BUSY_DELAY;
      }
    }
    else if (VoiceState == V_WAIT_BUSY_DELAY) {
      if (--VoiceTimer == 0) {
        VoiceState = V_IDLE;
      }
    }
    PORTB &= ~(1 << OUT_B_LIGHT); // Latch clock low
  }
}
