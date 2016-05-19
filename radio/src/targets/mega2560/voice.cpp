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


#include "../../opentx.h"

// Bracame : Driver rewrited for driving WTV20SDMini module

// Start and stop bits need to be 2ms in duration. Start bit is low, stop bit is high
#define SOMOSTOP_TIME    68 //This is the needed 2ms (4) + 30ms (60) to allow for the 
                       //point at which the busy flag is checkable + 2ms for saftey (4)
#define SOMOSTART_TIME   5 //The 2ms of a stop/start bit

enum Somo14State
	{
	PAUSE,
	SENDSTART,
	SENDDATA,
	SENDSTOP
	};

#define QUEUE_LENGTH 10
uint16_t Somo14playlist[QUEUE_LENGTH] = {0};
volatile uint8_t SomoInputIndex = 0;
uint8_t SomoPlayIndex = 0;
uint8_t Startstop = SOMOSTART_TIME;
uint16_t Somo14_current = 0;
uint8_t state = PAUSE;

	
#ifdef SIMU
#define SOMOBUSY busy
#else
#define BUSY
#endif


void somo14_sendstart()
{
    SOMOClock_off; // Start Bit, CLK low for 2ms

    --Startstop;
    
    if (!Startstop) state = SENDDATA;
	
}

void somo14_senddata()
{
    static uint8_t i = 0;
    
    if (!SOMOCLK) // Only change data when the CLK is low
    {
      
      if (Somo14_current & 0x8000)
      {
        SOMOData_on; // Data high
      }
      Somo14_current = (Somo14_current<<1);
      ++i;
      _delay_us(1); // Data setup delay
      SOMOClock_on; // CLK high
    }

    else // Don't alter after sending last bit in preparation for sending stop bit
    {
      SOMOClock_off; // CLK low
      SOMOData_off; // Data low
    }
	
	if (i == 16) { i = 0; Startstop = SOMOSTOP_TIME; state = SENDSTOP; }
}

void somo14_sendstop()
{
	SOMOData_off; // Data low
	SOMOClock_on; // Stop Bit, CLK high for 2ms
	--Startstop;
	
    if (!Startstop && !SOMOBUSY) state = PAUSE;
}

void pushPrompt(uint16_t prompt)
{
  // if mute active => no voice
  if (g_eeGeneral.beepMode == e_mode_quiet) return;
  
  /* Load playlist and activate interrupt */
  Somo14playlist[SomoInputIndex] = prompt;
  ++SomoInputIndex;
  SomoInputIndex %= QUEUE_LENGTH;

  if (!isPlaying())
  {
  TIMSK5 |= (1<<OCIE5A); //enable interrupts on Output Compare A Match
  }
}

uint8_t isPlaying()
{
  /*interrupts active on Output Compare A Match ?*/
#if defined(SIMU)
  return false;
#else
  return (TIMSK5 & (1<<OCIE5A));
#endif
}

#ifndef SIMU

ISR(TIMER5_COMPA_vect) //Every 0.5ms normally, every 2ms during startup reset
{
    static uint8_t reset_dly=4;
    static uint8_t reset_pause=150;

  if (reset_dly) 
	{OCR5A=0x1f4; reset_dly--; SOMOReset_off; SOMOData_off; SOMOClock_on;} // RESET low
  else if (reset_pause)
	{OCR5A=0x1f4; reset_pause--; SOMOReset_on;} // RESET high
  else 
	{
	OCR5A = 0x7d; // another 0.5ms

	if (state == PAUSE)
	{
		if (SomoPlayIndex == SomoInputIndex)
		{
			TIMSK5 &= ~(1<<OCIE5A); // stop reentrance
			TCNT5=0;//reset timer				
			return; // Nothing else to play
		} 
		else
		{
			Somo14_current = Somo14playlist[SomoPlayIndex];
			++SomoPlayIndex;
			SomoPlayIndex %= QUEUE_LENGTH;
			Startstop = SOMOSTART_TIME;
			state = SENDSTART;
		}
	} // End PAUSE
	
	if (state == SENDSTART) {somo14_sendstart(); return;}

	if (state == SENDDATA) {somo14_senddata(); return;}

	if (state == SENDSTOP) {somo14_sendstop(); return;}
	
}
#endif
}
