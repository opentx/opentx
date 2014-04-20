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
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
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

#include "opentx.h"
#include "stamp-opentx.h"

#if !defined(SIMU)
extern "C" {
#endif
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
#if !defined(SIMU)
}
#endif

#define lua_registerint(L, n, i)       (lua_pushinteger(L, (i)), lua_setglobal(L, (n)))
#define lua_pushtablenil(L, k)         (lua_pushstring(L, (k)), lua_pushnil(L), lua_settable(L, -3))
#define lua_pushtableboolean(L, k, v)  (lua_pushstring(L, (k)), lua_pushboolean(L, (v)), lua_settable(L, -3))
#define lua_pushtablenumber(L, k, v)   (lua_pushstring(L, (k)), lua_pushinteger(L, (v)), lua_settable(L, -3))
#define lua_pushtablestring(L, k, v)   (lua_pushstring(L, (k)), lua_pushstring(L, (v)), lua_settable(L, -3))
#define lua_pushtablezstring(L, k, v)  { char _zz[sizeof(v)+1]; zchar2str(_zz, (v), sizeof(v)); lua_pushstring(L, (k)); lua_pushstring(L, _zz); lua_settable(L, -3); }
#define lua_registerlib(L, name, tab)  (luaL_newmetatable(L, name), luaL_setfuncs(L, tab, 0), lua_setglobal(L, name))

lua_State *L = NULL;
uint8_t luaState = 0;
ScriptInternalData scriptInternalData[MAX_SCRIPTS] = { { SCRIPT_NOFILE, 0 } };
ScriptInternalData standaloneScript = { SCRIPT_NOFILE, 0 };

#define PERMANENT_SCRIPTS_MAX_INSTRUCTIONS (1000/100)
#define MANUAL_SCRIPTS_MAX_INSTRUCTIONS    (10000/100)
#define SCRIPTS_MAX_HEAP                   50
#define SET_LUA_INSTRUCTIONS_COUNT(x)      (instructionsPercent=0, lua_sethook(L, hook, LUA_MASKCOUNT, x))

static int instructionsPercent = 0;
void hook(lua_State* L, lua_Debug *ar)
{
  instructionsPercent++;
  if (instructionsPercent > 100) {
    // From now on, as soon as a line is executed, error
    // keep erroring until you're script reaches the top
    lua_sethook(L, hook, LUA_MASKLINE, 0);
    luaL_error(L, "");
  }
}

static int luaGetVersion(lua_State *L)
{
  lua_pushnumber(L, VERS_NUM);
  return 1;
}

static int luaGetTime(lua_State *L)
{
  lua_pushunsigned(L, get_tmr10ms());
  return 1;
}

static int luaGetValue(lua_State *L)
{
  if (lua_isnumber(L, 1)) {
    int src = luaL_checkinteger(L, 1);
    lua_pushinteger(L, getValue(src));
    return 1;
  }
  else {
    const char *what = luaL_checkstring(L, 1);
    if (!strcmp(what, "altitude")) {
      lua_pushnumber(L, double(frskyData.hub.baroAltitude)/100);
      return 1;
    }
    else if (!strcmp(what, "latitude")) {
      if (frskyData.hub.gpsFix)
        lua_pushnumber(L, gpsToDouble(frskyData.hub.gpsLatitudeNS=='S', frskyData.hub.gpsLatitude_bp, frskyData.hub.gpsLatitude_ap));
      else
        lua_pushnil(L);
      return 1;
    }
    else if (!strcmp(what, "longitude")) {
      if (frskyData.hub.gpsFix)
        lua_pushnumber(L, gpsToDouble(frskyData.hub.gpsLongitudeEW=='W', frskyData.hub.gpsLongitude_bp, frskyData.hub.gpsLongitude_ap));
      else
        lua_pushnil(L);
      return 1;
    }
    else if (!strcmp(what, "pilot latitude")) {
      if (frskyData.hub.gpsFix)
        lua_pushnumber(L, pilotLatitude);
      else
        lua_pushnil(L);
      return 1;
    }
    else if (!strcmp(what, "pilot longitude")) {
      if (frskyData.hub.gpsFix)
        lua_pushnumber(L, pilotLongitude);
      else
        lua_pushnil(L);
      return 1;
    }
  }

  return 0;
}

static int luaPlayFile(lua_State *L)
{
  const char * filename = luaL_checkstring(L, 1);
  PLAY_FILE(filename, 0, 0);
  return 0;
}

static int luaLcdLock(lua_State *L)
{
  lcd_locked = true;
  return 0;
}

static int luaLcdClear(lua_State *L)
{
  lcd_clear();
  return 0;
}

static int luaLcdDrawPoint(lua_State *L)
{
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  lcd_plot(x, y);
  return 0;
}

static int luaLcdDrawLine(lua_State *L)
{
  int x1 = luaL_checkinteger(L, 1);
  int y1 = luaL_checkinteger(L, 2);
  int x2 = luaL_checkinteger(L, 3);
  int y2 = luaL_checkinteger(L, 4);
  lcd_line(x1, y1, x2, y2);
  return 0;
}

static int luaLcdDrawRectangle(lua_State *L)
{
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int w = luaL_checkinteger(L, 3);
  int h = luaL_checkinteger(L, 4);
  lcd_rect(x, y, w, h, 0xff, 0);
  return 0;
}

static int luaLcdDrawText(lua_State *L)
{
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  const char * s = luaL_checkstring(L, 3);
  int att = luaL_checkinteger(L, 4);
  lcd_putsAtt(x, y, s, att);
  return 0;
}

static int luaLcdDrawSwitch(lua_State *L)
{
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int s = luaL_checkinteger(L, 3);
  int att = luaL_checkinteger(L, 4);
  putsSwitches(x, y, s, att);
  return 0;
}

static int luaLcdDrawPixmap(lua_State *L)
{
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  const char * filename = luaL_checkstring(L, 3);
  // TODO int att = luaL_checkinteger(L, 4);
  bmp_ptr_t bitmap = 0;
  const pm_char * error = bmpLoad(bitmap, filename, LCD_W, LCD_H);
  if (bitmap && !error) {
    lcd_bmp(x, y, bitmap);
    // free(bitmap);
  }
  return 0;
}

static int luaLcdDrawScreenTitle(lua_State *L)
{
  const char * str = luaL_checkstring(L, 1);
  int idx = luaL_checkinteger(L, 2);
  int cnt = luaL_checkinteger(L, 3);

  displayScreenIndex(idx-1, cnt, 0);
  lcd_filled_rect(0, 0, LCD_W, FH, SOLID, FILL_WHITE|GREY_DEFAULT);
  title(str);

  return 0;
}

static int luaModelGetTimer(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < MAX_TIMERS) {
    TimerData & timer = g_model.timers[idx];
    lua_newtable(L);
    lua_pushtablenumber(L, "mode", timer.mode);
    lua_pushtablenumber(L, "start", timer.start);
    lua_pushtablenumber(L, "value", timersStates[idx].val);
    lua_pushtablenumber(L, "countdownBeep", timer.countdownBeep);
    lua_pushtableboolean(L, "minuteBeep", timer.minuteBeep);
    lua_pushtableboolean(L, "persistent", timer.persistent);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaModelSetTimer(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);

  if (idx < MAX_TIMERS) {
    TimerData & timer = g_model.timers[idx];
    luaL_checktype(L, -1, LUA_TTABLE);
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      luaL_checktype(L, -2, LUA_TSTRING); // key is string
      const char * key = luaL_checkstring(L, -2);
      if (!strcmp(key, "mode")) {
        timer.mode = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "start")) {
        timer.start = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "value")) {
        timersStates[idx].val = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "countdownBeep")) {
        timer.countdownBeep = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "minuteBeep")) {
        timer.minuteBeep = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "persistent")) {
        timer.persistent = luaL_checkinteger(L, -1);
      }
    }
    eeDirty(EE_MODEL);
  }
  return 0;
}

static int getFirstInput(int chn)
{
  for (int i=0; i<MAX_INPUTS; i++) {
    ExpoData * expo = expoAddress(i);
    if (!expo->srcRaw || expo->chn>chn) break;
    if (expo->chn == chn) {
      return i;
    }
  }
  return -1;
}

static int getInputsCountFromFirst(int chn, int first)
{
  int count = 0;
  if (first >= 0) {
    for (int i=first; i<MAX_INPUTS; i++) {
      ExpoData * expo = expoAddress(i);
      if (!expo->srcRaw || expo->chn!=chn) break;
      count++;
    }
  }
  return count;
}

static int getInputsCount(int chn)
{
  int first = getFirstInput(chn);
  return getInputsCountFromFirst(chn, first);
}

static int luaModelGetInputsCount(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int count = getInputsCount(chn);
  lua_pushinteger(L, count);
  return 1;
}

static int luaModelGetInput(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int idx = luaL_checkunsigned(L, 2);
  int first = getFirstInput(chn);
  int count = getInputsCountFromFirst(chn, first);
  if (first>=0 && idx<count) {
    ExpoData * expo = expoAddress(first+idx);
    lua_newtable(L);
    lua_pushtablezstring(L, "name", expo->name);
    lua_pushtablenumber(L, "source", expo->srcRaw);
    lua_pushtablenumber(L, "weight", expo->weight);
    lua_pushtablenumber(L, "offset", expo->offset);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaModelInsertInput(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int idx = luaL_checkunsigned(L, 2);

  int first = getFirstInput(chn);
  int count = getInputsCountFromFirst(chn, first);

  if (chn<MAX_INPUTS && getExpoMixCount(1)<MAX_INPUTS && idx<=count) {
    idx = first+idx;
    s_currCh = chn+1;
    insertExpoMix(1, idx);
    ExpoData * expo = expoAddress(idx);
    luaL_checktype(L, -1, LUA_TTABLE);
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      luaL_checktype(L, -2, LUA_TSTRING); // key is string
      const char * key = luaL_checkstring(L, -2);
      if (!strcmp(key, "name")) {
        const char * name = luaL_checkstring(L, -1);
        str2zchar(expo->name, name, sizeof(expo->name));
      }
      else if (!strcmp(key, "source")) {
        expo->srcRaw = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "weight")) {
        expo->weight = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "offset")) {
        expo->offset = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "switch")) {
        expo->swtch = luaL_checkinteger(L, -1);
      }
    }
  }

  return 0;
}

static int luaModelDeleteInput(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int idx = luaL_checkunsigned(L, 2);

  int first = getFirstInput(chn);
  int count = getInputsCountFromFirst(chn, first);

  if (first>=0 && idx<count) {
    deleteExpoMix(1, first+idx);
  }

  return 0;
}

static int getFirstMix(int chn)
{
  for (int i=0; i<MAX_MIXERS; i++) {
    MixData * mix = mixAddress(i);
    if (!mix->srcRaw || mix->destCh>chn) break;
    if (mix->destCh == chn) {
      return i;
    }
  }
  return -1;
}

static int getMixesCountFromFirst(int chn, int first)
{
  int count = 0;
  if (first >= 0) {
    for (int i=first; i<MAX_MIXERS; i++) {
      MixData * mix = mixAddress(i);
      if (!mix->srcRaw || mix->destCh!=chn) break;
      count++;
    }
  }
  return count;
}

static int getMixesCount(int chn)
{
  int first = getFirstMix(chn);
  return getMixesCountFromFirst(chn, first);
}

static int luaModelGetMixesCount(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int count = getMixesCount(chn);
  lua_pushinteger(L, count);
  return 1;
}

static int luaModelGetMix(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int idx = luaL_checkunsigned(L, 2);
  int first = getFirstMix(chn);
  int count = getMixesCountFromFirst(chn, first);
  if (first>=0 && idx<count) {
    MixData * mix = mixAddress(first+idx);
    lua_newtable(L);
    lua_pushtablezstring(L, "name", mix->name);
    lua_pushtablenumber(L, "source", mix->srcRaw);
    lua_pushtablenumber(L, "weight", mix->weight);
    lua_pushtablenumber(L, "offset", mix->offset);
    lua_pushtablenumber(L, "switch", mix->swtch);
    lua_pushtablenumber(L, "multiplex", mix->mltpx);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaModelInsertMix(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int idx = luaL_checkunsigned(L, 2);

  int first = getFirstMix(chn);
  int count = getMixesCountFromFirst(chn, first);

  if (chn<NUM_CHNOUT && getExpoMixCount(0)<MAX_MIXERS && idx<=count) {
    idx = first+idx;
    s_currCh = chn+1;
    insertExpoMix(0, idx);
    MixData *mix = mixAddress(idx);
    luaL_checktype(L, -1, LUA_TTABLE);
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      luaL_checktype(L, -2, LUA_TSTRING); // key is string
      const char * key = luaL_checkstring(L, -2);
      if (!strcmp(key, "name")) {
        const char * name = luaL_checkstring(L, -1);
        str2zchar(mix->name, name, sizeof(mix->name));
      }
      else if (!strcmp(key, "source")) {
        mix->srcRaw = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "weight")) {
        mix->weight = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "offset")) {
        mix->offset = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "switch")) {
        mix->swtch = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "multiplex")) {
        mix->mltpx = luaL_checkinteger(L, -1);
      }
    }
  }

  return 0;
}

static int luaModelDeleteMix(lua_State *L)
{
  int chn = luaL_checkunsigned(L, 1);
  int idx = luaL_checkunsigned(L, 2);

  int first = getFirstMix(chn);
  int count = getMixesCountFromFirst(chn, first);

  if (first>=0 && idx<count) {
    deleteExpoMix(0, first+idx);
  }

  return 0;
}

static int luaModelGetLogicalSwitch(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < NUM_LOGICAL_SWITCH) {
    LogicalSwitchData * sw = cswAddress(idx);
    lua_newtable(L);
    lua_pushtablenumber(L, "function", sw->func);
    lua_pushtablenumber(L, "v1", sw->v1);
    lua_pushtablenumber(L, "v2", sw->v2);
    lua_pushtablenumber(L, "v3", sw->v3);
    lua_pushtablenumber(L, "and", sw->andsw);
    lua_pushtablenumber(L, "delay", sw->delay);
    lua_pushtablenumber(L, "duration", sw->duration);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaModelSetLogicalSwitch(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < NUM_LOGICAL_SWITCH) {
    LogicalSwitchData * sw = cswAddress(idx);
    luaL_checktype(L, -1, LUA_TTABLE);
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      luaL_checktype(L, -2, LUA_TSTRING); // key is string
      const char * key = luaL_checkstring(L, -2);
      if (!strcmp(key, "function")) {
        sw->func = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "v1")) {
        sw->v1 = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "v2")) {
        sw->v2 = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "v3")) {
        sw->v3 = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "and")) {
        sw->andsw = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "delay")) {
        sw->delay = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "duration")) {
        sw->duration = luaL_checkinteger(L, -1);
      }
    }
  }

  return 0;
}

static int luaModelGetCustomFunction(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < NUM_CFN) {
    CustomFnData * cfn = &g_model.funcSw[idx];
    lua_newtable(L);
    lua_pushtablenumber(L, "switch", CFN_SWITCH(cfn));
    lua_pushtablenumber(L, "function", CFN_FUNC(cfn));
    lua_pushtablezstring(L, "name", cfn->play.name);
    lua_pushtablenumber(L, "value", cfn->all.val);
    lua_pushtablenumber(L, "mode", cfn->all.mode);
    lua_pushtablenumber(L, "active", CFN_ACTIVE(cfn));
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaModelSetCustomFunction(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < NUM_CFN) {
    CustomFnData * cfn = &g_model.funcSw[idx];
    luaL_checktype(L, -1, LUA_TTABLE);
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      luaL_checktype(L, -2, LUA_TSTRING); // key is string
      const char * key = luaL_checkstring(L, -2);
      if (!strcmp(key, "switch")) {
        CFN_SWITCH(cfn) = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "function")) {
        CFN_FUNC(cfn) = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "name")) {
        const char * name = luaL_checkstring(L, -1);
        str2zchar(cfn->play.name, name, sizeof(cfn->play.name));
      }
      else if (!strcmp(key, "value")) {
        cfn->all.val = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "mode")) {
        cfn->all.mode = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "active")) {
        CFN_ACTIVE(cfn) = luaL_checkinteger(L, -1);
      }
    }
  }

  return 0;
}

static int luaModelGetOutput(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < NUM_CHNOUT) {
    LimitData * limit = limitAddress(idx);
    lua_newtable(L);
    lua_pushtablezstring(L, "name", limit->name);
    lua_pushtablenumber(L, "min", limit->min);
    lua_pushtablenumber(L, "max", limit->min);
    lua_pushtablenumber(L, "offset", limit->offset);
    lua_pushtablenumber(L, "ppmCenter", limit->ppmCenter);
    lua_pushtableboolean(L, "symetrical", limit->symetrical);
    lua_pushtableboolean(L, "revert", limit->revert);
    if (limit->curve)
      lua_pushtablenumber(L, "curve", limit->curve-1);
    else
      lua_pushtablenil(L, "curve");
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaModelSetOutput(lua_State *L)
{
  int idx = luaL_checkunsigned(L, 1);
  if (idx < NUM_CHNOUT) {
    LimitData * limit = limitAddress(idx);
    luaL_checktype(L, -1, LUA_TTABLE);
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      luaL_checktype(L, -2, LUA_TSTRING); // key is string
      const char * key = luaL_checkstring(L, -2);
      if (!strcmp(key, "name")) {
        const char * name = luaL_checkstring(L, -1);
        str2zchar(limit->name, name, sizeof(limit->name));
      }
      else if (!strcmp(key, "min")) {
        limit->min = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "max")) {
        limit->max = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "offset")) {
        limit->offset = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "ppmCenter")) {
        limit->ppmCenter = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "symetrical")) {
        limit->symetrical = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "revert")) {
        limit->revert = luaL_checkinteger(L, -1);
      }
      else if (!strcmp(key, "curve")) {
        if (lua_isnil(L, -1))
          limit->curve = 0;
        else
          limit->curve = luaL_checkinteger(L, -1) + 1;
      }
    }
  }

  return 0;
}
static int luaPopupInput(lua_State *L)
{
  uint8_t event = luaL_checkinteger(L, 2);
  s_warning_input_value = luaL_checkinteger(L, 3);
  s_warning_input_min = luaL_checkinteger(L, 4);
  s_warning_input_max = luaL_checkinteger(L, 5);
  s_warning = luaL_checkstring(L, 1);
  s_warning_type = WARNING_TYPE_INPUT;
  displayWarning(event);
  if (s_warning_result) {
    s_warning_result = 0;
    lua_pushstring(L, "OK");
  }
  else if (!s_warning) {
    lua_pushstring(L, "CANCEL");
  }
  else {
    lua_pushinteger(L, s_warning_input_value);
  }
  s_warning = NULL;
  return 1;
}

int luaGetInputs(ScriptInternalData & sid)
{
  if (!lua_istable(L, -1))
    return -1;

  sid.inputsCount = 0;
  for (lua_pushnil(L); lua_next(L, -2) && sid.inputsCount<MAX_SCRIPT_OUTPUTS; lua_pop(L, 1)) {
    luaL_checktype(L, -2, LUA_TNUMBER); // key is number
    luaL_checktype(L, -1, LUA_TTABLE); // value is table
    uint8_t field = 0;
    for (lua_pushnil(L); lua_next(L, -2) && field<5; lua_pop(L, 1), field++) {
      switch (field) {
        case 0:
          luaL_checktype(L, -2, LUA_TNUMBER); // key is number
          luaL_checktype(L, -1, LUA_TSTRING); // value is string
          sid.inputs[sid.inputsCount].name = lua_tostring(L, -1);
          break;
        case 1:
          luaL_checktype(L, -2, LUA_TNUMBER); // key is number
          luaL_checktype(L, -1, LUA_TNUMBER); // value is number
          sid.inputs[sid.inputsCount].type = lua_tointeger(L, -1);
          if (sid.inputs[sid.inputsCount].type == 0) {
            sid.inputs[sid.inputsCount].min = -100;
            sid.inputs[sid.inputsCount].max = 100;
          }
          else {
            sid.inputs[sid.inputsCount].max = MIXSRC_LAST_TELEM;
          }
          break;
        case 2:
          luaL_checktype(L, -2, LUA_TNUMBER); // key is number
          luaL_checktype(L, -1, LUA_TNUMBER); // value is number
          sid.inputs[sid.inputsCount].min = lua_tointeger(L, -1);
          break;
        case 3:
          luaL_checktype(L, -2, LUA_TNUMBER); // key is number
          luaL_checktype(L, -1, LUA_TNUMBER); // value is number
          sid.inputs[sid.inputsCount].max = lua_tointeger(L, -1);
          break;
        case 4:
          luaL_checktype(L, -2, LUA_TNUMBER); // key is number
          luaL_checktype(L, -1, LUA_TNUMBER); // value is number
          sid.inputs[sid.inputsCount].def = lua_tointeger(L, -1);
          break;
      }
    }

    sid.inputsCount++;
  }

  return 0;
}

int luaGetOutputs(ScriptInternalData & sid)
{
  if (!lua_istable(L, -1))
    return -1;

  sid.outputsCount = 0;
  for (lua_pushnil(L); lua_next(L, -2) && sid.outputsCount<MAX_SCRIPT_OUTPUTS; lua_pop(L, 1)) {
    luaL_checktype(L, -2, LUA_TNUMBER); // key is number
    luaL_checktype(L, -1, LUA_TSTRING); // value is string
    sid.outputs[sid.outputsCount++].name = lua_tostring(L, -1);
  }

  return 0;
}

static const luaL_Reg modelLib[] = {
  { "getTimer", luaModelGetTimer },
  { "setTimer", luaModelSetTimer },
  { "getInputsCount", luaModelGetInputsCount },
  { "getInput", luaModelGetInput },
  { "insertInput", luaModelInsertInput },
  { "deleteInput", luaModelDeleteInput },
  { "getMixesCount", luaModelGetMixesCount },
  { "getMix", luaModelGetMix },
  { "insertMix", luaModelInsertMix },
  { "deleteMix", luaModelDeleteMix },
  { "getLogicalSwitch", luaModelGetLogicalSwitch },
  { "setLogicalSwitch", luaModelSetLogicalSwitch },
  { "getCustomFunction", luaModelGetCustomFunction },
  { "setCustomFunction", luaModelSetCustomFunction },
  { "getOutput", luaModelGetOutput },
  { "setOutput", luaModelSetOutput },
  { NULL, NULL }  /* sentinel */
};

static const luaL_Reg lcdLib[] = {
  { "lock", luaLcdLock },
  { "clear", luaLcdClear },
  { "drawPoint", luaLcdDrawPoint },
  { "drawLine", luaLcdDrawLine },
  { "drawRectangle", luaLcdDrawRectangle },
  { "drawText", luaLcdDrawText },
  { "drawSwitch", luaLcdDrawSwitch },
  { "drawPixmap", luaLcdDrawPixmap },
  { "drawScreenTitle", luaLcdDrawScreenTitle },
  { NULL, NULL }  /* sentinel */
};

void luaInit()
{
  if (L) {
    lua_close(L);
  }

  L = luaL_newstate();

  // Init lua
  luaL_openlibs(L);

  // Push OpenTX libs
  lua_registerlib(L, "model", modelLib);
  lua_registerlib(L, "lcd", lcdLib);

  // Push OpenTX functions
  lua_register(L, "getTime", luaGetTime);
  lua_register(L, "getVersion", luaGetVersion);
  lua_register(L, "getValue", luaGetValue);
  lua_register(L, "playFile", luaPlayFile);
  lua_register(L, "popupInput", luaPopupInput);

  // Push OpenTX constants
  lua_registerint(L, "DBLSIZE", DBLSIZE);
  lua_registerint(L, "MIDSIZE", MIDSIZE);
  lua_registerint(L, "SMLSIZE", SMLSIZE);
  lua_registerint(L, "INVERS", INVERS);
  lua_registerint(L, "BLINK", BLINK);
  lua_registerint(L, "VALUE", 0);
  lua_registerint(L, "SOURCE", 1);
  lua_registerint(L, "REPLACE", MLTPX_REP);
  lua_registerint(L, "STICK_RUDDER", MIXSRC_Rud);
  lua_registerint(L, "STICK_ELEVATOR", MIXSRC_Ele);
  lua_registerint(L, "STICK_THROTTLE", MIXSRC_Thr);
  lua_registerint(L, "STICK_AILERON", MIXSRC_Ail);
  lua_registerint(L, "SWITCH_LAST", SWSRC_LAST_CSW);
  lua_registerint(L, "EVT_MENU_BREAK", EVT_KEY_BREAK(KEY_MENU));
  lua_registerint(L, "EVT_PAGE_BREAK", EVT_KEY_BREAK(KEY_PAGE));
  lua_registerint(L, "EVT_ENTER_BREAK", EVT_KEY_BREAK(KEY_ENTER));
  lua_registerint(L, "EVT_EXIT_BREAK", EVT_KEY_BREAK(KEY_EXIT));
  lua_registerint(L, "EVT_PLUS_BREAK", EVT_KEY_BREAK(KEY_PLUS));
  lua_registerint(L, "EVT_MINUS_BREAK", EVT_KEY_BREAK(KEY_MINUS));
}

void luaFree(ScriptInternalData & sid)
{
  if (sid.run) {
    luaL_unref(L, LUA_REGISTRYINDEX, sid.run);
    sid.run = 0;
    lua_gc(L, LUA_GCCOLLECT, 0);
  }
}

int luaLoad(const char *filename, ScriptInternalData & sid)
{
  int init = 0;

  uint8_t prev_mem = lua_gc(L, LUA_GCCOUNT, 0);

  sid.instructions = 0;
  sid.state = SCRIPT_OK;

  luaFree(sid);

  SET_LUA_INSTRUCTIONS_COUNT(MANUAL_SCRIPTS_MAX_INSTRUCTIONS);

  if (luaL_loadfile(L, filename) == 0 &&
      lua_pcall(L, 0, 1, 0) == 0 &&
      lua_istable(L, -1)) {

    luaL_checktype(L, -1, LUA_TTABLE);

    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
      const char *key = lua_tostring(L, -2);
      if (!strcmp(key, "init")) {
        init = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_pushnil(L);
      }
      else if (!strcmp(key, "run")) {
        sid.run = luaL_ref(L, LUA_REGISTRYINDEX);
        lua_pushnil(L);
      }
      else if (!strcmp(key, "input")) {
        luaGetInputs(sid);
      }
      else if (!strcmp(key, "output")) {
        luaGetOutputs(sid);
      }
    }

    if (init) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, init);
      luaL_unref(L, LUA_REGISTRYINDEX, init);
      if (lua_pcall(L, 0, 0, 0) != 0) {
        TRACE("Error in script %s init: %s", filename, lua_tostring(L, -1));
        sid.state = SCRIPT_SYNTAX_ERROR;
      }
    }
  }
  else {
    TRACE("Error in script %s: %s", filename, lua_tostring(L, -1));
    sid.state = SCRIPT_SYNTAX_ERROR;
  }

  sid.memory = lua_gc(L, LUA_GCCOUNT, 0) - prev_mem;

  if (sid.state != SCRIPT_OK) {
    luaFree(sid);
  }

  return sid.state;
}

void luaLoadModelScript(uint8_t index)
{
  ScriptData & sd = g_model.scriptsData[index];
  ScriptInternalData & sid = scriptInternalData[index];

  memset(&sid, 0, sizeof(sid));
  sid.state = SCRIPT_NOFILE;

  if (ZEXIST(sd.file)) {
    char filename[sizeof(SCRIPTS_PATH)+sizeof(sd.file)+sizeof(SCRIPTS_EXT)] = SCRIPTS_PATH "/";
    strncpy(filename+sizeof(SCRIPTS_PATH), sd.file, sizeof(sd.file));
    filename[sizeof(SCRIPTS_PATH)+sizeof(sd.file)] = '\0';
    strcat(filename+sizeof(SCRIPTS_PATH), SCRIPTS_EXT);
    luaLoad(filename, sid);
  }
}

void luaLoadModelScripts()
{
  // Load model scripts
  for (int i=0; i<MAX_SCRIPTS; i++) {
    luaLoadModelScript(i);
  }
}

char lua_warning_str[WARNING_LINE_LEN+1];

void luaError(uint8_t error)
{
  const char *msg = lua_tostring(L, -1);
  if (msg) {
#if defined(SIMU)
    if (!strncmp(msg, "./", 2)) msg += 2;
#else
    if (!strncmp(msg, "/SCRIPTS/", 9)) msg += 9;
#endif
    strncpy(lua_warning_str, msg, WARNING_LINE_LEN);
    lua_warning_str[WARNING_LINE_LEN] = '\0';
    msg = lua_warning_str;
  }
  else {
    switch (error) {
      case SCRIPT_SYNTAX_ERROR:
        msg = "Script syntax error";
        break;
      case SCRIPT_KILLED:
        msg = "Script killed";
        break;
      case SCRIPT_LEAK:
        msg = "Script memory leak";
        break;
    }
  }
  POPUP_WARNING(msg);
}

void luaExec(const char *filename)
{
  LUA_RESET();
  standaloneScript.state = SCRIPT_NOFILE;
  int result = luaLoad(filename, standaloneScript);
  // TODO the same with run ...
  if (result == SCRIPT_OK) {
    luaState = LUASTATE_STANDALONE_SCRIPT_RUNNING;
  }
  else {
    luaError(result);
  }
}

void luaTask(uint8_t evt)
{
  lcd_locked = false;

  if (luaState & LUASTATE_STANDALONE_SCRIPT_RUNNING) {
    // standalone script
    if (standaloneScript.state == SCRIPT_OK && standaloneScript.run) {
      SET_LUA_INSTRUCTIONS_COUNT(MANUAL_SCRIPTS_MAX_INSTRUCTIONS);
      lua_rawgeti(L, LUA_REGISTRYINDEX, standaloneScript.run);
      lua_pushinteger(L, evt);
      if (lua_pcall(L, 1, 1, 0) == 0) {
        if (!lua_isnumber(L, -1)) {
          if (instructionsPercent>100) {
            TRACE("Script killed");
            standaloneScript.state = SCRIPT_KILLED;
          }
          else {
            TRACE("Script error");
            standaloneScript.state = SCRIPT_SYNTAX_ERROR;
          }
          luaState = LUASTATE_RELOAD_MODEL_SCRIPTS;
        }
        else {
          int scriptResult = lua_tointeger(L, -1);
          lua_pop(L, 1);  /* pop returned value */
          if (scriptResult == 0) {
            if (lua_gc(L, LUA_GCCOUNT, 0) > SCRIPTS_MAX_HEAP) {
              TRACE("Script memory leak");
              standaloneScript.state = SCRIPT_LEAK;
              luaState = LUASTATE_RELOAD_MODEL_SCRIPTS;
            }
          }
          else {
            TRACE("Script finished with status %d", scriptResult);
            standaloneScript.state = SCRIPT_NOFILE;
            luaState = LUASTATE_RELOAD_MODEL_SCRIPTS;
          }
        }
      }
      else {
        TRACE("Script error: %s", lua_tostring(L, -1));
        standaloneScript.state = (instructionsPercent > 100 ? SCRIPT_KILLED : SCRIPT_SYNTAX_ERROR);
        luaState = LUASTATE_RELOAD_MODEL_SCRIPTS;
      }

      if (standaloneScript.state != SCRIPT_OK) {
        luaError(standaloneScript.state);
      }

    }
  }
  else {
    // model scripts
    if (luaState & LUASTATE_RELOAD_MODEL_SCRIPTS) {
      luaState = 0;
      LUA_RESET();
      luaLoadModelScripts();
    }

    for (int i=0; i<MAX_SCRIPTS; i++) {
      ScriptData & sd = g_model.scriptsData[i];
      ScriptInternalData & sid = scriptInternalData[i];
      if (sid.state == SCRIPT_OK) {
        uint8_t prev_mem = lua_gc(L, LUA_GCCOUNT, 0);
        SET_LUA_INSTRUCTIONS_COUNT(PERMANENT_SCRIPTS_MAX_INSTRUCTIONS);
        lua_rawgeti(L, LUA_REGISTRYINDEX, sid.run);
        for (int j=0; j<sid.inputsCount; j++) {
          if (sid.inputs[j].type == 1)
            lua_pushinteger(L, (uint8_t)sd.inputs[j]);
          else
            lua_pushinteger(L, sd.inputs[j]);
        }
        if (lua_pcall(L, sid.inputsCount, sid.outputsCount, 0) == 0) {
          for (int j=0; j<sid.outputsCount; j++) {
            if (!lua_isnumber(L, -1)) {
              sid.state = (instructionsPercent > 100 ? SCRIPT_KILLED : SCRIPT_SYNTAX_ERROR);
              TRACE("Script %10s disabled", sd.file);
              break;
            }
            sid.outputs[j].value = lua_tointeger(L, -1);
            lua_pop(L, 1);
          }
        }
        else {
          if (instructionsPercent > 100) {
            TRACE("Script %10s killed", sd.file);
            sid.state = SCRIPT_KILLED;
          }
          else {
            TRACE("Script %10s error: %s", sd.file, lua_tostring(L, -1));
            sid.state = SCRIPT_SYNTAX_ERROR;
          }
        }

        if (sid.state != SCRIPT_OK) {
          luaFree(sid);
        }
        else {
          sid.memory += lua_gc(L, LUA_GCCOUNT, 0) - prev_mem;
          if (instructionsPercent > sid.instructions) {
            sid.instructions = instructionsPercent;
          }
        }
      }
    }

    // TRACE("gc=%d", lua_gc(L, LUA_GCCOUNT, 0));

    if (lua_gc(L, LUA_GCCOUNT, 0) > SCRIPTS_MAX_HEAP) {
      uint8_t max_memory = 0;
      int8_t max_idx = -1;
      for (int i=0; i<MAX_SCRIPTS; i++) {
        ScriptInternalData & sid = scriptInternalData[i];
        if (sid.state == SCRIPT_OK && sid.memory > max_memory && sid.memory > 15) {
          max_idx = i;
        }
      }
      if (max_idx >= 0) {
        ScriptInternalData & sid = scriptInternalData[max_idx];
        TRACE("Script %d killed", max_idx);
        sid.state = SCRIPT_LEAK;
        luaFree(sid);
      }
    }
  }
}
