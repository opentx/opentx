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

void pxx2ModuleRequiredScreen(event_t event)
{
  lcdClear();
  lcdDrawCenteredText(15, "THIS FEATURE REQUIRES");
  lcdDrawCenteredText(30, "ACCESS UPGRADE ON");
  lcdDrawCenteredText(45, "YOUR INTERNAL MODULE");

  if (event == EVT_KEY_FIRST(KEY_EXIT)) {
    killEvents(event);
    popMenu();
  }
}

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

bool hasSpektrumAnalyserCapability(uint8_t module)
{
#if defined(SIMU)
  return true;
#else
  return (reusableBuffer.hardwareAndSettings.modules[module].information.modelID == PXX2_MODULE_IXJT_S ||
          reusableBuffer.hardwareAndSettings.modules[module].information.modelID == PXX2_MODULE_IXJT_PRO  ||
          reusableBuffer.hardwareAndSettings.modules[module].information.modelID >= PXX2_MODULE_R9M_LITE_PRO);
#endif
}

bool hasPowerMeterCapablity(uint8_t module)
{
#if defined(SIMU)
  return true;
#else
  return (reusableBuffer.hardwareAndSettings.modules[module].information.modelID == PXX2_MODULE_IXJT_PRO  ||
         reusableBuffer.hardwareAndSettings.modules[module].information.modelID == PXX2_MODULE_R9M_LITE_PRO);
#endif
}

void menuRadioTools(event_t event)
{
  uint8_t spektrum_modules = 0, power_modules = 0;

  if(event == EVT_ENTRY  || event == EVT_ENTRY_UP) {
    memclear(&reusableBuffer.hardwareAndSettings, sizeof(reusableBuffer.hardwareAndSettings));
    for (uint8_t idx=0; idx < NUM_MODULES; idx++) {
      if (isModulePXX2(idx) && (idx == INTERNAL_MODULE ? IS_INTERNAL_MODULE_ON() : IS_EXTERNAL_MODULE_ON())) {
        reusableBuffer.hardwareAndSettings.modules[idx].current = PXX2_HW_INFO_TX_ID;
        reusableBuffer.hardwareAndSettings.modules[idx].maximum = PXX2_HW_INFO_TX_ID;
        moduleSettings[idx].mode = MODULE_MODE_GET_HARDWARE_INFO;
      }
    }
  }

  for (uint8_t idx=0; idx < NUM_MODULES; idx++) {
    if(hasSpektrumAnalyserCapability(idx)) {
      spektrum_modules++;
    }
    if(hasPowerMeterCapablity(idx)) {
      power_modules++;
    }
  }

  SIMPLE_MENU("TOOLS", menuTabGeneral, MENU_RADIO_TOOLS, HEADER_LINE + spektrum_modules + power_modules);

  uint8_t menu_index=0;
  if(hasSpektrumAnalyserCapability(INTERNAL_MODULE))
    addRadioTool(menu_index++, "Spectrum (INT)", menuRadioSpectrumAnalyser, INTERNAL_MODULE, event);
  if(hasPowerMeterCapablity(INTERNAL_MODULE))
    addRadioTool(menu_index++, "Power Meter (INT)", menuRadioPowerMeter, INTERNAL_MODULE, event);
  if(hasSpektrumAnalyserCapability(EXTERNAL_MODULE))
    addRadioTool(menu_index++, "Spectrum (EXT)", menuRadioSpectrumAnalyser, EXTERNAL_MODULE, event);
  if(hasPowerMeterCapablity(EXTERNAL_MODULE))
    addRadioTool(menu_index++, "Power Meter (EXT)", menuRadioPowerMeter, EXTERNAL_MODULE, event);
}