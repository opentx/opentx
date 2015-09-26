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

#if defined(CPUARM)
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
#endif

void menuGeneralSetup(uint8_t event);
void menuGeneralSdManager(uint8_t event);
void menuGeneralCustomFunctions(uint8_t event);
void menuGeneralTrainer(uint8_t event);
void menuGeneralVersion(uint8_t event);
void menuGeneralDiagKeys(uint8_t event);
void menuGeneralDiagAna(uint8_t event);
void menuGeneralHardware(uint8_t event);
void menuGeneralCalib(uint8_t event);

enum EnumTabDiag {
  e_Setup,
  CASE_SDCARD(e_Sd)
  CASE_CPUARM(e_GeneralCustomFunctions)
  e_Trainer,
  e_Vers,
  e_Keys,
  e_Ana,
  CASE_CPUARM(e_Hardware)
  e_Calib
};

const MenuFuncP_PROGMEM menuTabGeneral[] PROGMEM = {
  menuGeneralSetup,
  CASE_SDCARD(menuGeneralSdManager)
  CASE_CPUARM(menuGeneralCustomFunctions)
  menuGeneralTrainer,
  menuGeneralVersion,
  menuGeneralDiagKeys,
  menuGeneralDiagAna,
  CASE_CPUARM(menuGeneralHardware)
  menuGeneralCalib
};

#if defined(CPUARM)
void menuGeneralCustomFunctions(uint8_t event)
{
  MENU(STR_MENUGLOBALFUNCS, menuTabGeneral, e_GeneralCustomFunctions, NUM_CFN+1, {0, NAVIGATION_LINE_BY_LINE|4/*repeated*/});
  return menuCustomFunctions(event, g_eeGeneral.customFn, &globalFunctionsContext);
}
#endif
