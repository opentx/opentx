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

// driver adapted from somo14d driver for gruvin9x board for driving WTV20SDMini module

#include "../../opentx.h"

// Start and stop bits need to be 2ms in duration. Start bit is low, stop bit is high
#define WTV20SD_STOP_TIME    68 // This is the needed 2ms (4) + 30ms (60) to allow for the
                                // point at which the busy flag is checkable + 2ms for saftey (4)
#define WTV20SD_START_TIME   5  // The 2ms of a stop/start bit

enum WTV20SD_State
{
  PAUSE,
  SENDSTART,
  SENDDATA,
  SENDSTOP
};

#define QUEUE_LENGTH 10
uint16_t WTV20SD_playlist[QUEUE_LENGTH] = {0};
volatile uint8_t WTV20SD_InputIndex = 0;
uint8_t WTV20SD_PlayIndex = 0;
uint8_t Startstop = WTV20SD_START_TIME;
uint16_t WTV20SD_current = 0;
uint8_t state = PAUSE;

void WTV20SD_sendstart()
{
  WTV20SD_Clock_off; // Start Bit, CLK low for 2ms

  --Startstop;

  if (!Startstop) state = SENDDATA;
}

void WTV20SD_senddata()
{
  static uint8_t i = 0;

  if (!WTV20SD_CLK) {
    // Only change data when the CLK is low
    if (WTV20SD_current & 0x8000) {
      WTV20SD_Data_on; // Data high
    }
    WTV20SD_current = (WTV20SD_current<<1);
    ++i;
    _delay_us(1); // Data setup delay
    WTV20SD_Clock_on; // CLK high
  }
  else {
    // Don't alter after sending last bit in preparation for sending stop bit
    WTV20SD_Clock_off; // CLK low
    WTV20SD_Data_off; // Data low
  }

  if (i == 16) { i = 0; Startstop = WTV20SD_STOP_TIME; state = SENDSTOP; }
}

void WTV20SD_sendstop()
{
  WTV20SD_Data_off; // Data low
  WTV20SD_Clock_on; // Stop Bit, CLK high for 2ms
  --Startstop;
	
  if (!Startstop && !WTV20SD_BUSY) state = PAUSE;
}

void pushPrompt(uint16_t prompt)
{
  // if mute active => no voice
  if (g_eeGeneral.beepMode == e_mode_quiet) return;
  
  /* Load playlist and activate interrupt */
  WTV20SD_playlist[WTV20SD_InputIndex] = prompt;
  ++WTV20SD_InputIndex;
  WTV20SD_InputIndex %= QUEUE_LENGTH;

  if (!isPlaying()) {
    TIMSK5 |= (1<<OCIE5A); // enable interrupts on Output Compare A Match
  }
}

uint8_t isPlaying()
{
  /* interrupts active on Output Compare A Match ? */
#if defined(SIMU)
  return false;
#else
  return (TIMSK5 & (1<<OCIE5A));
#endif
}

#if !defined(SIMU)
ISR(TIMER5_COMPA_vect) // every 0.5ms normally, every 2ms during startup reset
{
  static uint8_t reset_dly=4;
  static uint8_t reset_pause=150;

  if (reset_dly) {
    OCR5A=0x1f4;
    reset_dly--;
    WTV20SD_Reset_off;
    WTV20SD_Data_off;
    WTV20SD_Clock_on;
  } // RESET low
  else if (reset_pause) {
    OCR5A=0x1f4;
    reset_pause--;
    WTV20SD_Reset_on;
  } // RESET high
  else {
    OCR5A = 0x7d; // another 0.5ms

    if (state == PAUSE) {
      if (WTV20SD_PlayIndex == WTV20SD_InputIndex) {
        TIMSK5 &= ~(1<<OCIE5A); // stop reentrance
        TCNT5=0; // reset timer
        return; // nothing else to play
      }
      else {
        WTV20SD_current = WTV20SD_playlist[WTV20SD_PlayIndex];
        ++WTV20SD_PlayIndex;
        WTV20SD_PlayIndex %= QUEUE_LENGTH;
        Startstop = WTV20SD_START_TIME;
        state = SENDSTART;
      }
    } // end PAUSE

    if (state == SENDSTART) {
      WTV20SD_sendstart();
      return;
    }

    if (state == SENDDATA) {
      WTV20SD_senddata();
      return;
    }

    if (state == SENDSTOP) {
      WTV20SD_sendstop();
      return;
    }
  }
}
#endif
