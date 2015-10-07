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

#include "../../opentx.h"

MenuFuncP g_menuStack[5];
evt_t menuEvent = 0;
uint8_t g_menuPos[4];
uint8_t g_menuStackPtr = 0;

void popMenu()
{
  assert(g_menuStackPtr>0);
  g_menuStackPtr = g_menuStackPtr-1;
  menuEvent = EVT_ENTRY_UP;
}

void chainMenu(MenuFuncP newMenu)
{
  g_menuStack[g_menuStackPtr] = newMenu;
  menuEvent = EVT_ENTRY;
}

void pushMenu(MenuFuncP newMenu)
{
  killEvents(KEY_ENTER);

  if (g_menuStackPtr == 0) {
    if (newMenu == menuGeneralSetup)
      g_menuPos[0] = 1;
    if (newMenu == menuModelSelect)
      g_menuPos[0] = 0;
  }
  else {
    g_menuPos[g_menuStackPtr] = m_posVert;
  }

  g_menuStackPtr++;

  assert(g_menuStackPtr < DIM(g_menuStack));

  g_menuStack[g_menuStackPtr] = newMenu;
  menuEvent = EVT_ENTRY;
}

void menuModelNotes(evt_t event)
{
  if (event == EVT_ENTRY) {
    strcpy(s_text_file, MODELS_PATH "/");
    char *buf = strcat_modelname(&s_text_file[sizeof(MODELS_PATH)], g_eeGeneral.currModel);
    strcpy(buf, TEXT_EXT);
  }

  menuTextView(event);
}

void pushModelNotes()
{
  pushMenu(menuModelNotes);
}
