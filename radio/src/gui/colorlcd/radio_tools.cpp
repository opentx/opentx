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

#include <algorithm>
#include "radio_tools.h"
#include "radio_spectrum_analyser.h"
#include "opentx.h"
#include "libopenui.h"

extern uint8_t g_moduleIdx;

#define TOOL_NAME_MAXLEN  16

#if defined(LUA) || defined(DEBUG)
bool readToolName(const char * filename, char * name)
{
  FIL file;
  char buffer[1024];
  UINT count;

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return "Error opening file";
  }

  if (f_read(&file, &buffer, sizeof(buffer), &count) != FR_OK) {
    f_close(&file);
    return false;
  }

  const char * tns = "TNS|";
  auto * start = std::search(buffer, buffer + sizeof(buffer), tns, tns + 4);
  if (start >= buffer + sizeof(buffer))
    return false;

  start += 4;

  const char * tne = "|TNE";
  auto * end = std::search(buffer, buffer + sizeof(buffer), tne, tne + 4);
  if (end >= buffer + sizeof(buffer) || end <= start)
    return false;

  uint8_t len = end - start;
  if (len > TOOL_NAME_MAXLEN)
    return false;

  strncpy(name, start, len);
  memclear(name + len, TOOL_NAME_MAXLEN + 1 - len);

  return true;
}

bool isRadioScriptTool(const char * filename)
{
  const char * ext = getFileExtension(filename);
  return ext && !strcasecmp(ext, SCRIPT_EXT);
}
#endif

RadioToolsPage::RadioToolsPage() :
  PageTab(STR_MENUTOOLS, ICON_RADIO_TOOLS)
{
}

void RadioToolsPage::build(FormWindow * window)
{
  this->window = window;

  memclear(&reusableBuffer.radioTools, sizeof(reusableBuffer.radioTools));
  waiting = 0;

#if defined(PXX2)
  for (uint8_t module = 0; module < NUM_MODULES; module++) {
    if (isModulePXX2(module) && (module == INTERNAL_MODULE ? IS_INTERNAL_MODULE_ON() : IS_EXTERNAL_MODULE_ON())) {
      waiting |= (1 << module);
      moduleState[module].readModuleInformation(&reusableBuffer.radioTools.modules[module], PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
    }
  }
#endif

  rebuild(window);
}

void RadioToolsPage::checkEvents()
{
  bool refresh = false;

  for (uint8_t module = 0; module < NUM_MODULES; module++) {
    if ((waiting & (1 << module)) && reusableBuffer.radioTools.modules[module].information.modelID) {
      waiting &= ~(1 << module);
      refresh = true;
    }
  }

  if (refresh) {
    rebuild(window);
  }

  PageTab::checkEvents();
}

void RadioToolsPage::rebuild(FormWindow * window)
{
  FormGridLayout grid;
  grid.spacer(8);
  grid.setLabelWidth(100);

  window->clear();

// LUA scripts in TOOLS
#if defined(LUA) || defined(DEBUG)
  FILINFO fno;
  DIR dir;

#if defined(CROSSFIRE)
//  if (isFileAvailable(SCRIPTS_TOOLS_PATH "/CROSSFIRE/crossfire.lua"))
//    addRadioScriptTool(index++, SCRIPTS_TOOLS_PATH "/CROSSFIRE/crossfire.lua");
#endif

  FRESULT res = f_opendir(&dir, SCRIPTS_TOOLS_PATH);
  if (res == FR_OK) {
    for (;;) {
      TCHAR path[_MAX_LFN+1] = SCRIPTS_TOOLS_PATH "/";
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fattrib & AM_DIR) continue;            /* Skip subfolders */
      if (fno.fattrib & AM_HID) continue;            /* Skip hidden files */
      if (fno.fattrib & AM_SYS) continue;            /* Skip system files */

      strcat(path, fno.fname);
      if (isRadioScriptTool(fno.fname)) {
        char toolName[TOOL_NAME_MAXLEN + 1];
        const char * label;
        char * ext = (char *)getFileExtension(path);
        if (readToolName(path, toolName)) {
          label = toolName;
        }
        else {
          *ext = '\0';
          label = getBasename(path);
        }
        new StaticText(window, grid.getLabelSlot(), "lua", BUTTON_BACKGROUND | CENTERED);
        new TextButton(window, grid.getFieldSlot(1), label, [=]() -> uint8_t {
          f_chdir("/SCRIPTS/TOOLS/");
          //luaExec(path);
          return 0;
        }, 0);
        grid.nextLine();
      }
    }
  }
#endif

#if defined(PXX2)
  // PXX2 modules tools
  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER)) {
    new StaticText(window, grid.getLabelSlot(), "access", BUTTON_BACKGROUND | CENTERED);
    new TextButton(window, grid.getFieldSlot(1), STR_SPECTRUM_ANALYSER_INT, [=]() -> uint8_t {
        new RadioSpectrumAnalyser(INTERNAL_MODULE);
        return 0;
    }, 0);
    grid.nextLine();
  }

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER)) {
    new StaticText(window, grid.getLabelSlot(), "access", BUTTON_BACKGROUND | CENTERED);
    new TextButton(window, grid.getFieldSlot(1), STR_POWER_METER_INT, [=]() -> uint8_t {
//        new RadioPowerMeter(INTERNAL_MODULE);
        return 0;
    }, 0);
    grid.nextLine();
  }

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER)) {
    new StaticText(window, grid.getLabelSlot(), "access", BUTTON_BACKGROUND | CENTERED);
    new TextButton(window, grid.getFieldSlot(1), STR_SPECTRUM_ANALYSER_EXT, [=]() -> uint8_t {
        new RadioSpectrumAnalyser(EXTERNAL_MODULE);
        return 0;
    }, 0);
    grid.nextLine();
  }

  if (isModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER)) {
    new StaticText(window, grid.getLabelSlot(), "access", BUTTON_BACKGROUND | CENTERED);
    new TextButton(window, grid.getFieldSlot(1), STR_POWER_METER_EXT, [=]() -> uint8_t {
//        new RadioPowerMeter(EXTERNAL_MODULE);
      return 0;
    }, 0);

    grid.nextLine();
  }
#endif

  window->setInnerHeight(grid.getWindowHeight());
}