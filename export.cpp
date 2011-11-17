/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * Original contributors
 * - Philip Moss Adapted first frsky functions from jeti.cpp code by
 * - Karl Szmutny <shadow@privy.de>

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#include "simpgmspace.h"
#include "lcd.h"
#include "gruvin9x.h"
#include "menus.h"

int16_t g_anas[NUM_STICKS+NUM_POTS];
uint8_t *g_switches[MAX_PSWITCH];
extern uint8_t eeprom[EESIZE];

extern "C" void initGruvin9x(uint8_t *_eeprom)
{
  eepromFile = NULL; // in memory
  memcpy(eeprom, _eeprom, sizeof(eeprom));

  g_menuStack[0] = menuMainView;
  g_menuStack[1] = menuProcModelSelect;
  eeReadAll(); //load general setup and selected model
  // TODO
  //checkLowEEPROM(); //enough eeprom free?
  //checkTHR();
  //checkSwitches(); //must be last
  //state = 2;
}

extern "C" void runGruvin9x(int16_t *anas, uint8_t *switches, int16_t *chans, uint8_t *display)
{
  memcpy(g_anas, anas, sizeof(uint16_t) * NUM_STICKS+NUM_POTS);
  memcpy(g_switches, switches, sizeof(uint8_t) * MAX_PSWITCH);

  g_tmr10ms++;
  perMain();

  memcpy(chans, g_chans512, sizeof(g_chans512));
  memcpy(display, displayBuf, sizeof(displayBuf));
}

uint16_t anaIn(uint8_t chan)
{
  // printf("anaIn(%d) => %d\n", chan, g_anas[chan]); fflush(stdout);
  return g_anas[chan];
}
