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

#define MODEL_CUSTOM_FUNC_1ST_COLUMN          (3)
#define MODEL_CUSTOM_FUNC_2ND_COLUMN          (5*FW-2)
#define MODEL_CUSTOM_FUNC_3RD_COLUMN          (15*FW+2)
#define MODEL_CUSTOM_FUNC_4TH_COLUMN          (20*FW)
#if defined(GRAPHICS)
  #define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF  (20*FW)
#else
  #define MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF  (18*FW+2)
#endif

#if defined(CPUARM) && defined(SDCARD)
void onCustomFunctionsFileSelectionMenu(const char *result)
{
  int  sub = m_posVert - 1;
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
    if (!listSdFiles(directory, func==FUNC_PLAY_SCRIPT ? SCRIPTS_EXT : SOUNDS_EXT, sizeof(cfn->play.name), NULL)) {
      POPUP_WARNING(func==FUNC_PLAY_SCRIPT ? STR_NO_SCRIPTS_ON_SD : STR_NO_SOUNDS_ON_SD);
      s_menu_flags = 0;
    }
  }
  else {
    // The user choosed a file in the list
    memcpy(cfn->play.name, result, sizeof(cfn->play.name));
    eeDirty(eeFlags);
  }
}
#endif

void menuCustomFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext)
{
  int8_t sub = m_posVert - 1;

#if defined(CPUARM)
  uint8_t eeFlags = (functions == g_model.customFn) ? EE_MODEL : EE_GENERAL;
#elif !defined(CPUM64) || defined(AUTOSWITCH)
  uint8_t eeFlags = EE_MODEL;
#endif

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+s_pgOfs;

    CustomFunctionData *cfn = &functions[k];
    uint8_t func = CFN_FUNC(cfn);
    for (uint8_t j=0; j<5; j++) {
      uint8_t attr = ((sub==k && m_posHorz==j) ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
      uint8_t active = (attr && (s_editMode>0 || p1valdiff));
      switch (j) {
        case 0:
          putsSwitches(MODEL_CUSTOM_FUNC_1ST_COLUMN, y, CFN_SWITCH(cfn), attr | ((functionsContext->activeSwitches & ((MASK_CFN_TYPE)1 << k)) ? BOLD : 0));
          if (active || AUTOSWITCH_ENTER_LONG()) CHECK_INCDEC_SWITCH(event, CFN_SWITCH(cfn), SWSRC_FIRST, SWSRC_LAST, eeFlags, isSwitchAvailableInCustomFunctions);
#if defined(CPUARM)
          if (func == FUNC_OVERRIDE_CHANNEL && functions != g_model.customFn) {
            func = CFN_FUNC(cfn) = func+1;
          }
#endif
          break;

        case 1:
          if (CFN_SWITCH(cfn)) {
            lcd_putsiAtt(MODEL_CUSTOM_FUNC_2ND_COLUMN, y, STR_VFSWFUNC, func, attr);
            if (active) {
#if defined(CPUARM)
              CFN_FUNC(cfn) = checkIncDec(event, CFN_FUNC(cfn), 0, FUNC_MAX-1, eeFlags, isAssignableFunctionAvailable);
#else
              CHECK_INCDEC_MODELVAR_ZERO(event, CFN_FUNC(cfn), FUNC_MAX-1);
#endif
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
#if defined(CPUARM)
            putsMixerSource(lcdNextPos, y, CFN_CH_INDEX(cfn)==0 ? 0 : MIXSRC_Rud+CFN_CH_INDEX(cfn)-1, attr);
#else
            putsMixerSource(lcdNextPos, y, MIXSRC_Rud+CFN_CH_INDEX(cfn)-1, attr);
#endif
          }
#if defined(GVARS)
          else if (func == FUNC_ADJUST_GVAR) {
            maxParam = MAX_GVARS-1;
            putsStrIdx(lcdNextPos, y, STR_GV, CFN_GVAR_INDEX(cfn)+1, attr);
#if defined(CPUARM)
            if (active) CFN_GVAR_INDEX(cfn) = checkIncDec(event, CFN_GVAR_INDEX(cfn), 0, maxParam, eeFlags);
#else
            if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_GVAR_INDEX(cfn), maxParam);
#endif
            break;
          }
#endif
#if defined(CPUARM)
          else if (func == FUNC_SET_TIMER) {
            maxParam = MAX_TIMERS-1;
            lcd_putsiAtt(lcdNextPos, y, STR_VFSWRESET, CFN_TIMER_INDEX(cfn), attr);
            if (active) CFN_TIMER_INDEX(cfn) = checkIncDec(event, CFN_TIMER_INDEX(cfn), 0, maxParam, eeFlags);
            break;
          }
#endif
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
#if defined(CPUARM)
          int16_t val_min = 0;
          int16_t val_max = 255;
#else
          int8_t val_min = 0;
          uint8_t val_max = 255;
#endif
          if (func == FUNC_RESET) {
            val_max = FUNC_RESET_PARAM_LAST;
            lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_VFSWRESET, CFN_PARAM(cfn), attr);
          }
#if defined(OVERRIDE_CHANNEL_FUNCTION)
          else if (func == FUNC_OVERRIDE_CHANNEL) {
#if !defined(CPUARM)
            val_displayed = (int8_t)CFN_PARAM(cfn);
#endif
            val_min = -LIMIT_EXT_PERCENT; val_max = +LIMIT_EXT_PERCENT;
            lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
          }
#endif
#if defined(CPUARM)
          else if (func >= FUNC_SET_FAILSAFE && func <= FUNC_BIND) {
            val_max = NUM_MODULES-1;
            lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, "\004Int.Ext.", CFN_PARAM(cfn), attr);
          }
#endif
#if defined(CPUARM)
          else if (func == FUNC_SET_TIMER) {
            val_max = 59*60+59;
            putsTimer(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT, attr);
          }
#endif
#if defined(AUDIO)
          else if (func == FUNC_PLAY_SOUND) {
            val_max = AU_FRSKY_LAST-AU_FRSKY_FIRST-1;
            lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_FUNCSOUNDS, val_displayed, attr);
          }
#endif
#if defined(HAPTIC)
          else if (func == FUNC_HAPTIC) {
            val_max = 3;
            lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
          }
#endif
#if defined(CPUARM) && defined(SDCARD)
          else if (func == FUNC_PLAY_TRACK || func == FUNC_BACKGND_MUSIC || func == FUNC_PLAY_SCRIPT) {
            coord_t x = (func == FUNC_PLAY_TRACK ? MODEL_CUSTOM_FUNC_2ND_COLUMN + FW + FW*strlen(TR_PLAY_TRACK) : MODEL_CUSTOM_FUNC_3RD_COLUMN);
            if (ZEXIST(cfn->play.name))
              lcd_putsnAtt(x, y, cfn->play.name, sizeof(cfn->play.name), attr);
            else
              lcd_putsiAtt(x, y, STR_VCSWFUNC, 0, attr);
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
              if (listSdFiles(directory, func==FUNC_PLAY_SCRIPT ? SCRIPTS_EXT : SOUNDS_EXT, sizeof(cfn->play.name), cfn->play.name)) {
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
            INCDEC_ENABLE_CHECK(isSourceAvailable);
          }
#endif
#if defined(CPUARM)
          else if (func == FUNC_VOLUME) {
            val_max = MIXSRC_LAST_CH;
            putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
            INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
            INCDEC_ENABLE_CHECK(isSourceAvailable);
          }
#elif defined(VOICE)
          else if (func == FUNC_PLAY_TRACK) {
#if defined(GVARS)
            if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
              killEvents(event);
              s_editMode = !s_editMode;
              active = true;
              val_displayed = (val_displayed > 250 ? 0 : 251);
            }
            if (val_displayed > 250) {
              putsStrIdx(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed-250, attr);
            }
            else {
              lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
            }
#else
            lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed+PROMPT_CUSTOM_BASE, attr|LEFT);
#endif
          }
          else if (func == FUNC_PLAY_BOTH) {
            lcd_putcAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+3*FWNUM, y, '|', attr);
            lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+3*FWNUM, y, val_displayed+PROMPT_CUSTOM_BASE, attr);
            lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN+2+3*FWNUM, y, (val_displayed+PROMPT_CUSTOM_BASE+1)%10, attr|LEFT);
          }
          else if (func == FUNC_PLAY_VALUE) {
            val_max = MIXSRC_FIRST_TELEM + TELEM_DISPLAY_MAX - 1;
            putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
            INCDEC_ENABLE_CHECK(functionsContext == &globalFunctionsContext ? isSourceAvailableInGlobalFunctions : isSourceAvailable);
          }
#endif
#if defined(SDCARD)
          else if (func == FUNC_LOGS) {
            if (val_displayed) {
              lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|PREC1|LEFT);
              lcd_putc(lcdLastPos, y, 's');
            }
            else {
              lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_MMMINV, 0, attr);
            }
          }
#endif
#if defined(GVARS)
          else if (func == FUNC_ADJUST_GVAR) {
            switch (CFN_GVAR_MODE(cfn)) {
              case FUNC_ADJUST_GVAR_CONSTANT:
                val_displayed = (int16_t)CFN_PARAM(cfn);
                val_min = -CFN_GVAR_CST_MAX; val_max = +CFN_GVAR_CST_MAX;
                lcd_outdezAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr|LEFT);
                break;
              case FUNC_ADJUST_GVAR_SOURCE:
                val_max = MIXSRC_LAST_CH;
                putsMixerSource(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, val_displayed, attr);
                INCDEC_SET_FLAG(eeFlags | INCDEC_SOURCE);
                INCDEC_ENABLE_CHECK(isSourceAvailable);
                break;
              case FUNC_ADJUST_GVAR_GVAR:
                val_max = MAX_GVARS-1;
                putsStrIdx(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, STR_GV, val_displayed+1, attr);
                break;
              default: // FUNC_ADJUST_GVAR_INC
                val_max = 1;
                lcd_putsiAtt(MODEL_CUSTOM_FUNC_3RD_COLUMN, y, PSTR("\003-=1+=1"), val_displayed, attr);
                break;
            }

            if (attr && event==EVT_KEY_LONG(KEY_ENTER)) {
              killEvents(event);
              s_editMode = !s_editMode;
              active = true;
              CFN_GVAR_MODE(cfn) += 1;
#if defined(CPUARM)
              CFN_GVAR_MODE(cfn) &= 0x03;
#endif
              val_displayed = 0;
            }
          }
#endif
          else if (attr) {
            REPEAT_LAST_CURSOR_MOVE();
          }

          if (active) {
            CFN_PARAM(cfn) = CHECK_INCDEC_PARAM(event, val_displayed, val_min, val_max);
          }
          break;
        }

        case 4:
          if (HAS_ENABLE_PARAM(func)) {
            menu_lcd_onoff(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF, y, CFN_ACTIVE(cfn), attr);
#if defined(CPUARM)
            if (active) CFN_ACTIVE(cfn) = checkIncDec(event, CFN_ACTIVE(cfn), 0, 1, eeFlags);
#else
            if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_ACTIVE(cfn), 1);
#endif
          }
          else if (HAS_REPEAT_PARAM(func)) {
            if (CFN_PLAY_REPEAT(cfn) == 0) {
              lcd_putcAtt(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF+3, y, '-', attr);
            }
#if defined(CPUARM)
            else if (CFN_PLAY_REPEAT(cfn) == CFN_PLAY_REPEAT_NOSTART) {
              lcd_putsAtt(MODEL_CUSTOM_FUNC_4TH_COLUMN_ONOFF, y, "!-", attr);
            }
#endif
            else {
              lcd_outdezAtt(MODEL_CUSTOM_FUNC_4TH_COLUMN+2+FW, y, CFN_PLAY_REPEAT(cfn)*CFN_PLAY_REPEAT_MUL, attr);
            }
#if defined(CPUARM)
            if (active) CFN_PLAY_REPEAT(cfn) = checkIncDec(event, CFN_PLAY_REPEAT(cfn)==CFN_PLAY_REPEAT_NOSTART?-1:CFN_PLAY_REPEAT(cfn), -1, 60/CFN_PLAY_REPEAT_MUL, eeFlags);
#else
            if (active) CHECK_INCDEC_MODELVAR_ZERO(event, CFN_PLAY_REPEAT(cfn), 60/CFN_PLAY_REPEAT_MUL);
#endif
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
  MENU(STR_MENUCUSTOMFUNC, menuTabModel, e_CustomFunctions, NUM_CFN+1, {0, NAVIGATION_LINE_BY_LINE|4/*repeated*/});
  return menuCustomFunctions(event, g_model.customFn, &modelFunctionsContext);
}
