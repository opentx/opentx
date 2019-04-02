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

void addRadioTool(uint8_t index, const char * label, void (* tool)(event_t event), uint8_t module, event_t event)
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
  uint8_t spektrum_modules = 0, power_modules = 0;

  if (event == EVT_ENTRY  || event == EVT_ENTRY_UP) {
    memclear(&reusableBuffer.hardwareAndSettings, sizeof(reusableBuffer.hardwareAndSettings));
#if defined(PXX2)
    for (uint8_t module = 0; module < NUM_MODULES; module++) {
      if (isModulePXX2(module) && (module == INTERNAL_MODULE ? IS_INTERNAL_MODULE_ON() : IS_EXTERNAL_MODULE_ON())) {
        reusableBuffer.hardwareAndSettings.modules[module].current = PXX2_HW_INFO_TX_ID;
        reusableBuffer.hardwareAndSettings.modules[module].maximum = PXX2_HW_INFO_TX_ID;
        moduleSettings[module].mode = MODULE_MODE_GET_HARDWARE_INFO;
      }
    }
#endif
  }

  for (uint8_t module = 0; module < NUM_MODULES; module++) {
    if (isModuleOptionAvailable(module, MODULE_OPTION_SPEKTRUM_ANALYSER)) {
      spektrum_modules++;
    }
    if (isModuleOptionAvailable(module, MODULE_OPTION_POWER_METER)) {
      power_modules++;
    }
  }

  SIMPLE_MENU("TOOLS", menuTabGeneral, MENU_RADIO_TOOLS, HEADER_LINE + spektrum_modules + power_modules);

#if defined(PXX2)
  uint8_t menu_index = 0;

  if (isModuleOptionAvailable(INTERNAL_MODULE, MODULE_OPTION_SPEKTRUM_ANALYSER))
    addRadioTool(menu_index++, "Spectrum (INT)", menuRadioSpectrumAnalyser, INTERNAL_MODULE, event);

  if (isModuleOptionAvailable(INTERNAL_MODULE, MODULE_OPTION_POWER_METER))
    addRadioTool(menu_index++, "Power Meter (INT)", menuRadioPowerMeter, INTERNAL_MODULE, event);

  if (isModuleOptionAvailable(EXTERNAL_MODULE, MODULE_OPTION_SPEKTRUM_ANALYSER))
    addRadioTool(menu_index++, "Spectrum (EXT)", menuRadioSpectrumAnalyser, EXTERNAL_MODULE, event);

  if (isModuleOptionAvailable(EXTERNAL_MODULE, MODULE_OPTION_POWER_METER))
    addRadioTool(menu_index++, "Power Meter (EXT)", menuRadioPowerMeter, EXTERNAL_MODULE, event);
#endif
}