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
#include "opentx.h"

extern uint8_t g_moduleIdx;

inline void REFRESH_TOOSLFILES()
{
  reusableBuffer.radioTools.offset = 65535;
  menuVerticalOffset = 0;
}

inline bool IS_LUA_TOOLS(uint8_t index)
{
  return reusableBuffer.radioTools.lines[index].exec == nullptr ;
}

#if defined(LUA)
void addRadioScriptTool(const char * path)
{
  char toolName[RADIO_TOOL_NAME_MAXLEN + 1];
  const char * label;
  char * ext = (char *)getFileExtension(path);
  if (readToolName(toolName, path)) {
    label = toolName;
  }
  else {
    *ext = '\0';
    label = getBasename(path);
  }

  if (menuVerticalOffset == 0) {
    for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
      char * line = reusableBuffer.radioTools.lines[i].displayname;
      if (line[0] == '\0') {
        if (i < NUM_BODY_LINES-1) memmove(&reusableBuffer.radioTools.lines[i+1], line, sizeof(reusableBuffer.radioTools.lines[i]) * (NUM_BODY_LINES-1-i));
        memset(line, 0, sizeof(reusableBuffer.radioTools.lines[i].displayname));
        strcpy(line, label);
        *ext = '.';
        strcpy(reusableBuffer.radioTools.lines[i].filename, getBasename(path));
        break;
      }
    }
  }
  else if (reusableBuffer.radioTools.offset == menuVerticalOffset) {
    for (int8_t i=NUM_BODY_LINES-1; i>=0; i--) {
      char *line = reusableBuffer.radioTools.lines[i].displayname;
      if (line[0] == '\0') {
        if (i > 0) memmove(&reusableBuffer.radioTools.lines[0], &reusableBuffer.radioTools.lines[1], sizeof(reusableBuffer.radioTools.lines[0]) * i);
        memset(line, 0, sizeof(reusableBuffer.radioTools.lines[i].displayname));
        strcpy(line, label);
        *ext = '.';
        strcpy(reusableBuffer.radioTools.lines[i].filename, getBasename(path));
        break;
      }
    }
  }
  else if (menuVerticalOffset > reusableBuffer.radioTools.offset) {
    memset(&reusableBuffer.radioTools.lines[NUM_BODY_LINES - 1], 0, sizeof(reusableBuffer.radioTools.lines[0]));
    strcpy(reusableBuffer.radioTools.lines[NUM_BODY_LINES - 1].displayname, label);
    *ext = '.';
    strcpy(reusableBuffer.radioTools.lines[NUM_BODY_LINES - 1].filename, getBasename(path));
  }
  else {
    memset(&reusableBuffer.radioTools.lines[0], 0, sizeof(reusableBuffer.radioTools.lines[0]));
    strcpy(reusableBuffer.radioTools.lines[0].filename, label);
    *ext = '.';
    strcpy(reusableBuffer.radioTools.lines[0].filename, getBasename(path));
  }
  reusableBuffer.radioTools.count++;
}
#endif

void addRadioModuleTool(const char * label, bool (* tool)(event_t), uint8_t module) {
  if (menuVerticalOffset == 0) {
    for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
      char * line = reusableBuffer.radioTools.lines[i].displayname;
      if (line[0] == '\0') {
        if (i < NUM_BODY_LINES-1) memmove(&reusableBuffer.radioTools.lines[i+1], line, sizeof(reusableBuffer.radioTools.lines[i]) * (NUM_BODY_LINES-1-i));
        memset(line, 0, sizeof(reusableBuffer.radioTools.lines[0].displayname));
        strcpy(line, label);
        reusableBuffer.radioTools.lines[i].exec = tool;
        reusableBuffer.radioTools.lines[i].module = module;
        break;
      }
    }
  }
  else if (reusableBuffer.radioTools.offset == menuVerticalOffset) {
    for (int8_t i=NUM_BODY_LINES-1; i>=0; i--) {
      char *line = reusableBuffer.radioTools.lines[i].displayname;
      if (line[0] == '\0') {
        if (i > 0) memmove(&reusableBuffer.radioTools.lines[0], &reusableBuffer.radioTools.lines[1], sizeof(reusableBuffer.radioTools.lines[0]) * i);
        memset(line, 0, sizeof(reusableBuffer.radioTools.lines[0].displayname));
        strcpy(line, label);
        reusableBuffer.radioTools.lines[i].exec = tool;
        reusableBuffer.radioTools.lines[i].module = module;
        break;
      }
    }
  }
  reusableBuffer.radioTools.count++;
}
bool menuRadioTools(event_t event)
{
  SIMPLE_MENU(STR_MENUTOOLS, RADIO_ICONS, menuTabGeneral, MENU_RADIO_TOOLS, reusableBuffer.radioTools.count);

  int index = menuVerticalPosition - menuVerticalOffset;

  switch (event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      // no break;

    case EVT_ENTRY_UP:
      memclear(&reusableBuffer.radioTools, sizeof(reusableBuffer.radioTools));
      menuVerticalPosition = 0;
      REFRESH_TOOSLFILES();
#if defined(PXX2)
      for (uint8_t module = 0; module < NUM_MODULES; module++) {
        if (isModulePXX2(module) && (module == INTERNAL_MODULE ? IS_INTERNAL_MODULE_ON() : IS_EXTERNAL_MODULE_ON())) {
          moduleState[module].readModuleInformation(&reusableBuffer.radioTools.modules[module], PXX2_HW_INFO_TX_ID, PXX2_HW_INFO_TX_ID);
        }
      }
#endif
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      if (IS_LUA_TOOLS(index)) {
        f_chdir("/SCRIPTS/TOOLS/");

        TCHAR path[_MAX_LFN+1] = SCRIPTS_TOOLS_PATH "/";
        strcat(path, reusableBuffer.radioTools.lines[index].filename);
        luaExec(path);
        REFRESH_TOOSLFILES();
        s_editMode = 0;
      }
      else {
        g_moduleIdx = reusableBuffer.radioTools.lines[index].module;
        pushMenu(reusableBuffer.radioTools.lines[index].exec);
        REFRESH_TOOSLFILES();
      }
      break;
  }

#if defined(LUA)
  if (reusableBuffer.radioTools.offset != menuVerticalOffset) {

    if (menuVerticalOffset == reusableBuffer.radioTools.offset + 1) {
      memmove(&reusableBuffer.radioTools.lines[0], &reusableBuffer.radioTools.lines[1], (NUM_BODY_LINES-1)*sizeof(reusableBuffer.radioTools.lines[0]));
      //memset(&reusableBuffer.radioTools.lines[NUM_BODY_LINES-1], 0xff, LEN_FILE_PATH_MAX);
    }
    else if (menuVerticalOffset == reusableBuffer.radioTools.offset - 1) {
      memmove(&reusableBuffer.radioTools.lines[1], &reusableBuffer.radioTools.lines[0], (NUM_BODY_LINES-1)*sizeof(reusableBuffer.radioTools.lines[0]));
      memset(&reusableBuffer.radioTools.lines[0], 0, sizeof(reusableBuffer.radioTools.lines[0]));
    }
    else {
      reusableBuffer.radioTools.offset = menuVerticalOffset;
      memset(reusableBuffer.radioTools.lines, 0, sizeof(reusableBuffer.radioTools.lines));
    }

    reusableBuffer.radioTools.count = 0;

#if defined(INTERNAL_MODULE_PXX2)
    if (isPXX2ModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER))
      addRadioModuleTool(STR_SPECTRUM_ANALYSER_INT, menuRadioSpectrumAnalyser, INTERNAL_MODULE);

    if (isPXX2ModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER))
      addRadioModuleTool(STR_POWER_METER_INT, menuRadioPowerMeter, INTERNAL_MODULE);
#elif defined(INTERNAL_MODULE_MULTI)
    addRadioModuleTool(STR_SPECTRUM_ANALYSER_INT, menuRadioSpectrumAnalyser, INTERNAL_MODULE);
#endif
#if defined(PXX2) || defined(MULTIMODULE)
    if (isPXX2ModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_SPECTRUM_ANALYSER) ||
        isModuleMultimodule(EXTERNAL_MODULE))
      addRadioModuleTool(STR_SPECTRUM_ANALYSER_EXT, menuRadioSpectrumAnalyser, EXTERNAL_MODULE);
#endif
#if defined(PXX2)
    if (isPXX2ModuleOptionAvailable(reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].information.modelID, MODULE_OPTION_POWER_METER))
        addRadioModuleTool(STR_POWER_METER_EXT, menuRadioPowerMeter, EXTERNAL_MODULE);
#endif


    FILINFO fno;
    DIR dir;
    FRESULT res = f_opendir(&dir, SCRIPTS_TOOLS_PATH);
    if (res == FR_OK) {
      for (;;) {
        TCHAR path[_MAX_LFN + 1] = SCRIPTS_TOOLS_PATH "/";
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fattrib & AM_DIR) continue;            /* Skip subfolders */
        if (fno.fattrib & AM_HID) continue;            /* Skip hidden files */
        if (fno.fattrib & AM_SYS) continue;            /* Skip system files */
        strcat(path, fno.fname);

        if (!isRadioScriptTool(fno.fname))
          continue;

        addRadioScriptTool(path);
      }
      f_closedir(&dir);

      if (index == 0) {
        lcdDrawCenteredText(LCD_H / 2, STR_NO_TOOLS);
      }
    }
  }
#endif
  reusableBuffer.radioTools.offset = menuVerticalOffset;

  for (uint8_t i = 0; i < NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    LcdFlags attr = (index == i ? INVERS : 0);
    if (reusableBuffer.radioTools.lines[i].displayname[0]) {
      lcdDrawText(MENUS_MARGIN_LEFT, y, reusableBuffer.radioTools.lines[i].displayname, attr);
    }
  }

  reusableBuffer.radioTools.linesCount = index;

  return true;
}