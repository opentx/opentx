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

#define MODEL_CUSTOM_FUNC_1ST_COLUMN          (4*FW+2)
#define MODEL_CUSTOM_FUNC_2ND_COLUMN          (8*FW+2)
#define MODEL_CUSTOM_FUNC_3RD_COLUMN          (21*FW)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN          (33*FW-3)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF    (34*FW-3)

void onCustomFunctionsFileSelectionMenu(const char *result)
{
  int  sub = m_posVert;
  CustomFunctionData * cfn;
  uint8_t eeFlags;

  if (g_menuStack[g_menuStackPtr] == menuModelCustomFunctions) {
    cfn = &g_model.customFn[sub];
    eeFlags = EE_MODEL;
  }
  else {
    cfn = &g_eeGeneral.customFn[sub];
    eeFlags = EE_GENERAL;
  }

  uint8_t func = CFN_FUNC(cfn);

  if (result == STR_UPDATE_LIST) {
    char directory[256];
    if (func == FUNC_PLAY_SCRIPT) {
      strcpy(directory, SCRIPTS_FUNCS_PATH);
    }
    else {
      strcpy(directory, SOUNDS_PATH);
      strncpy(directory+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
    }
    if (!sdListFiles(directory, func==FUNC_PLAY_SCRIPT ? SCRIPTS_EXT : SOUNDS_EXT, sizeof(cfn->play.name), NULL)) {
      POPUP_WARNING(func==FUNC_PLAY_SCRIPT ? STR_NO_SCRIPTS_ON_SD : STR_NO_SOUNDS_ON_SD);
      s_menu_flags = 0;
    }
  }
  else {
    // The user choosed a file in the list
    memcpy(cfn->play.name, result, sizeof(cfn->play.name));
    storageDirty(eeFlags);
    if (func == FUNC_PLAY_SCRIPT) {
      LUA_LOAD_MODEL_SCRIPTS();
    }
  }
}

void onCustomFunctionsMenu(const char *result)
{
  int sub = m_posVert;
  CustomFunctionData * cfn;
  uint8_t eeFlags;

  if (g_menuStack[g_menuStackPtr] == menuModelCustomFunctions) {
    cfn = &g_model.customFn[sub];
    eeFlags = EE_MODEL;
  }
  else {
    cfn = &g_eeGeneral.customFn[sub];
    eeFlags = EE_GENERAL;
  }

  if (result == STR_COPY) {
    clipboard.type = CLIPBOARD_TYPE_CUSTOM_FUNCTION;
    clipboard.data.cfn = *cfn;
  }
  else if (result == STR_PASTE) {
    *cfn = clipboard.data.cfn;
    storageDirty(eeFlags);
  }
  else if (result == STR_CLEAR) {
    memset(cfn, 0, sizeof(CustomFunctionData));
    storageDirty(eeFlags);
  }
  else if (result == STR_INSERT) {
    memmove(cfn+1, cfn, (NUM_CFN-sub-1)*sizeof(CustomFunctionData));
    memset(cfn, 0, sizeof(CustomFunctionData));
    storageDirty(eeFlags);
  }
  else if (result == STR_DELETE) {
    memmove(cfn, cfn+1, (NUM_CFN-sub-1)*sizeof(CustomFunctionData));
    memset(&g_model.customFn[NUM_CFN-1], 0, sizeof(CustomFunctionData));
    storageDirty(eeFlags);
  }
}

void onAdjustGvarSourceLongEnterPress(const char * result)
{
  CustomFunctionData * cfn = &g_model.customFn[m_posVert];

  if (result == STR_CONSTANT) {
    CFN_GVAR_MODE(cfn) = FUNC_ADJUST_GVAR_CONSTANT;
    CFN_PARAM(cfn) = 0;
    storageDirty(EE_MODEL);
  }
  else if (result == STR_MIXSOURCE) {
    CFN_GVAR_MODE(cfn) = FUNC_ADJUST_GVAR_SOURCE;
    CFN_PARAM(cfn) = 0;
    storageDirty(EE_MODEL);
  }
  else if (result == STR_GLOBALVAR) {
    CFN_GVAR_MODE(cfn) = FUNC_ADJUST_GVAR_GVAR;
    CFN_PARAM(cfn) = 0;
    storageDirty(EE_MODEL);
  }
  else if (result == STR_INCDEC) {
    CFN_GVAR_MODE(cfn) = FUNC_ADJUST_GVAR_INC;
    CFN_PARAM(cfn) = 0;
    storageDirty(EE_MODEL);
  }
  else {
    onSourceLongEnterPress(result);
  }
}

void menuCustomFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext)
{
  int sub = m_posVert;
  uint8_t eeFlags = (functions == g_model.customFn) ? EE_MODEL : EE_GENERAL;
  if (m_posHorz<0 && event==EVT_KEY_LONG(KEY_ENTER) && !READ_ONLY()) {
    killEvents(event);
    CustomFunctionData *cfn = &functions[sub];
    if (!CFN_EMPTY(cfn))
      MENU_ADD_ITEM(STR_COPY);
    if (clipboard.type == CLIPBOARD_TYPE_CUSTOM_FUNCTION)
      MENU_ADD_ITEM(STR_PASTE);
    if (!CFN_EMPTY(cfn) && CFN_EMPTY(&functions[NUM_CFN-1]))
      MENU_ADD_ITEM(STR_INSERT);
    if (!CFN_EMPTY(cfn))
      MENU_ADD_ITEM(STR_CLEAR);
    for (int i=sub+1; i<NUM_CFN; i++) {
      if (!CFN_EMPTY(&functions[i])) {
        MENU_ADD_ITEM(STR_DELETE);
        break;
      }
    }
    menuHandler = onCustomFunctionsMenu;
  }

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    int k = i+s_pgOfs;

    putsStrIdx(0, y, functions == g_model.customFn ? STR_SF : STR_GF, k+1, (sub==k && m_posHorz<0) ? INVERS : 0);

    CustomFunctionData *cfn = &functions[k];
    uint8_t func = CFN_FUNC(cfn);
    for (uint8_t j=0; j<5; j++) {
      uint8_t attr = ((sub==k && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && s_editMode>0);
      switch (j) {
        case 0:
          putsSwitches(MODEL_CUSTOM_FUNC_1ST_COLUMN, y, CFN_SWITCH(cfn), attr | ((functionsContext->activeSwitches & ((MASK_CFN_TYPE)1 << k)) ? BOLD : 0));
          if (active || AUTOSWITCH_ENTER_LONG()) CHECK_INCDEC_SWITCH(event, CFN_SWITCH(cfn), SWSRC_FIRST, SWSRC_LAST, eeFlags, isSwitchAvailableInCustomFunctions);
          if (func == FUNC_OVERRIDE_CHANNEL && functions != g_model.customFn) {
            func = CFN_FUNC(cfn) = func+1;
          }
          break;

        case 1:
          if (CFN_SWITCH(cfn)) {
            lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_2ND_COLUMN, y, STR_VFSWFUNC, func, attr);
            if (active) {
              func = CFN_FUNC(cfn) = checkIncDec(event, CFN_FUNC(cfn), 0, FUNC_MAX-1, eeFlags, isAssignableFunctionAvailable);
              if (checkIncDec_Ret) CFN_RESET(cfn);
            }
          }
          else {
            j = 4; // skip other fields
            if (sub==k && m_posHorz > 0) {
              REPEAT_LAST_CURSOR_MOVE();
            }
          }
          break;

        case 2:
        {
          int8_t maxParam = NUM_CHNOUT-1;
#if defined(OVERRIDE_CHANNEL_FUNCTION)
          if (func == FUNC_OVERRIDE_CHANNEL) {
            putsChn(lcdNextPos, y, CFN_CH_INDEX(cfn)+1, attr);
          }
          else
#endif
          if (func == FUNC_TRAINER) {
            maxParam = 4;
            putsMixerSource(lcdNextPos, y, CFN_CH_INDEX(cfn)==0 ? 0 : MIXSRC_Rud+CFN_CH_INDEX(cfn)-1, attr);
          }
#if defined(GVARS)
          else if (func == FUNC_ADJUST_GVAR) {
            maxParam = MAX_GVARS-1;
            putsStrIdx(lcdNextPos, y, STR_GV, CFN_GVAR_INDEX(cfn)+1, attr);
            if (active) CFN_GVAR_INDEX(cfn) = checkIncDec(event, CFN_GVAR_INDEX(cfn), 0, maxParam, eeFlags);
            break;
          }
#endif
          else if (func == FUNC_SET_TIMER) {
            maxParam = TIMERS-1;
            putsStrIdx(lcdNextPos, y, STR_TIMER, CFN_TIMER_INDEX(cfn)+1, attr);
            if (active) CFN_TIMER_INDEX(cfn) = checkIncDec(event, CFN_TIMER_INDEX(cfn), 0, maxParam, eeFlags);
            break;
          }
          else if (attr) {
            REPEAT_LAST_CURSOR_MOVE();
          }
          if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_CH_INDEX(cfn), maxParam);
          break;
        }

        case 3:
        {
          INCDEC_DECLARE_VARS(eeFlags);
          int16_t val_displayed = CFN_PARAM(cfn);
          int16_t val_min = 0;
          int16_t val_max = 255;
          if (func == FUNC_RESET) {
            val_max = FUNC_RESET_PARAM_FIRST_TELEM+lastUsedTelemetryIndex();
            int param = CFN_PARAM(cfn);
            if (param < FUNC_RESET_PARAM_FIRST_TELEM) {
              lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_VFSWRESET, param, attr);
            }
            else {
              TelemetrySensor * sensor = & g_model.telemetrySensors[param-FUNC_RESET_PARAM_FIRST_TELEM];
              lcdDrawSizedText(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, sensor->label, TELEM_LABEL_LEN, attr|ZCHAR);
            }
            if (active) INCDEC_ENABLE_CHECK(functionsContext == &globalFunctionsContext ? isSourceAvailableInGlobalResetSpecialFunction : isSourceAvailableInResetSpecialFunction);
          }
#if defined(OVERRIDE_CHANNEL_FUNCTION)
          else if (func == FUNC_OVERRIDE_CHANNEL) {
            val_min = -LIMIT_EXT_PERCENT; val_max = +LIMIT_EXT_PERCENT;
            lcdDrawNumber(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
          }
#endif
          else if (func >= FUNC_SET_FAILSAFE && func <= FUNC_BIND) {
            val_max = NUM_MODULES-1;
            lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, "\004Int.Ext.", CFN_PARAM(cfn), attr);
          }
          else if (func == FUNC_SET_TIMER) {
            val_max = 59*60+59;
            putsTimer(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT, attr);
          }
          else if (func == FUNC_PLAY_SOUND) {
            val_max = AU_FRSKY_LAST-AU_FRSKY_FIRST-1;
            lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_FUNCSOUNDS, val_displayed, attr);
          }
#if defined(HAPTIC)
          else if (func == FUNC_HAPTIC) {
            val_max = 3;
            lcdDrawNumber(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
          }
#endif
#if defined(SDCARD)
          else if (func == FUNC_PLAY_TRACK || func == FUNC_BACKGND_MUSIC || func == FUNC_PLAY_SCRIPT) {
            coord_t x = MODEL_CUSTOM_FUNC_3RD_COLUMN;
            if (ZEXIST(cfn->play.name))
              lcdDrawSizedText(x, y, cfn->play.name, sizeof(cfn->play.name), attr);
            else
              lcdDrawTextAtIndex(x, y, STR_VCSWFUNC, 0, attr);
            if (active && event==EVT_KEY_BREAK(KEY_ENTER)) {
              s_editMode = 0;
              char directory[256];
              if (func==FUNC_PLAY_SCRIPT) {
                strcpy(directory, SCRIPTS_FUNCS_PATH);
              }
              else {
                strcpy(directory, SOUNDS_PATH);
                strncpy(directory+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
              }
              if (sdListFiles(directory, func==FUNC_PLAY_SCRIPT ? SCRIPTS_EXT : SOUNDS_EXT, sizeof(cfn->play.name), cfn->play.name)) {
                menuHandler = onCustomFunctionsFileSelectionMenu;
              }
              else {
                POPUP_WARNING(func==FUNC_PLAY_SCRIPT ? STR_NO_SCRIPTS_ON_SD : STR_NO_SOUNDS_ON_SD);
                s_menu_flags = 0;
              }
            }
            break;
          }
          else if (func == FUNC_PLAY_VALUE) {
            val_max = MIXSRC_LAST_TELEM;
            putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
            if (active) {
              INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
              INCDEC_ENABLE_CHECK(functionsContext == &globalFunctionsContext ? isSourceAvailableInGlobalFunctions : isSourceAvailable);
            }
          }
#endif
          else if (func == FUNC_VOLUME) {
            val_max = MIXSRC_LAST_CH;
            putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
            if (active) {
              INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
              INCDEC_ENABLE_CHECK(isSourceAvailable);
            }
          }
          else if (func == FUNC_LOGS) {
            if (val_displayed) {
              lcdDrawNumber(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|PREC1|LEFT);
              lcdDrawChar(lcdLastPos, y, 's');
            }
            else {
              lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_MMMINV, 0, attr);
            }
          }
#if defined(REVPLUS)
          else if (func == FUNC_BACKLIGHT) {
            drawSlider(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, CFN_PARAM(cfn), 100, attr);
            if (active) INCDEC_SET_FLAG(eeFlags | NO_INCDEC_MARKS);
            val_min = 0;
            val_max = 100;
          }
#endif
#if defined(GVARS)
          else if (func == FUNC_ADJUST_GVAR) {
            switch (CFN_GVAR_MODE(cfn)) {
              case FUNC_ADJUST_GVAR_CONSTANT:
                val_displayed = (int16_t)CFN_PARAM(cfn);
                val_min = -CFN_GVAR_CST_MAX; val_max = +CFN_GVAR_CST_MAX;
                lcdDrawNumber(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
                break;
              case FUNC_ADJUST_GVAR_SOURCE:
                val_max = MIXSRC_LAST_CH;
                putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
                if (active) {
                  INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
                  INCDEC_ENABLE_CHECK(isSourceAvailable);
                }
                break;
              case FUNC_ADJUST_GVAR_GVAR:
                val_max = MAX_GVARS-1;
                putsStrIdx(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed+1, attr);
                break;
              default: // FUNC_ADJUST_GVAR_INC
#if 0 // TODO 2.2.X
                val_min = -100; val_max = +100;
                if (val_displayed < 0)
                  lcdDrawText(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, "-=", attr);
                else
                  lcdDrawText(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, "+=", attr);
                lcdDrawNumber(lcdNextPos, y, abs(val_displayed), attr|LEFT);
#endif
                val_max = 1;
                lcdDrawTextAtIndex(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, PSTR("\003-=1+=1"), val_displayed, attr);
                break;
            }
          }
#endif
          else if (attr) {
            REPEAT_LAST_CURSOR_MOVE();
          }

          if (active || event==EVT_KEY_LONG(KEY_ENTER)) {
            CFN_PARAM(cfn) = CHECK_INCDEC_PARAM(event, val_displayed, val_min, val_max);
            if (func == FUNC_ADJUST_GVAR && attr && event==EVT_KEY_LONG(KEY_ENTER)) {
              killEvents(event);
              if (CFN_GVAR_MODE(cfn) != FUNC_ADJUST_GVAR_CONSTANT)
                MENU_ADD_ITEM(STR_CONSTANT);
              if (CFN_GVAR_MODE(cfn) != FUNC_ADJUST_GVAR_SOURCE)
                MENU_ADD_ITEM(STR_MIXSOURCE);
              if (CFN_GVAR_MODE(cfn) != FUNC_ADJUST_GVAR_GVAR)
                MENU_ADD_ITEM(STR_GLOBALVAR);
              if (CFN_GVAR_MODE(cfn) != FUNC_ADJUST_GVAR_INC)
                MENU_ADD_ITEM(STR_INCDEC);
              menuHandler = onAdjustGvarSourceLongEnterPress;
              s_editMode = EDIT_MODIFY_FIELD;
            }
          }
          break;
        }

        case 4:
          if (HAS_ENABLE_PARAM(func)) {
            menu_lcd_onoff(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF, y, CFN_ACTIVE(cfn), attr);
            if (active) CFN_ACTIVE(cfn) = checkIncDec(event, CFN_ACTIVE(cfn), 0, 1, eeFlags);
          }
          else if (HAS_REPEAT_PARAM(func)) {
            if (CFN_PLAY_REPEAT(cfn) == 0) {
              lcdDrawText(MODEL_CUSTOM_FUNC_4TH_COLUMN+2, y, "1x", attr);
            }
            else if (CFN_PLAY_REPEAT(cfn) == CFN_PLAY_REPEAT_NOSTART) {
              lcdDrawChar(MODEL_CUSTOM_FUNC_4TH_COLUMN-1, y, '!', attr);
              lcdDrawText(MODEL_CUSTOM_FUNC_4TH_COLUMN+2, y, "1x", attr);
            }
            else {
              lcdDrawNumber(MODEL_CUSTOM_FUNC_4TH_COLUMN+2+FW, y, CFN_PLAY_REPEAT(cfn)*CFN_PLAY_REPEAT_MUL, attr);
              lcdDrawChar(MODEL_CUSTOM_FUNC_4TH_COLUMN+2+FW, y, 's', attr);
            }
            if (active) CFN_PLAY_REPEAT(cfn) = checkIncDec(event, CFN_PLAY_REPEAT(cfn)==CFN_PLAY_REPEAT_NOSTART?-1:CFN_PLAY_REPEAT(cfn), -1, 60/CFN_PLAY_REPEAT_MUL, eeFlags);
          }
          else if (attr) {
            REPEAT_LAST_CURSOR_MOVE();
          }
          break;
      }
    }
  }
}

void menuModelCustomFunctions(uint8_t event)
{
  MENU(STR_MENUCUSTOMFUNC, menuTabModel, e_CustomFunctions, NUM_CFN, { NAVIGATION_LINE_BY_LINE|4/*repeated*/ });
  return menuCustomFunctions(event, g_model.customFn, &modelFunctionsContext);
}
