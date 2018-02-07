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

void displayFlightModes(coord_t x, coord_t y, FlightModesType value)
{
  uint8_t p = MAX_FLIGHT_MODES;
  do {
    --p;
    if (!(value & (1<<p)))
      lcdDrawChar(x, y, '0'+p);
    x -= FWNUM;
  } while (p!=0);
}

#if !defined(CPUARM)
FlightModesType editFlightModes(coord_t x, coord_t y, event_t event, FlightModesType value, uint8_t attr)
{
  drawFieldLabel(x, y, STR_FLMODE);

  uint8_t posHorz = menuHorizontalPosition;

  for (uint8_t p=0; p<MAX_FLIGHT_MODES; p++) {
    lcdDrawChar(x, y, '0'+p, ((posHorz==p) && attr) ? BLINK|INVERS : ((value & (1<<p)) ? 0 : INVERS));
    x += FW;
  }

  if (attr) {
    if (s_editMode && ((event==EVT_KEY_BREAK(KEY_ENTER) || p1valdiff))) {
      s_editMode = 0;
      value ^= (1<<posHorz);
      storageDirty(EE_MODEL);
    }
  }

  return value;
}
#endif

enum MenuModelFlightModeItems {
  ITEM_MODEL_FLIGHT_MODE_NAME,
  ITEM_MODEL_FLIGHT_MODE_SWITCH,
  ITEM_MODEL_FLIGHT_MODE_TRIMS,
  IF_ROTARY_ENCODERS(ITEM_MODEL_FLIGHT_MODE_ROTARY_ENCODERS)
  ITEM_MODEL_FLIGHT_MODE_FADE_IN,
  ITEM_MODEL_FLIGHT_MODE_FADE_OUT,
#if defined(GVARS) && !defined(PCBSTD)
  ITEM_MODEL_FLIGHT_MODE_GVARS_LABEL,
  ITEM_MODEL_FLIGHT_MODE_GV1,
  ITEM_MODEL_FLIGHT_MODE_GV2,
  ITEM_MODEL_FLIGHT_MODE_GV3,
  ITEM_MODEL_FLIGHT_MODE_GV4,
  ITEM_MODEL_FLIGHT_MODE_GV5,
#if defined(CPUARM)
  ITEM_MODEL_FLIGHT_MODE_GV6,
  ITEM_MODEL_FLIGHT_MODE_GV7,
  ITEM_MODEL_FLIGHT_MODE_GV8,
  ITEM_MODEL_FLIGHT_MODE_GV9,
#endif
#endif
  ITEM_MODEL_FLIGHT_MODE_MAX
};

bool isTrimModeAvailable(int mode)
{
  return (mode < 0 || (mode%2) == 0 || (mode/2) != s_currIdx);
}

void menuModelFlightModeOne(event_t event)
{
  FlightModeData * fm = flightModeAddress(s_currIdx);
  drawFlightMode(13*FW, 0, s_currIdx+1, (getFlightMode()==s_currIdx ? BOLD : 0));

#if defined(GVARS) && !defined(GVARS_IN_CURVES_SCREEN)
#if defined(PCBTARANIS)
  #define VERTICAL_SHIFT  (ITEM_MODEL_FLIGHT_MODE_FADE_IN-ITEM_MODEL_FLIGHT_MODE_TRIMS)
  static const pm_uint8_t mstate_tab_fm1[] PROGMEM = {0, 3, 0, 0, (uint8_t)-1, 1, 1, 1, 1, 1, 1};
#else
  #define VERTICAL_SHIFT  (ITEM_MODEL_FLIGHT_MODE_FADE_IN-ITEM_MODEL_FLIGHT_MODE_SWITCH)
  static const pm_uint8_t mstate_tab_fm1[] PROGMEM = {0, 0, 0, (uint8_t)-1, 1, 1, 1, 1, 1};
#endif
  static const pm_uint8_t mstate_tab_others[] PROGMEM = {0, 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0, 0, (uint8_t)-1, 2, 2, 2, 2, 2};

  check(event, 0, NULL, 0, (s_currIdx == 0) ? mstate_tab_fm1 : mstate_tab_others, DIM(mstate_tab_others)-1, ITEM_MODEL_FLIGHT_MODE_MAX - HEADER_LINE - (s_currIdx==0 ? (ITEM_MODEL_FLIGHT_MODE_FADE_IN-ITEM_MODEL_FLIGHT_MODE_SWITCH-1) : 0));

  TITLE(STR_MENUFLIGHTMODE);

  #define PHASE_ONE_FIRST_LINE (1+1*FH)
#else
  SUBMENU(STR_MENUFLIGHTMODE, 3 + (s_currIdx==0 ? 0 : 2 + (bool)NUM_ROTARY_ENCODERS), {0, 0, 3, IF_ROTARY_ENCODERS(NUM_ROTARY_ENCODERS-1) 0/*, 0*/});
  #define PHASE_ONE_FIRST_LINE (1+1*FH)
#endif

  int8_t sub = menuVerticalPosition;
  int8_t editMode = s_editMode;

#if defined(GVARS) && !defined(PCBSTD)
  if (s_currIdx == 0 && sub>=ITEM_MODEL_FLIGHT_MODE_SWITCH) sub += VERTICAL_SHIFT;

  for (uint8_t k=0; k<LCD_LINES-1; k++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
    int8_t i = k + menuVerticalOffset;

    if (s_currIdx == 0 && i>=ITEM_MODEL_FLIGHT_MODE_SWITCH) i += VERTICAL_SHIFT;
    uint8_t attr = (sub==i ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
#else
  for (uint8_t i=0, k=0, y=PHASE_ONE_FIRST_LINE; i<ITEM_MODEL_FLIGHT_MODE_MAX; i++, k++, y+=FH) {
    if (s_currIdx == 0 && i==ITEM_MODEL_FLIGHT_MODE_SWITCH) i = ITEM_MODEL_FLIGHT_MODE_FADE_IN;
    uint8_t attr = (sub==k ? (editMode>0 ? BLINK|INVERS : INVERS) : 0);
#endif
    switch (i) {
      case ITEM_MODEL_FLIGHT_MODE_NAME:
        editSingleName(MIXES_2ND_COLUMN, y, STR_PHASENAME, fm->name, sizeof(fm->name), event, attr);
        break;

      case ITEM_MODEL_FLIGHT_MODE_SWITCH:
        fm->swtch = editSwitch(MIXES_2ND_COLUMN, y, fm->swtch, attr, event);
        break;

      case ITEM_MODEL_FLIGHT_MODE_TRIMS:
        lcdDrawTextAlignedLeft(y, STR_TRIMS);
#if defined(CPUARM)
        for (uint8_t t = 0; t < NUM_STICKS; t++) {
          drawTrimMode(MIXES_2ND_COLUMN + (t*2*FW), y, s_currIdx, t, menuHorizontalPosition == t ? attr : 0);
          if (s_editMode >= 0 && attr && menuHorizontalPosition == t) {
            trim_t & v = fm->trim[t];
            v.mode = checkIncDec(event, v.mode==TRIM_MODE_NONE ? -1 : v.mode, -1, k==0 ? 0 : 2*MAX_FLIGHT_MODES-1, EE_MODEL, isTrimModeAvailable);
          }
        }
#else
        for (uint8_t t = 0; t < NUM_STICKS; t++) {
          drawTrimMode(MIXES_2ND_COLUMN + (t*FW), y, s_currIdx, t, menuHorizontalPosition == t ? attr : 0);
          if (attr && menuHorizontalPosition == t && ((editMode > 0) || p1valdiff)) {
            int16_t v = getRawTrimValue(s_currIdx, t);
            if (v < TRIM_EXTENDED_MAX)
              v = TRIM_EXTENDED_MAX;
            v = checkIncDec(event, v, TRIM_EXTENDED_MAX, TRIM_EXTENDED_MAX + MAX_FLIGHT_MODES - 1, EE_MODEL);
            if (checkIncDec_Ret) {
              if (v == TRIM_EXTENDED_MAX)
                v = 0;
              setTrimValue(s_currIdx, t, v);
            }
          }
        }
#endif
        break;

#if ROTARY_ENCODERS > 0
      case ITEM_MODEL_FLIGHT_MODE_ROTARY_ENCODERS:
        lcdDrawTextAlignedLeft(y, STR_ROTARY_ENCODER);
        for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
          putsRotaryEncoderMode(MIXES_2ND_COLUMN+(t*FW), y, s_currIdx, t, menuHorizontalPosition==t ? attr : 0);
          if (attr && menuHorizontalPosition==t && ((editMode>0) || p1valdiff)) {
            int16_t v = flightModeAddress(s_currIdx)->rotaryEncoders[t];
            if (v < ROTARY_ENCODER_MAX) v = ROTARY_ENCODER_MAX;
            v = checkIncDec(event, v, ROTARY_ENCODER_MAX, ROTARY_ENCODER_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
            if (checkIncDec_Ret) {
              if (v == ROTARY_ENCODER_MAX) v = 0;
              flightModeAddress(s_currIdx)->rotaryEncoders[t] = v;
            }
          }
        }
        break;
#endif

      case ITEM_MODEL_FLIGHT_MODE_FADE_IN:
        fm->fadeIn = EDIT_DELAY(0, y, event, attr, STR_FADEIN, fm->fadeIn);
        break;

      case ITEM_MODEL_FLIGHT_MODE_FADE_OUT:
        fm->fadeOut = EDIT_DELAY(0, y, event, attr, STR_FADEOUT, fm->fadeOut);
        break;

#if defined(GVARS) && !defined(PCBSTD)
      case ITEM_MODEL_FLIGHT_MODE_GVARS_LABEL:
        lcdDrawTextAlignedLeft(y, STR_GLOBAL_VARS);
        break;

      default:
      {
        uint8_t idx = i-ITEM_MODEL_FLIGHT_MODE_GV1;
        uint8_t posHorz = menuHorizontalPosition;

        if (attr && posHorz > 0 && s_currIdx==0) posHorz++;

        drawStringWithIndex(INDENT_WIDTH, y, STR_GV, idx+1, posHorz==0 ? attr : 0);
#if defined(CPUARM)
        lcdDrawSizedText(4*FW, y,g_model.gvars[idx].name, LEN_GVAR_NAME, ZCHAR);
        if (attr && editMode>0 && posHorz==0) {
          s_currIdx = sub - ITEM_MODEL_FLIGHT_MODE_GV1;
          editMode = 0;
          pushMenu(menuModelGVarOne);
        }
#else
        editName(4*FW, y, g_model.gvars[idx].name, LEN_GVAR_NAME, event, posHorz==0 ? attr : 0);
#endif
        int16_t v = fm->gvars[idx];
        if (v > GVAR_MAX) {
          uint8_t p = v - GVAR_MAX - 1;
          if (p >= s_currIdx) p++;
          drawFlightMode(9*FW, y, p+1, posHorz==1 ? attr : 0);
        }
        else {
          lcdDrawText(9*FW, y, STR_OWN, posHorz==1 ? attr : 0);
        }
        if (attr && s_currIdx>0 && posHorz==1 && (editMode>0 || p1valdiff)) {
          if (v < GVAR_MAX) v = GVAR_MAX;
          v = checkIncDec(event, v, GVAR_MAX, GVAR_MAX+MAX_FLIGHT_MODES-1, EE_MODEL);
          if (checkIncDec_Ret) {
            if (v == GVAR_MAX) v = 0;
            fm->gvars[idx] = v;
          }
        }
#if defined(CPUARM)
        editGVarValue(17*FW, y, event, idx, getGVarFlightMode(s_currIdx, idx), posHorz==2 ? attr : 0);
#else
        uint8_t p = getGVarFlightMode(s_currIdx, idx);
        lcdDrawNumber(18*FW, y, GVAR_VALUE(idx, p), posHorz==2 ? attr : 0);
        if (attr && posHorz==2 && ((editMode>0) || p1valdiff)) {
          GVAR_VALUE(idx, p) = checkIncDec(event, GVAR_VALUE(idx, p), -500, 500, EE_MODEL);
        }
#endif
        break;
      }
#endif
    }
  }
}

#if defined(ROTARY_ENCODERS)
  #if ROTARY_ENCODERS > 2
    #define NAME_OFS                   (-4-12)
    #define SWITCH_OFS                 (-FW/2-2-13)
    #define TRIMS_OFS                  (-FW/2-4-15)
    #define ROTARY_ENC_OFS             (0)
  #else
    #define NAME_OFS                   (-4)
    #define SWITCH_OFS                 (-FW/2-2)
    #define TRIMS_OFS                  (-FW/2-4)
    #define ROTARY_ENC_OFS             (2)
  #endif
#elif defined(PCBTARANIS)
  #define NAME_POS                     20
  #define SWITCH_POS                   59
  #define TRIMS_POS                    79
#else
  #define NAME_OFS                     0
  #define SWITCH_OFS                   (FW/2)
  #define TRIMS_OFS                    (FW/2)
#endif

void menuModelFlightModesAll(event_t event)
{
  SIMPLE_MENU(STR_MENUFLIGHTMODES, menuTabModel, MENU_MODEL_FLIGHT_MODES, HEADER_LINE+MAX_FLIGHT_MODES+1);

  int8_t sub = menuVerticalPosition - HEADER_LINE;

  switch (event) {
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_FIRST(KEY_ENTER):
      if (sub == MAX_FLIGHT_MODES) {
        s_editMode = 0;
        trimsCheckTimer = 200; // 2 seconds
      }
      // no break
#if !defined(PCBX7)
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
      if (sub >= 0 && sub < MAX_FLIGHT_MODES) {
        s_currIdx = sub;
        pushMenu(menuModelFlightModeOne);
      }
      break;
  }

  uint8_t att;
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
#if defined(CPUARM)
    int8_t y = 1 + (1+i-menuVerticalOffset)*FH;
    if (y<1*FH+1 || y>(LCD_LINES-1)*FH+1) continue;
#else
    uint8_t y = 1 + (i+1)*FH;
#endif
    att = (i==sub ? INVERS : 0);
    FlightModeData * p = flightModeAddress(i);
    drawFlightMode(0, y, i+1, att|(getFlightMode()==i ? BOLD : 0));
#if defined(PCBTARANIS)
    lcdDrawSizedText(NAME_POS, y, p->name, sizeof(p->name), ZCHAR);
#else
    lcdDrawSizedText(4*FW+NAME_OFS, y, p->name, sizeof(p->name), ZCHAR);
#endif
    if (i == 0) {
      for (uint8_t t=0; t<NUM_STICKS; t++) {
#if defined(PCBTARANIS)
        drawTrimMode(TRIMS_POS+t*FW*2, y, i, t, 0);
#else
        drawShortTrimMode((9+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS, y, i, t, 0);
#endif
      }
    }
    else {
#if defined(PCBTARANIS)
      drawSwitch(SWITCH_POS, y, p->swtch, 0);
      for (uint8_t t=0; t<NUM_STICKS; t++) {
        drawTrimMode(TRIMS_POS+t*FW*2, y, i, t, 0);
      }
#else
      drawSwitch((4+LEN_FLIGHT_MODE_NAME)*FW+SWITCH_OFS, y, p->swtch, 0);
      for (uint8_t t=0; t<NUM_STICKS; t++) {
        drawShortTrimMode((9+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS, y, i, t, 0);
      }
#endif
#if defined(CPUM2560)
      for (uint8_t t=0; t<NUM_ROTARY_ENCODERS; t++) {
        putsRotaryEncoderMode((13+LEN_FLIGHT_MODE_NAME+t)*FW+TRIMS_OFS+ROTARY_ENC_OFS, y, i, t, 0);
      }
#endif
    }

    if (p->fadeIn || p->fadeOut) {
      lcdDrawChar(LCD_W-FW, y, (p->fadeIn && p->fadeOut) ? '*' : (p->fadeIn ? 'I' : 'O'));
    }
  }

#if defined(CPUARM)
  if (menuVerticalOffset != MAX_FLIGHT_MODES-(LCD_LINES-2)) return;
#endif

  lcdDrawTextAlignedLeft((LCD_LINES-1)*FH+1, STR_CHECKTRIMS);
  drawFlightMode(OFS_CHECKTRIMS, (LCD_LINES-1)*FH+1, mixerCurrentFlightMode+1);
  if (sub==MAX_FLIGHT_MODES && !trimsCheckTimer) {
    lcdInvertLastLine();
  }
}
