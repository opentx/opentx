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

void onModelCustomScriptMenu(const char *result)
{
  ScriptData &sd = g_model.scriptsData[s_currIdx];

  if (result == STR_UPDATE_LIST) {
    if (!listSdFiles(SCRIPTS_MIXES_PATH, SCRIPTS_EXT, sizeof(sd.file), NULL)) {
      POPUP_WARNING(STR_NO_SCRIPTS_ON_SD);
      s_menu_flags = 0;
    }
  }
  else {
    // The user choosed a lua file in the list
    copySelection(sd.file, result, sizeof(sd.file));
    memset(sd.inputs, 0, sizeof(sd.inputs));
    eeDirty(EE_MODEL);
    LUA_LOAD_MODEL_SCRIPT(s_currIdx);
  }
}

enum menuModelCustomScriptItems {
  ITEM_MODEL_CUSTOMSCRIPT_FILE,
  ITEM_MODEL_CUSTOMSCRIPT_NAME,
  ITEM_MODEL_CUSTOMSCRIPT_PARAMS_LABEL,
};

#define SCRIPT_ONE_2ND_COLUMN_POS  (12*FW)
#define SCRIPT_ONE_3RD_COLUMN_POS  (23*FW)

void menuModelCustomScriptOne(uint8_t event)
{
  TITLE(STR_MENUCUSTOMSCRIPT);

  ScriptData &sd = g_model.scriptsData[s_currIdx];

  putsStrIdx(lcdLastPos+FW, 0, "LUA", s_currIdx+1, 0);

  SUBMENU_NOTITLE(3+scriptInputsOutputs[s_currIdx].inputsCount, { 0, 0, LABEL(inputs), 0/*repeated*/ });

  int8_t sub = m_posVert;

  for (int k=0; k<LCD_LINES-1; k++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    int i = k + s_pgOfs;
    LcdFlags attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);

    if (i == ITEM_MODEL_CUSTOMSCRIPT_FILE) {
      lcd_putsLeft(y, STR_SCRIPT);
      if (ZEXIST(sd.file))
        lcd_putsnAtt(SCRIPT_ONE_2ND_COLUMN_POS, y, sd.file, sizeof(sd.file), attr);
      else
        lcd_putsiAtt(SCRIPT_ONE_2ND_COLUMN_POS, y, STR_VCSWFUNC, 0, attr);
      if (attr && event==EVT_KEY_BREAK(KEY_ENTER) && !READ_ONLY()) {
        s_editMode = 0;
        if (listSdFiles(SCRIPTS_MIXES_PATH, SCRIPTS_EXT, sizeof(sd.file), sd.file, LIST_NONE_SD_FILE)) {
          menuHandler = onModelCustomScriptMenu;
        }
        else {
          POPUP_WARNING(STR_NO_SCRIPTS_ON_SD);
          s_menu_flags = 0;
        }
      }
    }
    else if (i == ITEM_MODEL_CUSTOMSCRIPT_NAME) {
      lcd_putsLeft(y, TR_NAME);
      editName(SCRIPT_ONE_2ND_COLUMN_POS, y, sd.name, sizeof(sd.name), event, attr);
    }
    else if (i == ITEM_MODEL_CUSTOMSCRIPT_PARAMS_LABEL) {
      lcd_putsLeft(y, STR_INPUTS);
    }
    else if (i <= ITEM_MODEL_CUSTOMSCRIPT_PARAMS_LABEL+scriptInputsOutputs[s_currIdx].inputsCount) {
      int inputIdx = i-ITEM_MODEL_CUSTOMSCRIPT_PARAMS_LABEL-1;
      lcd_putsnAtt(INDENT_WIDTH, y, scriptInputsOutputs[s_currIdx].inputs[inputIdx].name, 10, 0);
      if (scriptInputsOutputs[s_currIdx].inputs[inputIdx].type == 0) {
        lcd_outdezAtt(SCRIPT_ONE_2ND_COLUMN_POS, y, g_model.scriptsData[s_currIdx].inputs[inputIdx]+scriptInputsOutputs[s_currIdx].inputs[inputIdx].def, attr|LEFT);
        if (attr) {
          CHECK_INCDEC_MODELVAR(event, g_model.scriptsData[s_currIdx].inputs[inputIdx], scriptInputsOutputs[s_currIdx].inputs[inputIdx].min-scriptInputsOutputs[s_currIdx].inputs[inputIdx].def, scriptInputsOutputs[s_currIdx].inputs[inputIdx].max-scriptInputsOutputs[s_currIdx].inputs[inputIdx].def);
        }
      }
      else {
        uint8_t *source = (uint8_t *)&g_model.scriptsData[s_currIdx].inputs[inputIdx];
        putsMixerSource(SCRIPT_ONE_2ND_COLUMN_POS, y, *source + scriptInputsOutputs[s_currIdx].inputs[inputIdx].def, attr);
        if (attr) {
          CHECK_INCDEC_MODELSOURCE(event, *source, scriptInputsOutputs[s_currIdx].inputs[inputIdx].min-scriptInputsOutputs[s_currIdx].inputs[inputIdx].def, scriptInputsOutputs[s_currIdx].inputs[inputIdx].max-scriptInputsOutputs[s_currIdx].inputs[inputIdx].def);
        }
      }
    }
  }

  if (scriptInputsOutputs[s_currIdx].outputsCount > 0) {
    lcd_vline(SCRIPT_ONE_3RD_COLUMN_POS-4, FH+1, LCD_H-FH-1);
    lcd_puts(SCRIPT_ONE_3RD_COLUMN_POS, FH+1, STR_OUTPUTS);

    for (int i=0; i<scriptInputsOutputs[s_currIdx].outputsCount; i++) {
      putsMixerSource(SCRIPT_ONE_3RD_COLUMN_POS+INDENT_WIDTH, FH+1+FH+i*FH, MIXSRC_FIRST_LUA+(s_currIdx*MAX_SCRIPT_OUTPUTS)+i, 0);
      lcd_outdezNAtt(SCRIPT_ONE_3RD_COLUMN_POS+11*FW+3, FH+1+FH+i*FH, calcRESXto1000(scriptInputsOutputs[s_currIdx].outputs[i].value), PREC1);
    }
  }
}

void menuModelCustomScripts(uint8_t event)
{
  lcd_outdezAtt(19*FW, 0, luaGetMemUsed(), 0);
  lcd_puts(19*FW+1, 0, STR_BYTES);

  MENU(STR_MENUCUSTOMSCRIPTS, menuTabModel, e_CustomScripts, MAX_SCRIPTS, { NAVIGATION_LINE_BY_LINE|3/*repeated*/ });

  coord_t y;
  int8_t  sub = m_posVert;

  if (event == EVT_KEY_FIRST(KEY_ENTER)) {
    s_currIdx = sub;
    pushMenu(menuModelCustomScriptOne);
  }

  for (int i=0, scriptIndex=0; i<MAX_SCRIPTS; i++) {
    y = 1 + (i+1)*FH;

    ScriptData &sd = g_model.scriptsData[i];

    // LUAx header
    putsStrIdx(0, y, "LUA", i+1, sub==i ? INVERS : 0);

    // LUA script
    if (ZEXIST(sd.file)) {
      lcd_putsnAtt(5*FW, y, sd.file, sizeof(sd.file), 0);
      switch (scriptInternalData[scriptIndex].state) {
        case SCRIPT_SYNTAX_ERROR:
          lcd_puts(30*FW+2, y, "(error)");
          break;
        case SCRIPT_KILLED:
          lcd_puts(29*FW+2, y, "(killed)");
          break;
        default:
          lcd_outdezAtt(34*FW, y, luaGetCpuUsed(scriptIndex));
          lcd_putc(34*FW, y, '%');
          break;
      }
      scriptIndex++;
    }
    else {
      lcd_putsiAtt(5*FW, y, STR_VCSWFUNC, 0, 0);
    }

    // Script name
    lcd_putsnAtt(16*FW, y, sd.name, sizeof(sd.name), ZCHAR);
  }
}
