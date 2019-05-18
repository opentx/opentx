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

void addRadioTool(uint8_t index, const char * label, void (* tool)(event_t), uint8_t module)
{
  int8_t sub = menuVerticalPosition - HEADER_LINE;
  LcdFlags attr = (sub == index ? INVERS : 0);
  coord_t y = MENU_HEADER_HEIGHT + 1 + index * FH;
  lcdDrawNumber(3, y, index + 1, LEADING0|LEFT, 2);
  lcdDrawText(3*FW, y, label, (sub == index ? INVERS  : 0));
  if (attr && s_editMode > 0) {
    s_editMode = 0;
    g_moduleIdx = module;
    pushMenu(tool);
  }
}

void menuRadioTools(event_t event)
{
  uint8_t spectrum_modules = 0, power_modules = 0;

  if (event == EVT_ENTRY  || event == EVT_ENTRY_UP) {
    memclear(&reusableBuffer.hardwareAndSettings, sizeof(reusableBuffer.hardwareAndSettings));
#if defined(PXX2)
    for (uint8_t module = 0; module < NUM_MODULES; module++) {
      if (isModulePXX2(module) && (module == INTERNAL_MODULE ? IS_INTERNAL_MODULE_ON() : IS_EXTERNAL_MODULE_ON())) {
        moduleState[module].readModuleInformation(&reusableBuffer.hardwareAndSettings.modules[module], PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
      }
    }
#endif
  }

  for (uint8_t module = 0; module < NUM_MODULES; module++) {
    if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[module].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER)) {
      spectrum_modules++;
    }
    if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[module].information.modelID, MODULE_OPTION_POWER_METER)) {
      power_modules++;
    }
  }

  SIMPLE_MENU(STR_MENUTOOLS, menuTabGeneral, MENU_RADIO_TOOLS, HEADER_LINE + spectrum_modules + power_modules);

  uint8_t index = 0;

#if defined(PXX2)
  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER))
    addRadioTool(index++, STR_SPECTRUM_ANALYSER_INT, menuRadioSpectrumAnalyser, INTERNAL_MODULE);

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER))
    addRadioTool(index++, STR_POWER_METER_INT, menuRadioPowerMeter, INTERNAL_MODULE);

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER))
    addRadioTool(index++, STR_SPECTRUM_ANALYSER_EXT, menuRadioSpectrumAnalyser, EXTERNAL_MODULE);

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER))
    addRadioTool(index++, STR_POWER_METER_EXT, menuRadioPowerMeter, EXTERNAL_MODULE);
#endif

  if (index == 0) {
    lcdDrawCenteredText(LCD_H/2, STR_NO_TOOLS);
  }
}