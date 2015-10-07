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

extern LP_CONST LanguagePack czLanguagePack;
extern LP_CONST LanguagePack enLanguagePack;
extern LP_CONST LanguagePack esLanguagePack;
extern LP_CONST LanguagePack frLanguagePack;
extern LP_CONST LanguagePack deLanguagePack;
extern LP_CONST LanguagePack itLanguagePack;
extern LP_CONST LanguagePack plLanguagePack;
extern LP_CONST LanguagePack ptLanguagePack;
extern LP_CONST LanguagePack skLanguagePack;
extern LP_CONST LanguagePack seLanguagePack;
extern LP_CONST LanguagePack huLanguagePack;

const LanguagePack * LP_CONST languagePacks[] = {
  // alphabetical order
  &czLanguagePack,
  &deLanguagePack,
  &enLanguagePack,
  &esLanguagePack,
  &frLanguagePack,
  &huLanguagePack,
  &itLanguagePack,
  &plLanguagePack,
  &ptLanguagePack,
  &seLanguagePack,
  &skLanguagePack,
  NULL
};

void menuGeneralSetup(evt_t event);
void menuGeneralSdManager(evt_t event);
void menuGeneralCustomFunctions(evt_t event);
void menuGeneralTrainer(evt_t event);
void menuGeneralVersion(evt_t event);
void menuGeneralCalib(evt_t event);

enum EnumTabDiag {
  e_Setup,
  e_Sd,
  e_GeneralCustomFunctions,
  e_Trainer,
  e_Calib,
  e_Vers,
};

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
  menuGeneralSetup,
  CASE_SDCARD(menuGeneralSdManager)
  CASE_CPUARM(menuGeneralCustomFunctions)
  menuGeneralTrainer,
  menuGeneralCalib,
  menuGeneralVersion,
};

void menuGeneralCustomFunctions(evt_t event)
{
  MENU(STR_MENUGLOBALFUNCS, menuTabGeneral, e_GeneralCustomFunctions, NUM_CFN, DEFAULT_SCROLLBAR_X, { NAVIGATION_LINE_BY_LINE|4/*repeated*/ });
  return menuCustomFunctions(event, g_eeGeneral.customFn, globalFunctionsContext);
}
