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

vertpos_t menuVerticalOffset;
int8_t s_editMode;
uint8_t noHighlightCounter;
uint8_t menuCalibrationState;
vertpos_t menuVerticalPosition;
horzpos_t menuHorizontalPosition;

#if defined(NAVIGATION_POT1)
int16_t p1valdiff;
#endif

#if defined(NAVIGATION_POT2)
int8_t p2valdiff;
#endif

int8_t  checkIncDec_Ret;

#if defined(PCBXLITE)
// invert the value
#define DBLKEYS_PRESSED_RGT_LFT(in)    ((in & ((1<<KEY_SHIFT) + (1<<KEY_UP))) == ((1<<KEY_SHIFT) + (1<<KEY_UP)))
// set to 0
#define DBLKEYS_PRESSED_UP_DWN(in)     ((in & ((1<<KEY_SHIFT) + (1<<KEY_DOWN))) ==  ((1<<KEY_SHIFT) + (1<<KEY_DOWN)))
// set to max
#define DBLKEYS_PRESSED_RGT_UP(in)     ((in & ((1<<KEY_SHIFT) + (1<<KEY_RIGHT))) == ((1<<KEY_SHIFT) + (1<<KEY_RIGHT)))
// set to min
#define DBLKEYS_PRESSED_LFT_DWN(in)    ((in & ((1<<KEY_SHIFT) + (1<<KEY_LEFT))) == ((1<<KEY_SHIFT) + (1<<KEY_LEFT)))
#elif defined(PCBX7)
#define DBLKEYS_PRESSED_RGT_LFT(in)    (false)
#define DBLKEYS_PRESSED_UP_DWN(in)     (false)
#define DBLKEYS_PRESSED_RGT_UP(in)     (false)
#define DBLKEYS_PRESSED_LFT_DWN(in)    (false)
#else
#define DBLKEYS_PRESSED_RGT_LFT(in)    ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT)) == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_LEFT))
#define DBLKEYS_PRESSED_UP_DWN(in)     ((in & (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_UP + KEYS_GPIO_PIN_DOWN))
#define DBLKEYS_PRESSED_RGT_UP(in)     ((in & (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))  == (KEYS_GPIO_PIN_RIGHT + KEYS_GPIO_PIN_UP))
#define DBLKEYS_PRESSED_LFT_DWN(in)    ((in & (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN)) == (KEYS_GPIO_PIN_LEFT + KEYS_GPIO_PIN_DOWN))
#endif

#if defined(CPUARM)
INIT_STOPS(stops100, 3, -100, 0, 100)
INIT_STOPS(stops1000, 3, -1000, 0, 1000)
INIT_STOPS(stopsSwitch, 15, SWSRC_FIRST, CATEGORY_END(-SWSRC_FIRST_LOGICAL_SWITCH), CATEGORY_END(-SWSRC_FIRST_TRIM), CATEGORY_END(-SWSRC_LAST_SWITCH+1), 0, CATEGORY_END(SWSRC_LAST_SWITCH), CATEGORY_END(SWSRC_FIRST_TRIM-1), CATEGORY_END(SWSRC_FIRST_LOGICAL_SWITCH-1), SWSRC_LAST)

#if defined(PCBTARANIS)
int checkIncDecSelection = 0;

void onSourceLongEnterPress(const char * result)
{
  if (result == STR_MENU_INPUTS)
    checkIncDecSelection = getFirstAvailable(MIXSRC_FIRST_INPUT, MIXSRC_LAST_INPUT, isInputAvailable)+1;
#if defined(LUA_MODEL_SCRIPTS)
  else if (result == STR_MENU_LUA)
    checkIncDecSelection = getFirstAvailable(MIXSRC_FIRST_LUA, MIXSRC_LAST_LUA, isSourceAvailable);
#endif
  else if (result == STR_MENU_STICKS)
    checkIncDecSelection = MIXSRC_FIRST_STICK;
  else if (result == STR_MENU_POTS)
    checkIncDecSelection = MIXSRC_FIRST_POT;
  else if (result == STR_MENU_MAX)
    checkIncDecSelection = MIXSRC_MAX;
  else if (result == STR_MENU_HELI)
    checkIncDecSelection = MIXSRC_FIRST_HELI;
  else if (result == STR_MENU_TRIMS)
    checkIncDecSelection = MIXSRC_FIRST_TRIM;
  else if (result == STR_MENU_SWITCHES)
    checkIncDecSelection = MIXSRC_FIRST_SWITCH;
  else if (result == STR_MENU_TRAINER)
    checkIncDecSelection = MIXSRC_FIRST_TRAINER;
  else if (result == STR_MENU_CHANNELS)
    checkIncDecSelection = getFirstAvailable(MIXSRC_FIRST_CH, MIXSRC_LAST_CH, isSourceAvailable);
  else if (result == STR_MENU_GVARS)
    checkIncDecSelection = MIXSRC_FIRST_GVAR;
  else if (result == STR_MENU_TELEMETRY) {
    for (int i = 0; i < MAX_TELEMETRY_SENSORS; i++) {
      TelemetrySensor * sensor = & g_model.telemetrySensors[i];
      if (sensor->isAvailable()) {
        checkIncDecSelection = MIXSRC_FIRST_TELEM + 3*i;
        break;
      }
    }
  }
}

void onSwitchLongEnterPress(const char * result)
{
  if (result == STR_MENU_SWITCHES)
    checkIncDecSelection = SWSRC_FIRST_SWITCH;
  else if (result == STR_MENU_TRIMS)
    checkIncDecSelection = SWSRC_FIRST_TRIM;
  else if (result == STR_MENU_LOGICAL_SWITCHES)
    checkIncDecSelection = SWSRC_FIRST_LOGICAL_SWITCH + getFirstAvailable(0, MAX_LOGICAL_SWITCHES, isLogicalSwitchAvailable);
  else if (result == STR_MENU_OTHER)
    checkIncDecSelection = SWSRC_ON;
  else if (result == STR_MENU_INVERT)
    checkIncDecSelection = SWSRC_INVERT;
}
#endif

#if defined(PCBX7)
int checkIncDec(event_t event, int val, int i_min, int i_max, unsigned int i_flags, IsValueAvailable isValueAvailable, const CheckIncDecStops &stops)
{
  int newval = val;

#if 0 // TODO ? defined(DBLKEYS)
  uint32_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in)) {
      if (!isValueAvailable || isValueAvailable(-val)) {
        newval = -val;
      }
    }
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > stops.max() ? stops.max() : i_max);
      while (isValueAvailable && !isValueAvailable(newval) && newval>i_min) {
        --newval;
      }
    }
    else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < stops.min() ? stops.min() : i_min);
      while (isValueAvailable && !isValueAvailable(newval) && newval<i_max) {
        ++newval;
      }
    }
    else if (DBLKEYS_PRESSED_UP_DWN(in)) {
      newval = 0;
    }
    else {
      dblkey = false;
    }

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
      killEvents(KEY_PAGE);
      killEvents(KEY_MENU);
      killEvents(KEY_ENTER);
      killEvents(KEY_EXIT);
      event = 0;
    }
  }
#endif

  if (s_editMode>0 && event==EVT_ROTARY_RIGHT) {
    newval += min<int>(rotencSpeed, i_max-val);
    while (isValueAvailable && !isValueAvailable(newval) && newval<=i_max) {
      newval++;
    }
    if (newval > i_max) {
      newval = val;
      AUDIO_KEY_ERROR();
    }
  }
  else if (s_editMode>0 && event==EVT_ROTARY_LEFT) {
    newval -= min<int>(rotencSpeed, val-i_min);
    while (isValueAvailable && !isValueAvailable(newval) && newval>=i_min) {
      newval--;
    }
    if (newval < i_min) {
      newval = val;
      AUDIO_KEY_ERROR();
    }
  }

  if (!READ_ONLY() && i_min==0 && i_max==1 && event==EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        unsigned int swtch = abs(getMovedSwitch());
        if (swtch) {
          newval = switchToMix(swtch);
        }
      }
#endif
    }
  }
#endif

  if (newval != val) {
    storageDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  }
  else {
    checkIncDec_Ret = 0;
  }

  if (i_flags & INCDEC_SOURCE) {
    if (event == EVT_KEY_LONG(KEY_ENTER)) {
      killEvents(event);
      checkIncDecSelection = MIXSRC_NONE;

      if (i_min <= MIXSRC_FIRST_INPUT && i_max >= MIXSRC_FIRST_INPUT) {
        if (getFirstAvailable(MIXSRC_FIRST_INPUT, MIXSRC_LAST_INPUT, isInputAvailable) != MIXSRC_NONE) {
          POPUP_MENU_ADD_ITEM(STR_MENU_INPUTS);
        }
      }
#if defined(LUA_MODEL_SCRIPTS)
      if (i_min <= MIXSRC_FIRST_LUA && i_max >= MIXSRC_FIRST_LUA) {
        if (getFirstAvailable(MIXSRC_FIRST_LUA, MIXSRC_LAST_LUA, isSourceAvailable) != MIXSRC_NONE) {
          POPUP_MENU_ADD_ITEM(STR_MENU_LUA);
        }
      }
#endif
      if (i_min <= MIXSRC_FIRST_STICK && i_max >= MIXSRC_FIRST_STICK)      POPUP_MENU_ADD_ITEM(STR_MENU_STICKS);
      if (i_min <= MIXSRC_FIRST_POT && i_max >= MIXSRC_FIRST_POT)          POPUP_MENU_ADD_ITEM(STR_MENU_POTS);
      if (i_min <= MIXSRC_MAX && i_max >= MIXSRC_MAX)                      POPUP_MENU_ADD_ITEM(STR_MENU_MAX);
#if defined(HELI)
      if (i_min <= MIXSRC_FIRST_HELI && i_max >= MIXSRC_FIRST_HELI)        POPUP_MENU_ADD_ITEM(STR_MENU_HELI);
#endif
      if (i_min <= MIXSRC_FIRST_TRIM && i_max >= MIXSRC_FIRST_TRIM)        POPUP_MENU_ADD_ITEM(STR_MENU_TRIMS);
      if (i_min <= MIXSRC_FIRST_SWITCH && i_max >= MIXSRC_FIRST_SWITCH)    POPUP_MENU_ADD_ITEM(STR_MENU_SWITCHES);
      if (i_min <= MIXSRC_FIRST_TRAINER && i_max >= MIXSRC_FIRST_TRAINER)  POPUP_MENU_ADD_ITEM(STR_MENU_TRAINER);
      if (i_min <= MIXSRC_FIRST_CH && i_max >= MIXSRC_FIRST_CH)            POPUP_MENU_ADD_ITEM(STR_MENU_CHANNELS);
      if (i_min <= MIXSRC_FIRST_GVAR && i_max >= MIXSRC_FIRST_GVAR && isValueAvailable(MIXSRC_FIRST_GVAR)) {
        POPUP_MENU_ADD_ITEM(STR_MENU_GVARS);
      }

      if (i_min <= MIXSRC_FIRST_TELEM && i_max >= MIXSRC_FIRST_TELEM) {
        for (int i = 0; i < MAX_TELEMETRY_SENSORS; i++) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->isAvailable()) {
            POPUP_MENU_ADD_ITEM(STR_MENU_TELEMETRY);
            break;
          }
        }
      }
      POPUP_MENU_START(onSourceLongEnterPress);
    }
    if (checkIncDecSelection != 0) {
      newval = checkIncDecSelection;
      if (checkIncDecSelection != MIXSRC_MAX)
        s_editMode = EDIT_MODIFY_FIELD;
      checkIncDecSelection = 0;
    }
  }
  else if (i_flags & INCDEC_SWITCH) {
    if (event == EVT_KEY_LONG(KEY_ENTER)) {
      killEvents(event);
      checkIncDecSelection = SWSRC_NONE;
      if (i_min <= SWSRC_FIRST_SWITCH && i_max >= SWSRC_LAST_SWITCH)       POPUP_MENU_ADD_ITEM(STR_MENU_SWITCHES);
      if (i_min <= SWSRC_FIRST_TRIM && i_max >= SWSRC_LAST_TRIM)           POPUP_MENU_ADD_ITEM(STR_MENU_TRIMS);
      if (i_min <= SWSRC_FIRST_LOGICAL_SWITCH && i_max >= SWSRC_LAST_LOGICAL_SWITCH) {
        for (int i = 0; i < MAX_LOGICAL_SWITCHES; i++) {
          if (isValueAvailable && isValueAvailable(SWSRC_FIRST_LOGICAL_SWITCH+i)) {
            POPUP_MENU_ADD_ITEM(STR_MENU_LOGICAL_SWITCHES);
            break;
          }
        }
      }
      if (isValueAvailable && isValueAvailable(SWSRC_ON))                  POPUP_MENU_ADD_ITEM(STR_MENU_OTHER);
      if (isValueAvailable && isValueAvailable(-newval))                   POPUP_MENU_ADD_ITEM(STR_MENU_INVERT);
      POPUP_MENU_START(onSwitchLongEnterPress);
      s_editMode = EDIT_MODIFY_FIELD;
    }
    if (checkIncDecSelection != 0) {
      newval = (checkIncDecSelection == SWSRC_INVERT ? -newval : checkIncDecSelection);
      s_editMode = EDIT_MODIFY_FIELD;
      checkIncDecSelection = 0;
    }
  }
  return newval;
}
#else
int checkIncDec(event_t event, int val, int i_min, int i_max, unsigned int i_flags, IsValueAvailable isValueAvailable, const CheckIncDecStops &stops)
{
  int newval = val;

#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in)) {
      if (!isValueAvailable || isValueAvailable(-val)) {
        newval = -val;
      }
    }
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > stops.max() ? stops.max() : i_max);
      while (isValueAvailable && !isValueAvailable(newval) && newval>i_min) {
    	--newval;
      }
    }
    else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < stops.min() ? stops.min() : i_min);
      while (isValueAvailable && !isValueAvailable(newval) && newval<i_max) {
        ++newval;
      }
    }
    else if (DBLKEYS_PRESSED_UP_DWN(in)) {
      newval = 0;
    }
    else {
      dblkey = false;
    }

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
      event = 0;
    }
  }
#endif

#if defined(PCBXLITE)
  if (s_editMode > 0) {
    if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)) {
#else
    if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
#endif
      do {
        if (IS_KEY_REPT(event) && (i_flags & INCDEC_REP10)) {
          newval += min(10, i_max-val);
        }
        else {
          newval++;
        }
      } while (isValueAvailable && !isValueAvailable(newval) && newval<=i_max);

      if (newval > i_max) {
        newval = val;
        killEvents(event);
        AUDIO_KEY_ERROR();
      }
    }
#if defined(PCBXLITE)
    else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)) {
#else
    else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
#endif
      do {
        if (IS_KEY_REPT(event) && (i_flags & INCDEC_REP10)) {
          newval -= min(10, val-i_min);
        }
        else {
          newval--;
        }
      } while (isValueAvailable && !isValueAvailable(newval) && newval>=i_min);

      if (newval < i_min) {
        newval = val;
        killEvents(event);
        AUDIO_KEY_ERROR();
      }
    }
#if defined(PCBXLITE)
  }
#endif

  if (!READ_ONLY() && i_min==0 && i_max==1 && (event==EVT_KEY_BREAK(KEY_ENTER) || IS_ROTARY_BREAK(event))) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(NAVIGATION_POT1)
  // change values based on P1
  newval -= p1valdiff;
  p1valdiff = 0;
#endif

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int8_t source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        uint8_t swtch = abs(getMovedSwitch());
        if (swtch) {
          newval = switchToMix(swtch);
        }
      }
#endif
    }
  }
#endif

  if (newval != val) {
    if (!(i_flags & NO_INCDEC_MARKS) && (newval != i_max) && (newval != i_min) && (newval==0 || newval==-100 || newval==+100) && !IS_ROTARY_EVENT(event)) {
      pauseEvents(event); // delay before auto-repeat continues
    }
    AUDIO_KEY_PRESS();
    storageDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  }
  else {
    checkIncDec_Ret = 0;
  }

#if defined(PCBXLITE)
  if (i_flags & INCDEC_SOURCE) {
    if (event == EVT_KEY_LONG(KEY_ENTER) && !IS_SHIFT_PRESSED()) {
      killEvents(event);
      checkIncDecSelection = MIXSRC_NONE;

      if (i_min <= MIXSRC_FIRST_INPUT && i_max >= MIXSRC_FIRST_INPUT) {
        if (getFirstAvailable(MIXSRC_FIRST_INPUT, MIXSRC_LAST_INPUT, isInputAvailable) != MIXSRC_NONE) {
          POPUP_MENU_ADD_ITEM(STR_MENU_INPUTS);
        }
      }
#if defined(LUA_MODEL_SCRIPTS)
      if (i_min <= MIXSRC_FIRST_LUA && i_max >= MIXSRC_FIRST_LUA) {
        if (getFirstAvailable(MIXSRC_FIRST_LUA, MIXSRC_LAST_LUA, isSourceAvailable) != MIXSRC_NONE) {
          POPUP_MENU_ADD_ITEM(STR_MENU_LUA);
        }
      }
#endif
      if (i_min <= MIXSRC_FIRST_STICK && i_max >= MIXSRC_FIRST_STICK)      POPUP_MENU_ADD_ITEM(STR_MENU_STICKS);
      if (i_min <= MIXSRC_FIRST_POT && i_max >= MIXSRC_FIRST_POT)          POPUP_MENU_ADD_ITEM(STR_MENU_POTS);
      if (i_min <= MIXSRC_MAX && i_max >= MIXSRC_MAX)                      POPUP_MENU_ADD_ITEM(STR_MENU_MAX);
#if defined(HELI)
      if (i_min <= MIXSRC_FIRST_HELI && i_max >= MIXSRC_FIRST_HELI)        POPUP_MENU_ADD_ITEM(STR_MENU_HELI);
#endif
      if (i_min <= MIXSRC_FIRST_TRIM && i_max >= MIXSRC_FIRST_TRIM)        POPUP_MENU_ADD_ITEM(STR_MENU_TRIMS);
      if (i_min <= MIXSRC_FIRST_SWITCH && i_max >= MIXSRC_FIRST_SWITCH)    POPUP_MENU_ADD_ITEM(STR_MENU_SWITCHES);
      if (i_min <= MIXSRC_FIRST_TRAINER && i_max >= MIXSRC_FIRST_TRAINER)  POPUP_MENU_ADD_ITEM(STR_MENU_TRAINER);
      if (i_min <= MIXSRC_FIRST_CH && i_max >= MIXSRC_FIRST_CH)            POPUP_MENU_ADD_ITEM(STR_MENU_CHANNELS);
      if (i_min <= MIXSRC_FIRST_GVAR && i_max >= MIXSRC_FIRST_GVAR && isValueAvailable(MIXSRC_FIRST_GVAR)) {
        POPUP_MENU_ADD_ITEM(STR_MENU_GVARS);
      }

      if (i_min <= MIXSRC_FIRST_TELEM && i_max >= MIXSRC_FIRST_TELEM) {
        for (int i = 0; i < MAX_TELEMETRY_SENSORS; i++) {
          TelemetrySensor * sensor = & g_model.telemetrySensors[i];
          if (sensor->isAvailable()) {
            POPUP_MENU_ADD_ITEM(STR_MENU_TELEMETRY);
            break;
          }
        }
      }
      POPUP_MENU_START(onSourceLongEnterPress);
    }
    if (checkIncDecSelection != 0) {
      newval = checkIncDecSelection;
      if (checkIncDecSelection != MIXSRC_MAX)
        s_editMode = EDIT_MODIFY_FIELD;
      checkIncDecSelection = 0;
    }
  }
  else if (i_flags & INCDEC_SWITCH) {
    if (event == EVT_KEY_LONG(KEY_ENTER) && !IS_SHIFT_PRESSED()) {
      killEvents(event);
      checkIncDecSelection = SWSRC_NONE;
      if (i_min <= SWSRC_FIRST_SWITCH && i_max >= SWSRC_LAST_SWITCH)       POPUP_MENU_ADD_ITEM(STR_MENU_SWITCHES);
      if (i_min <= SWSRC_FIRST_TRIM && i_max >= SWSRC_LAST_TRIM)           POPUP_MENU_ADD_ITEM(STR_MENU_TRIMS);
      if (i_min <= SWSRC_FIRST_LOGICAL_SWITCH && i_max >= SWSRC_LAST_LOGICAL_SWITCH) {
        for (int i = 0; i < MAX_LOGICAL_SWITCHES; i++) {
          if (isValueAvailable && isValueAvailable(SWSRC_FIRST_LOGICAL_SWITCH+i)) {
            POPUP_MENU_ADD_ITEM(STR_MENU_LOGICAL_SWITCHES);
            break;
          }
        }
      }
      if (isValueAvailable && isValueAvailable(SWSRC_ON))                  POPUP_MENU_ADD_ITEM(STR_MENU_OTHER);
      if (isValueAvailable && isValueAvailable(-newval))                   POPUP_MENU_ADD_ITEM(STR_MENU_INVERT);
      POPUP_MENU_START(onSwitchLongEnterPress);
      s_editMode = EDIT_MODIFY_FIELD;
    }
    if (checkIncDecSelection != 0) {
      newval = (checkIncDecSelection == SWSRC_INVERT ? -newval : checkIncDecSelection);
      s_editMode = EDIT_MODIFY_FIELD;
      checkIncDecSelection = 0;
    }
  }
#endif
  return newval;
}
#endif
#else
int16_t checkIncDec(event_t event, int16_t val, int16_t i_min, int16_t i_max, uint8_t i_flags)
{
  int16_t newval = val;

#if defined(DBLKEYS)
  uint8_t in = KEYS_PRESSED();
  if (!(i_flags & NO_DBLKEYS) && (EVT_KEY_MASK(event))) {
    bool dblkey = true;
    if (DBLKEYS_PRESSED_RGT_LFT(in))
      newval = -val;
    else if (DBLKEYS_PRESSED_RGT_UP(in)) {
      newval = (i_max > 100 ? 100 : i_max);
    }
    else if (DBLKEYS_PRESSED_LFT_DWN(in)) {
      newval = (i_min < -100 ? -100 : i_min);
    }
    else if (DBLKEYS_PRESSED_UP_DWN(in)) {
      newval = 0;
    }
    else {
      dblkey = false;
    }

    if (dblkey) {
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      killEvents(KEY_RIGHT);
      killEvents(KEY_LEFT);
      event = 0;
    }
  }
#endif

  if (event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_RIGHT) || (s_editMode>0 && (IS_ROTARY_RIGHT(event) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_UP)))) {
    newval++;
  }
  else if (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_REPT(KEY_LEFT) || (s_editMode>0 && (IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_REPT(KEY_DOWN)))) {
    newval--;
  }

  if (!READ_ONLY() && i_min==0 && i_max==1 && (event==EVT_KEY_BREAK(KEY_ENTER) || IS_ROTARY_BREAK(event))) {
    s_editMode = 0;
    newval = !val;
  }

#if defined(NAVIGATION_POT1)
  // change values based on P1
  newval -= p1valdiff;
  p1valdiff = 0;
#endif

#if defined(AUTOSWITCH)
  if (i_flags & INCDEC_SWITCH) {
    newval = checkIncDecMovedSwitch(newval);
  }
#endif

#if defined(AUTOSOURCE)
  if (i_flags & INCDEC_SOURCE) {
    if (s_editMode>0) {
      int8_t source = GET_MOVED_SOURCE(i_min, i_max);
      if (source) {
        newval = source;
      }
#if defined(AUTOSWITCH)
      else {
        uint8_t swtch = abs(getMovedSwitch());
        if (swtch) {
          newval = switchToMix(swtch);
        }
      }
#endif
    }
  }
#endif

  if (newval > i_max || newval < i_min) {
    newval = (newval > i_max ? i_max : i_min);
    killEvents(event);
    AUDIO_KEY_ERROR();
  }

  if (newval != val) {
    if (!(i_flags & NO_INCDEC_MARKS) && (newval != i_max) && (newval != i_min) && (newval==0 || newval==-100 || newval==+100) && !IS_ROTARY_EVENT(event)) {
      pauseEvents(event); // delay before auto-repeat continues
    }
    if (!IS_KEY_REPT(event)) {
      AUDIO_KEY_PRESS();
    }
    storageDirty(i_flags & (EE_GENERAL|EE_MODEL));
    checkIncDec_Ret = (newval > val ? 1 : -1);
  }
  else {
    checkIncDec_Ret = 0;
  }
  return newval;
}
#endif

#if defined(CPUM64)
int8_t checkIncDecModel(event_t event, int8_t i_val, int8_t i_min, int8_t i_max)
{
  return checkIncDec(event, i_val, i_min, i_max, EE_MODEL);
}

int8_t checkIncDecModelZero(event_t event, int8_t i_val, int8_t i_max)
{
  return checkIncDecModel(event, i_val, 0, i_max);
}

int8_t checkIncDecGen(event_t event, int8_t i_val, int8_t i_min, int8_t i_max)
{
  return checkIncDec(event, i_val, i_min, i_max, EE_GENERAL);
}
#endif

#define SCROLL_TH      64
#define SCROLL_POT1_TH 32

#if defined(CPUARM)
  #define CURSOR_NOT_ALLOWED_IN_ROW(row)   ((int8_t)MAXCOL(row) < 0)
#else
  #define CURSOR_NOT_ALLOWED_IN_ROW(row)   (MAXCOL(row) == TITLE_ROW)
#endif

#define INC(val, min, max)             if (val<max) {val++;} else {val=min;}
#define DEC(val, min, max)             if (val>min) {val--;} else {val=max;}

#if defined(CPUARM)
tmr10ms_t menuEntryTime;
#endif

#if defined(PCBX7)
#define MAXCOL_RAW(row)                (horTab ? pgm_read_byte(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#define MAXCOL(row)                    (MAXCOL_RAW(row) >= HIDDEN_ROW ? MAXCOL_RAW(row) : (const uint8_t)(MAXCOL_RAW(row) & (~NAVIGATION_LINE_BY_LINE)))
#define COLATTR(row)                   (MAXCOL_RAW(row) == (uint8_t)-1 ? (const uint8_t)0 : (const uint8_t)(MAXCOL_RAW(row) & NAVIGATION_LINE_BY_LINE))
#define MENU_FIRST_LINE_EDIT           (menuTab ? (MAXCOL((uint16_t)0) >= HIDDEN_ROW ? (MAXCOL((uint16_t)1) >= HIDDEN_ROW ? 2 : 1) : 0) : 0)
#define POS_HORZ_INIT(posVert)         ((COLATTR(posVert) & NAVIGATION_LINE_BY_LINE) ? -1 : 0)

void check(event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, const pm_uint8_t * horTab, uint8_t horTabMax, vertpos_t rowcount)
{
  vertpos_t l_posVert = menuVerticalPosition;
  horzpos_t l_posHorz = menuHorizontalPosition;

  uint8_t maxcol = MAXCOL(l_posVert);

  if (menuTab) {
    int cc = curr;
    switch (event) {
#if 0 // TODO
      case EVT_KEY_LONG(KEY_MENU):
        if (menuTab == menuTabModel) {
          killEvents(event);
          if (modelHasNotes()) {
            POPUP_MENU_ADD_SD_ITEM(STR_VIEW_CHANNELS);
            POPUP_MENU_ADD_ITEM(STR_VIEW_NOTES);
            POPUP_MENU_START(onLongMenuPress);
          }
          else {
            pushMenu(menuChannelsView);
          }
        }
        break;
#endif

      case EVT_KEY_LONG(KEY_PAGE):
        if (curr > 0)
          cc = curr - 1;
        else
          cc = menuTabSize-1;
        killEvents(event);
        break;

      case EVT_KEY_BREAK(KEY_PAGE):
        if (curr < (menuTabSize-1))
          cc = curr + 1;
        else
          cc = 0;
        break;
    }

    if (!menuCalibrationState && cc != curr) {
      chainMenu((MenuHandlerFunc)pgm_read_adr(&menuTab[cc]));
    }

    // TODO if (!(flags&CHECK_FLAG_NO_SCREEN_INDEX)) {
      drawScreenIndex(curr, menuTabSize, 0);
    // }

    // TODO lcdDrawFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, SOLID, FILL_WHITE|GREY_DEFAULT);
  }

  DISPLAY_PROGRESS_BAR(menuTab ? lcdLastRightPos-2*FW-((curr+1)/10*FWNUM)-2 : 20*FW+1);

  switch (event) {
    case EVT_ENTRY:
      menuEntryTime = get_tmr10ms();
      s_editMode = EDIT_MODE_INIT;
      l_posVert = MENU_FIRST_LINE_EDIT;
      l_posHorz = POS_HORZ_INIT(l_posVert);
      break;

    case EVT_ENTRY_UP:
      menuEntryTime = get_tmr10ms();
      s_editMode = 0;
      l_posHorz = POS_HORZ_INIT(l_posVert);
      break;

    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
      if (menuHorizontalPosition < 0 && maxcol > 0 && READ_ONLY_UNLOCKED()) {
        l_posHorz = 0;
        AUDIO_KEY_PRESS();
      }
      else if (READ_ONLY_UNLOCKED()) {
        s_editMode = (s_editMode<=0);
        AUDIO_KEY_PRESS();
      }
      break;

    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0; // TODO needed? we call ENTRY_UP after which does the same
      popMenu();
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_editMode > 0) {
        s_editMode = 0;
        AUDIO_KEY_PRESS();
        break;
      }

      if (l_posHorz >= 0 && (COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE)) {
        l_posHorz = -1;
        AUDIO_KEY_PRESS();
      }
      else {
        uint8_t posVertInit = MENU_FIRST_LINE_EDIT;
        if (menuVerticalOffset != 0 || l_posVert != posVertInit) {
          menuVerticalOffset = 0;
          l_posVert = posVertInit;
          l_posHorz = POS_HORZ_INIT(l_posVert);
          AUDIO_KEY_PRESS();
        }
        else {
          popMenu();
        }
      }
      break;

    case EVT_ROTARY_RIGHT:
    case EVT_KEY_FIRST(KEY_RIGHT):
      AUDIO_KEY_PRESS();
      // no break
    case EVT_KEY_REPT(KEY_RIGHT):
      if (s_editMode > 0) break; // TODO it was !=
      if ((COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE)) {
        if (l_posHorz >= 0) {
          INC(l_posHorz, 0, maxcol);
          break;
        }
      }
      else {
        if (l_posHorz < maxcol) {
          l_posHorz++;
          break;
        }
        else {
          l_posHorz = 0;
        }
      }

      do {
        INC(l_posVert, MENU_FIRST_LINE_EDIT, rowcount-1);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

      s_editMode = 0; // if we go down, we must be in this mode

      l_posHorz = POS_HORZ_INIT(l_posVert);
      break;

    case EVT_ROTARY_LEFT:
    case EVT_KEY_FIRST(KEY_LEFT):
      AUDIO_KEY_PRESS();
      // no break
    case EVT_KEY_REPT(KEY_LEFT):
      if (s_editMode > 0) break; // TODO it was !=
      if ((COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE)) {
        if (l_posHorz >= 0) {
          DEC(l_posHorz, 0, maxcol);
          break;
        }
      }
      else if (l_posHorz > 0) {
        l_posHorz--;
        break;
      }
      else {
        l_posHorz = 0xff;
      }

      do {
        DEC(l_posVert, MENU_FIRST_LINE_EDIT, rowcount-1);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

      s_editMode = 0; // if we go up, we must be in this mode

      if ((COLATTR(l_posVert) & NAVIGATION_LINE_BY_LINE))
        l_posHorz = -1;
      else
        l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));

      break;
  }

  int linesCount = rowcount;

  if (l_posVert == 0 || (l_posVert==1 && MAXCOL(vertpos_t(0)) >= HIDDEN_ROW) || (l_posVert==2 && MAXCOL(vertpos_t(0)) >= HIDDEN_ROW && MAXCOL(vertpos_t(1)) >= HIDDEN_ROW)) {
    menuVerticalOffset = 0;
    if (horTab) {
      linesCount = 0;
      for (int i=0; i<rowcount; i++) {
        if (i>horTabMax || horTab[i] != HIDDEN_ROW) {
          linesCount++;
        }
      }
    }
  }
  else if (horTab) {
    if (rowcount > NUM_BODY_LINES) {
      while (1) {
        vertpos_t firstLine = 0;
        for (int numLines=0; firstLine<rowcount && numLines<menuVerticalOffset; firstLine++) {
          if (firstLine>=horTabMax || horTab[firstLine] != HIDDEN_ROW) {
            numLines++;
          }
        }
        if (l_posVert < firstLine) {
          menuVerticalOffset--;
        }
        else {
          vertpos_t lastLine = firstLine;
          for (int numLines=0; lastLine<rowcount && numLines<NUM_BODY_LINES; lastLine++) {
            if (lastLine>=horTabMax || horTab[lastLine] != HIDDEN_ROW) {
              numLines++;
            }
          }
          if (l_posVert >= lastLine) {
            menuVerticalOffset++;
          }
          else {
            linesCount = menuVerticalOffset + NUM_BODY_LINES;
            for (int i=lastLine; i<rowcount; i++) {
              if (i>horTabMax || horTab[i] != HIDDEN_ROW) {
                linesCount++;
              }
            }
            break;
          }
        }
      }
    }
  }
  else {
    if (l_posVert>=NUM_BODY_LINES+menuVerticalOffset) {
      menuVerticalOffset = l_posVert-NUM_BODY_LINES+1;
    }
    else if (l_posVert<menuVerticalOffset) {
      menuVerticalOffset = l_posVert;
    }
  }

  // TODO if (name) {
  //  title(name);
  // }

  menuVerticalPosition = l_posVert;
  menuHorizontalPosition = l_posHorz;
}
#else
#if defined(PCBXLITE)
#define MAXCOL_RAW(row)                (horTab ? pgm_read_byte(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#define MAXCOL(row)                    (MAXCOL_RAW(row) >= HIDDEN_ROW ? MAXCOL_RAW(row) : (const uint8_t)(MAXCOL_RAW(row) & (~NAVIGATION_LINE_BY_LINE)))
#else
#define MAXCOL(row)                    (horTab ? pgm_read_byte(horTab+min(row, (vertpos_t)horTabMax)) : (const uint8_t)0)
#endif
#define POS_HORZ_INIT(posVert)         0

void check(event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow)
{
  vertpos_t l_posVert = menuVerticalPosition;
  horzpos_t l_posHorz = menuHorizontalPosition;

  uint8_t maxcol = MAXCOL(l_posVert);

#if defined(NAVIGATION_POT1)
  // check pot 1 - if changed -> scroll values
  static int16_t p1val;
  static int16_t p1valprev;
  p1valdiff = (p1val-calibratedAnalogs[CALIBRATED_POT1]) / SCROLL_POT1_TH;
  if (p1valdiff) {
    p1valdiff = (p1valprev-calibratedAnalogs[CALIBRATED_POT1]) / 2;
    p1val = calibratedAnalogs[CALIBRATED_POT1];
  }
  p1valprev = calibratedAnalogs[CALIBRATED_POT1];
#endif

#if defined(NAVIGATION_POT2)
  // check pot 2 - if changed -> scroll menu
  static int16_t p2valprev;
  p2valdiff = (p2valprev-calibratedAnalogs[CALIBRATED_POT2]) / SCROLL_TH;
  if (p2valdiff) p2valprev = calibratedAnalogs[CALIBRATED_POT2];
#endif

#if defined(NAVIGATION_POT3)
  // check pot 3 if changed -> cursor down/up
  static int16_t p3valprev;
  int8_t scrollUD = (p3valprev-calibratedAnalogs[CALIBRATED_POT3]) / SCROLL_TH;
  if (scrollUD) p3valprev = calibratedAnalogs[CALIBRATED_POT3];
#else
  #define scrollUD 0
#endif

  if (p2valdiff || scrollUD || p1valdiff) backlightOn(); // on keypress turn the light on

  if (menuTab) {
    uint8_t attr = 0;

    if (l_posVert==0 && !menuCalibrationState) {
      attr = INVERS;

      int8_t cc = curr;

      if (p2valdiff) {
        cc = limit((int8_t)0, (int8_t)(cc - p2valdiff), (int8_t)(menuTabSize-1));
      }

      switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_BREAK:
          if (s_editMode < 0 && maxrow > 0) {
            s_editMode = 0;
            // TODO ? l_posVert = (horTab && horTab[1]==0xff) ? 2 : 1;
            l_posHorz = 0;
          }
          else {
            s_editMode = -1;
          }
          event = 0;
          break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_LEFT:
          if (s_editMode >= 0)
            break;
#endif
        case EVT_KEY_FIRST(KEY_LEFT):
          if (curr > 0)
            cc = curr - 1;
          else
            cc = menuTabSize-1;
          break;

#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_RIGHT:
          if (s_editMode >= 0)
            break;
#endif
        case EVT_KEY_FIRST(KEY_RIGHT):
          if (curr < (menuTabSize-1))
            cc = curr + 1;
          else
            cc = 0;
          break;
      }

      if (cc != curr) {
        chainMenu((MenuHandlerFunc)pgm_read_adr(&menuTab[cc]));
      }

#if defined(ROTARY_ENCODER_NAVIGATION)
      if (IS_ROTARY_ENCODER_NAVIGATION_ENABLE() && s_editMode < 0)
        attr = INVERS|BLINK;
#endif
    }

    menuCalibrationState = 0;
    drawScreenIndex(curr, menuTabSize, attr);

  }

  DISPLAY_PROGRESS_BAR(menuTab ? lcdLastRightPos-2*FW-((curr+1)/10*FWNUM)-2 : 20*FW+1);

  if (s_editMode<=0) {
    if (scrollUD) {
      l_posVert = limit((int8_t)0, (int8_t)(l_posVert - scrollUD), (int8_t)maxrow);
      l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
    }

    if (p2valdiff && l_posVert>0) {
      l_posHorz = limit((int8_t)0, (int8_t)((uint8_t)l_posHorz - p2valdiff), (int8_t)maxcol);
    }
  }

  switch (event)
  {
    case EVT_ENTRY:
#if defined(CPUARM)
      menuEntryTime = get_tmr10ms();
#endif
      l_posVert = 0;
      l_posHorz = POS_HORZ_INIT(l_posVert);
#if defined(ROTARY_ENCODER_NAVIGATION)
      if (menuTab) {
        s_editMode = EDIT_MODE_INIT;
        break;
      }
      // no break
#else
      s_editMode = EDIT_MODE_INIT;
      break;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ENTRY_UP:
      s_editMode = 0;
      break;

    case EVT_ROTARY_BREAK:
      if (s_editMode > 1) break;
#endif

    case EVT_KEY_FIRST(KEY_ENTER):
      if (!menuTab || l_posVert>0) {
        if (READ_ONLY_UNLOCKED()) {
          s_editMode = (s_editMode<=0);
        }
      }
      break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_LONG:
      if (s_editMode > 1) break;
      killEvents(event);
      if (l_posVert != 0) {
        l_posVert = 0;
        s_editMode = EDIT_MODE_INIT;
        break;
      }
      // no break
#endif
    case EVT_KEY_LONG(KEY_EXIT):
      s_editMode = 0; // TODO needed? we call ENTRY_UP after which does the same
      popMenu();
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      AUDIO_KEY_PRESS();
#if defined(ROTARY_ENCODER_NAVIGATION)
      if (s_editMode == 0)
        s_editMode = EDIT_MODE_INIT;
      else
#endif
      if (s_editMode>0) {
        s_editMode = 0;
        break;
      }

      if (l_posVert==0 || !menuTab) {
        popMenu();  // beeps itself
      }
      else {
        l_posVert = 0;
        l_posHorz = 0;
      }
      break;

    case EVT_KEY_REPT(KEY_RIGHT):  //inc
      if (l_posHorz==maxcol) break;
      // no break

    case EVT_KEY_FIRST(KEY_RIGHT)://inc
      if (!horTab || s_editMode>0) break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_RIGHT
      if (s_editMode != 0) break;
      if (l_posHorz < maxcol) {
        l_posHorz++;
        break;
      }
      else {
        l_posHorz = 0;
        if (!IS_ROTARY_RIGHT(event))
          break;
      }
#else
      INC(l_posHorz, 0, maxcol);
      break;
#endif

#if !defined(PCBX7)
    case EVT_KEY_REPT(KEY_DOWN):
      if (!IS_ROTARY_RIGHT(event) && l_posVert==maxrow) break;
      // no break

    case EVT_KEY_FIRST(KEY_DOWN):
      if (s_editMode>0) break;
      do {
        INC(l_posVert, 0, maxrow);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

#if defined(ROTARY_ENCODER_NAVIGATION)
      s_editMode = 0; // if we go down, we must be in this mode
#endif

      l_posHorz = min<horzpos_t>(l_posHorz, MAXCOL(l_posVert));
      break;
#endif

    case EVT_KEY_REPT(KEY_LEFT):  //dec
      if (l_posHorz==0) break;
      // no break

    case EVT_KEY_FIRST(KEY_LEFT)://dec
      if (!horTab || s_editMode>0) break;

#if defined(ROTARY_ENCODER_NAVIGATION)
    CASE_EVT_ROTARY_LEFT
      if (s_editMode != 0) break;
      if (l_posHorz > 0) {
        l_posHorz--;
        break;
      }
      else if (IS_ROTARY_LEFT(event) && s_editMode == 0) {
        l_posHorz = 0xff;
      }
      else {
        l_posHorz = maxcol;
        break;
      }
#else
      DEC(l_posHorz, 0, maxcol);
      break;
#endif

#if !defined(PCBX7)
    case EVT_KEY_REPT(KEY_UP):
      if (!IS_ROTARY_LEFT(event) && l_posVert==0) break;
      // no break
    case EVT_KEY_FIRST(KEY_UP):
      if (s_editMode>0) break;

      do {
        DEC(l_posVert, 0, maxrow);
      } while (CURSOR_NOT_ALLOWED_IN_ROW(l_posVert));

#if defined(ROTARY_ENCODER_NAVIGATION)
      s_editMode = 0; // if we go up, we must be in this mode
#endif

      l_posHorz = min((uint8_t)l_posHorz, MAXCOL(l_posVert));
      break;
#endif
  }

  uint8_t maxLines = menuTab ? LCD_LINES-1 : LCD_LINES-2;

#if defined(CPUARM)
  int linesCount = maxrow;
  if (l_posVert == 0 || (l_posVert==1 && MAXCOL(vertpos_t(0)) >= HIDDEN_ROW) || (l_posVert==2 && MAXCOL(vertpos_t(0)) >= HIDDEN_ROW && MAXCOL(vertpos_t(1)) >= HIDDEN_ROW)) {
    menuVerticalOffset = 0;
    if (horTab) {
      linesCount = 0;
      for (int i=0; i<maxrow; i++) {
        if (i>=horTabMax || horTab[i] != HIDDEN_ROW) {
          linesCount++;
        }
      }
    }
  }
  else if (horTab) {
    if (maxrow > maxLines) {
      while (1) {
        vertpos_t firstLine = 0;
        for (int numLines=0; firstLine<maxrow && numLines<menuVerticalOffset; firstLine++) {
          if (firstLine>=horTabMax || horTab[firstLine+1] != HIDDEN_ROW) {
            numLines++;
          }
        }
        if (l_posVert <= firstLine) {
          menuVerticalOffset--;
        }
        else {
          vertpos_t lastLine = firstLine;
          for (int numLines=0; lastLine<maxrow && numLines<maxLines; lastLine++) {
            if (lastLine>=horTabMax || horTab[lastLine+1] != HIDDEN_ROW) {
              numLines++;
            }
          }
          if (l_posVert > lastLine) {
            menuVerticalOffset++;
          }
          else {
            linesCount = menuVerticalOffset + maxLines;
            for (int i=lastLine; i<maxrow; i++) {
              if (i>=horTabMax || horTab[i] != HIDDEN_ROW) {
                linesCount++;
              }
            }
            break;
          }
        }
      }
    }
  }
#else
  if (l_posVert<1) {
    menuVerticalOffset=0;
  }
#endif
  else {
    if (l_posVert>maxLines+menuVerticalOffset) {
      menuVerticalOffset = l_posVert-maxLines;
    }
    else if (l_posVert<=menuVerticalOffset) {
      menuVerticalOffset = l_posVert-1;
    }
  }

  menuVerticalPosition = l_posVert;
  menuHorizontalPosition = l_posHorz;
#if !defined(CPUM64)
  // cosmetics on 9x
  if (menuVerticalOffset > 0) {
    l_posVert--;
    if (l_posVert == menuVerticalOffset && CURSOR_NOT_ALLOWED_IN_ROW(l_posVert)) {
      menuVerticalOffset = l_posVert-1;
    }
  }
#endif
}
#endif

void check_simple(event_t event, uint8_t curr, const MenuHandlerFunc * menuTab, uint8_t menuTabSize, vertpos_t maxrow)
{
  check(event, curr, menuTab, menuTabSize, 0, 0, maxrow);
}

void check_submenu_simple(event_t event, uint8_t maxrow)
{
  check_simple(event, 0, 0, 0, maxrow);
}

void repeatLastCursorMove(event_t event)
{
  if (CURSOR_MOVED_LEFT(event) || CURSOR_MOVED_RIGHT(event)) {
    putEvent(event);
  }
  else {
    menuHorizontalPosition = 0;
  }
}
