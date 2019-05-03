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

extern uint8_t g_moduleIdx;

void addRadioTool(uint8_t index, const char * label, bool (* tool)(event_t), uint8_t module)
{
  int8_t sub = menuVerticalPosition;
  LcdFlags attr = (sub == index ? INVERS : 0);
  coord_t y = MENU_CONTENT_TOP + index * FH;
  lcdDrawNumber(3, y, index + 1, LEADING0|LEFT, 2);
  lcdDrawText(30, y, label, (sub == index ? INVERS  : 0));
  if (attr && s_editMode > 0) {
    s_editMode = 0;
    g_moduleIdx = module;
    pushMenu(tool);
  }
}

bool menuRadioTools(event_t event)
{
  uint8_t spectrum_modules = 0, power_modules = 0;

  if (event == EVT_ENTRY  || event == EVT_ENTRY_UP) {
    memclear(&reusableBuffer.hardwareAndSettings, sizeof(reusableBuffer.hardwareAndSettings));

    for (uint8_t module = 0; module < NUM_MODULES; module++) {
      if (isModulePXX2(module) && (module == INTERNAL_MODULE ? IS_INTERNAL_MODULE_ON() : IS_EXTERNAL_MODULE_ON())) {
        moduleState[module].readModuleInformation(&reusableBuffer.hardwareAndSettings.modules[module], PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
      }
    }

  }

  for (uint8_t module = 0; module < NUM_MODULES; module++) {
    if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[module].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER)) {
      spectrum_modules++;
    }
    if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[module].information.modelID, MODULE_OPTION_POWER_METER)) {
      power_modules++;
    }
  }

  MENU(STR_MENUTOOLS, RADIO_ICONS, menuTabGeneral, MENU_RADIO_TOOLS, spectrum_modules + power_modules, {0,0,0,0});

  uint8_t menu_index = 0;

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER))
    addRadioTool(menu_index++, "Spectrum (INT)", menuRadioSpectrumAnalyser, INTERNAL_MODULE);

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER))
    addRadioTool(menu_index++, "Power Meter (INT)", menuRadioPowerMeter, INTERNAL_MODULE);

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER))
    addRadioTool(menu_index++, "Spectrum (EXT)", menuRadioSpectrumAnalyser, EXTERNAL_MODULE);

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER))
    addRadioTool(menu_index++, "Power Meter (EXT)", menuRadioPowerMeter, EXTERNAL_MODULE);

  return true;
}