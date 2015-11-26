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

#include <ctype.h>
#include <stdio.h>
#include "opentx.h"
#include "bin_allocator.h"
#include "lua/lua_api.h"
 
#if defined(LUA_COMPILER) && defined(SIMU)
  #include <lundump.h>
  #include <lstate.h>
#endif

#define PERMANENT_SCRIPTS_MAX_INSTRUCTIONS (10000/100)
#define MANUAL_SCRIPTS_MAX_INSTRUCTIONS    (20000/100)
#define SET_LUA_INSTRUCTIONS_COUNT(x)      (instructionsPercent=0, lua_sethook(L, hook, LUA_MASKCOUNT, x))
#define LUA_WARNING_INFO_LEN               64

lua_State *L = NULL;
uint8_t luaState = 0;
uint8_t luaScriptsCount = 0;
ScriptInternalData scriptInternalData[MAX_SCRIPTS] = { { SCRIPT_NOFILE, 0 } };
ScriptInputsOutputs scriptInputsOutputs[MAX_SCRIPTS] = { {0} };
ScriptInternalData standaloneScript = { SCRIPT_NOFILE, 0 };
uint16_t maxLuaInterval = 0;
uint16_t maxLuaDuration = 0;
bool luaLcdAllowed;
int instructionsPercent = 0;
char lua_warning_info[LUA_WARNING_INFO_LEN+1];
struct our_longjmp * global_lj = 0;

/* custom panic handler */
static int custom_lua_atpanic(lua_State *lua)
{
  TRACE("PANIC: unprotected error in call to Lua API (%s)\n", lua_tostring(L, -1));
  if (global_lj) {
    longjmp(global_lj->b, 1);
    /* will never return */
  }
  return 0;
}

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

int luaGetInputs(ScriptInputsOutputs & sid)
{
  if (!lua_istable(L, -1))
    return -1;

  sid.inputsCount = 0;
  for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
    luaL_checktype(L, -2, LUA_TNUMBER); // key is number
    luaL_checktype(L, -1, LUA_TTABLE); // value is table
    if (sid.inputsCount<MAX_SCRIPT_INPUTS) {
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
  }

  return 0;
}

int luaGetOutputs(ScriptInputsOutputs & sid)
{
  if (!lua_istable(L, -1))
    return -1;

  sid.outputsCount = 0;
  for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
    luaL_checktype(L, -2, LUA_TNUMBER); // key is number
    luaL_checktype(L, -1, LUA_TSTRING); // value is string
    if (sid.outputsCount<MAX_SCRIPT_OUTPUTS) {
      sid.outputs[sid.outputsCount++].name = lua_tostring(L, -1);
    }
  }

  return 0;
}

void luaDisable()
{
  POPUP_WARNING("Lua disabled!");
  luaState = INTERPRETER_PANIC;
}

void luaClose()
{
  if (L) {
    PROTECT_LUA() {
      lua_close(L);  // this should not panic, but we make sure anyway
    }
    else {
      // we can only disable Lua for the rest of the session
      luaDisable();
    }
    UNPROTECT_LUA();
    L = NULL;
  }
}

void luaRegisterAll()
{
  // Init lua
  luaL_openlibs(L);
}

void luaInit()
{
  luaClose();
  if (luaState != INTERPRETER_PANIC) {
#if defined(USE_BIN_ALLOCATOR)
    L = lua_newstate(bin_l_alloc, NULL);   //we use our own allocator!
#else
    L = lua_newstate(l_alloc, NULL);   //we use Lua default allocator
#endif
    if (L) {
      // install our panic handler
      lua_atpanic(L, &custom_lua_atpanic);

      // protect libs and constants registration
      PROTECT_LUA() {
        luaRegisterAll();
      }
      else {
        // if we got panic during registration
        // we disable Lua for this session
        luaDisable();
      }
      UNPROTECT_LUA();
    }
    else {
      /* log error and return */
      luaDisable();
    }
  }
}

void luaFree(ScriptInternalData & sid)
{
  PROTECT_LUA() {
    if (sid.run) {
      luaL_unref(L, LUA_REGISTRYINDEX, sid.run);
      sid.run = 0;
    }
    if (sid.background) {
      luaL_unref(L, LUA_REGISTRYINDEX, sid.background);
      sid.background = 0;
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
  }
  else {
    luaDisable();
  }
  UNPROTECT_LUA();
}

#if defined(LUA_COMPILER) && defined(SIMU)
static int luaDumpWriter(lua_State* L, const void* p, size_t size, void* u)
{
  UNUSED(L);
  UINT written;
  FRESULT result = f_write((FIL *)u, p, size, &written);
  return (result != FR_OK && !written);
}

static void luaCompileAndSave(const char *bytecodeName)
{
  FIL D;
  char srcName[1024];
  strcpy(srcName, bytecodeName);
  strcat(srcName, ".src");

  if (f_stat(srcName, 0) != FR_OK) {
    return;   // no source to compile
  }

  if (f_open(&D, bytecodeName, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
    TRACE("Could not open Lua bytecode output file %s", bytecodeName);
    return;
  }

  PROTECT_LUA() {
    if (luaL_loadfile(L, srcName) == 0) {
      lua_lock(L);
      luaU_dump(L, getproto(L->top - 1), luaDumpWriter, &D, 1);
      lua_unlock(L);
      TRACE("Saved Lua bytecode to file %s", bytecodeName);
    }
  }
  UNPROTECT_LUA();
  f_close(&D);
}
#endif

int luaLoad(const char *filename, ScriptInternalData & sid, ScriptInputsOutputs * sio=NULL)
{
  int init = 0;

  sid.instructions = 0;
  sid.state = SCRIPT_OK;

#if 0
  // not needed, we just called luaInit
  luaFree(sid);
#endif

  if (luaState == INTERPRETER_PANIC) {
    return SCRIPT_PANIC;
  }

#if defined(LUA_COMPILER) && defined(SIMU)
  luaCompileAndSave(filename);
#endif

  SET_LUA_INSTRUCTIONS_COUNT(MANUAL_SCRIPTS_MAX_INSTRUCTIONS);

  PROTECT_LUA() {
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
        else if (!strcmp(key, "background")) {
          sid.background = luaL_ref(L, LUA_REGISTRYINDEX);
          lua_pushnil(L);
        }
        else if (sio && !strcmp(key, "input")) {
          luaGetInputs(*sio);
        }
        else if (sio && !strcmp(key, "output")) {
          luaGetOutputs(*sio);
        }
      }

      if (init) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, init);
        if (lua_pcall(L, 0, 0, 0) != 0) {
          TRACE("Error in script %s init: %s", filename, lua_tostring(L, -1));
          sid.state = SCRIPT_SYNTAX_ERROR;
        }
        luaL_unref(L, LUA_REGISTRYINDEX, init);
        lua_gc(L, LUA_GCCOLLECT, 0);
      }
    }
    else {
      TRACE("Error in script %s: %s", filename, lua_tostring(L, -1));
      sid.state = SCRIPT_SYNTAX_ERROR;
    }
  }
  else {
    luaDisable();
    return SCRIPT_PANIC;
  }
  UNPROTECT_LUA();

  if (sid.state != SCRIPT_OK) {
    luaFree(sid);
  }

  return sid.state;
}

bool luaLoadMixScript(uint8_t index)
{
  ScriptData & sd = g_model.scriptsData[index];

  if (ZEXIST(sd.file)) {
    ScriptInternalData & sid = scriptInternalData[luaScriptsCount++];
    ScriptInputsOutputs * sio = &scriptInputsOutputs[index];
    sid.reference = SCRIPT_MIX_FIRST+index;
    sid.state = SCRIPT_NOFILE;
    char filename[sizeof(SCRIPTS_MIXES_PATH)+sizeof(sd.file)+sizeof(SCRIPTS_EXT)] = SCRIPTS_MIXES_PATH "/";
    strncpy(filename+sizeof(SCRIPTS_MIXES_PATH), sd.file, sizeof(sd.file));
    filename[sizeof(SCRIPTS_MIXES_PATH)+sizeof(sd.file)] = '\0';
    strcat(filename+sizeof(SCRIPTS_MIXES_PATH), SCRIPTS_EXT);
    if (luaLoad(filename, sid, sio) == SCRIPT_PANIC) {
      return false;
    }
  }
  return true;
}

bool luaLoadFunctionScript(uint8_t index)
{
  CustomFunctionData & fn = g_model.customFn[index];

  if (fn.func == FUNC_PLAY_SCRIPT && ZEXIST(fn.play.name)) {
    if (luaScriptsCount < MAX_SCRIPTS) {
      ScriptInternalData & sid = scriptInternalData[luaScriptsCount++];
      sid.reference = SCRIPT_FUNC_FIRST+index;
      sid.state = SCRIPT_NOFILE;
      char filename[sizeof(SCRIPTS_FUNCS_PATH)+sizeof(fn.play.name)+sizeof(SCRIPTS_EXT)] = SCRIPTS_FUNCS_PATH "/";
      strncpy(filename+sizeof(SCRIPTS_FUNCS_PATH), fn.play.name, sizeof(fn.play.name));
      filename[sizeof(SCRIPTS_FUNCS_PATH)+sizeof(fn.play.name)] = '\0';
      strcat(filename+sizeof(SCRIPTS_FUNCS_PATH), SCRIPTS_EXT);
      if (luaLoad(filename, sid) == SCRIPT_PANIC) {
        return false;
      }
    }
    else {
      POPUP_WARNING(STR_TOO_MANY_LUA_SCRIPTS);
      return false;
    }
  }
  return true;
}

bool luaLoadTelemetryScript(uint8_t index)
{
  TelemetryScreenType screenType = TELEMETRY_SCREEN_TYPE(index);

  if (screenType == TELEMETRY_SCREEN_TYPE_SCRIPT) {
    TelemetryScriptData & script = g_model.frsky.screens[index].script;
    if (ZEXIST(script.file)) {
      if (luaScriptsCount < MAX_SCRIPTS) {
        ScriptInternalData & sid = scriptInternalData[luaScriptsCount++];
        sid.reference = SCRIPT_TELEMETRY_FIRST+index;
        sid.state = SCRIPT_NOFILE;
        char filename[sizeof(SCRIPTS_TELEM_PATH)+sizeof(script.file)+sizeof(SCRIPTS_EXT)] = SCRIPTS_TELEM_PATH "/";
        strncpy(filename+sizeof(SCRIPTS_TELEM_PATH), script.file, sizeof(script.file));
        filename[sizeof(SCRIPTS_TELEM_PATH)+sizeof(script.file)] = '\0';
        strcat(filename+sizeof(SCRIPTS_TELEM_PATH), SCRIPTS_EXT);
        if (luaLoad(filename, sid) == SCRIPT_PANIC) {
          return false;
        }
      }
      else {
        POPUP_WARNING(STR_TOO_MANY_LUA_SCRIPTS);
        return false;
      }
    }
  }
  return true;
}

uint8_t isTelemetryScriptAvailable(uint8_t index)
{
  for (int i=0; i<luaScriptsCount; i++) {
    ScriptInternalData & sid = scriptInternalData[i];
    if (sid.reference == SCRIPT_TELEMETRY_FIRST+index) {
      return sid.state;
    }
  }
  return SCRIPT_NOFILE;
}

void luaLoadPermanentScripts()
{
  luaScriptsCount = 0;
  memset(scriptInternalData, 0, sizeof(scriptInternalData));
  memset(scriptInputsOutputs, 0, sizeof(scriptInputsOutputs));

  // Load model scripts
  for (int i=0; i<MAX_SCRIPTS; i++) {
    if (!luaLoadMixScript(i)) {
      return;
    }
  }

  // Load custom function scripts
  for (int i=0; i<NUM_CFN; i++) {
    if (!luaLoadFunctionScript(i)) {
      return;
    }
  }

  // Load custom telemetry scripts
  for (int i=0; i<MAX_TELEMETRY_SCREENS; i++) {
    if (!luaLoadTelemetryScript(i)) {
      return;
    }
  }
}

void displayLuaError(const char * title)
{
#if !defined(COLORLCD)
  displayBox(title);
#endif
  if (lua_warning_info[0]) {
    char * split = strstr(lua_warning_info, ": ");
    if (split) {
      lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y+FH+3, lua_warning_info, split-lua_warning_info, SMLSIZE);
      lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y+2*FH+2, split+2, lua_warning_info+LUA_WARNING_INFO_LEN-split, SMLSIZE);
    }
    else {
      lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y+FH+3, lua_warning_info, 40, SMLSIZE);
    }
  }
}

void displayAcknowledgeLuaError(evt_t event)
{
  warningResult = false;
  displayLuaError(warningText);
  if (event == EVT_KEY_BREAK(KEY_EXIT)) {
    warningText = NULL;
  }
}

void luaError(uint8_t error, bool acknowledge)
{
  const char * errorTitle;

  switch (error) {
    case SCRIPT_SYNTAX_ERROR:
      errorTitle = STR_SCRIPT_SYNTAX_ERROR;
      break;
    case SCRIPT_KILLED:
      errorTitle = STR_SCRIPT_KILLED;
      break;
    case SCRIPT_PANIC:
      errorTitle = STR_SCRIPT_PANIC;
      break;
    default:
      errorTitle = STR_SCRIPT_ERROR;
      break;
  }

  const char * msg = lua_tostring(L, -1);
  if (msg) {
#if defined(SIMU)
    if (!strncmp(msg, ".", 2)) msg += 1;
#endif
    if (!strncmp(msg, "/SCRIPTS/", 9)) msg += 9;
    strncpy(lua_warning_info, msg, LUA_WARNING_INFO_LEN);
    lua_warning_info[LUA_WARNING_INFO_LEN] = '\0';
  }
  else {
    lua_warning_info[0] = '\0';
  }

  if (acknowledge) {
    warningText = errorTitle;
    popupFunc = displayAcknowledgeLuaError;
  }
  else {
    displayLuaError(errorTitle);
  }
}

void luaExec(const char *filename)
{
  luaInit();
  if (luaState != INTERPRETER_PANIC) {
    standaloneScript.state = SCRIPT_NOFILE;
    int result = luaLoad(filename, standaloneScript);
    // TODO the same with run ...
    if (result == SCRIPT_OK) {
      luaState = INTERPRETER_RUNNING_STANDALONE_SCRIPT;
    }
    else {
      luaError(result);
      luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
    }
  }
}

void luaDoOneRunStandalone(evt_t evt)
{
  static uint8_t luaDisplayStatistics = false;

  if (standaloneScript.state == SCRIPT_OK && standaloneScript.run) {
    SET_LUA_INSTRUCTIONS_COUNT(MANUAL_SCRIPTS_MAX_INSTRUCTIONS);
    lua_rawgeti(L, LUA_REGISTRYINDEX, standaloneScript.run);
    lua_pushinteger(L, evt);
    if (lua_pcall(L, 1, 1, 0) == 0) {
      if (!lua_isnumber(L, -1)) {
        if (instructionsPercent > 100) {
          TRACE("Script killed");
          standaloneScript.state = SCRIPT_KILLED;
          luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
        }
        else if (lua_isstring(L, -1)) {
          char nextScript[_MAX_LFN+1];
          strncpy(nextScript, lua_tostring(L, -1), _MAX_LFN);
          nextScript[_MAX_LFN] = '\0';
          luaExec(nextScript);
        }
        else {
          TRACE("Script run function returned unexpected value");
          standaloneScript.state = SCRIPT_SYNTAX_ERROR;
          luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
        }
      }
      else {
        int scriptResult = lua_tointeger(L, -1);
        lua_pop(L, 1);  /* pop returned value */
        if (scriptResult != 0) {
          TRACE("Script finished with status %d", scriptResult);
          standaloneScript.state = SCRIPT_NOFILE;
          luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
          return;
        }
        else if (luaDisplayStatistics) {
#if defined(COLORLCD)
#else
          lcdDrawSolidHorizontalLine(0, 7*FH-1, lcdLastPos+6, ERASE);
          lcdDrawText(0, 7*FH, "GV Use: ");
          lcdDrawNumber(lcdLastPos, 7*FH, luaGetMemUsed(), LEFT);
          lcdDrawChar(lcdLastPos, 7*FH, 'b');
          lcdDrawSolidHorizontalLine(0, 7*FH-2, lcdLastPos+6, FORCE);
          lcdDrawVerticalLine(lcdLastPos+6, 7*FH-2, FH+2, SOLID, FORCE);
#endif
        }
      }
    }
    else {
      TRACE("Script error: %s", lua_tostring(L, -1));
      standaloneScript.state = (instructionsPercent > 100 ? SCRIPT_KILLED : SCRIPT_SYNTAX_ERROR);
      luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
    }

    if (standaloneScript.state != SCRIPT_OK) {
      luaError(standaloneScript.state);
      luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
    }

    if (evt == EVT_KEY_LONG(KEY_EXIT)) {
      TRACE("Script force exit");
      killEvents(evt);
      standaloneScript.state = SCRIPT_NOFILE;
      luaState = INTERPRETER_RELOAD_PERMANENT_SCRIPTS;
    }
    else if (evt == EVT_KEY_LONG(KEY_MENU)) {
      killEvents(evt);
      luaDisplayStatistics = !luaDisplayStatistics;
    }
  }
}
  
bool luaDoOneRunPermanentScript(uint8_t evt, int i, uint32_t scriptType)
{
  ScriptInternalData & sid = scriptInternalData[i];
  if (sid.state != SCRIPT_OK) return false;

  SET_LUA_INSTRUCTIONS_COUNT(PERMANENT_SCRIPTS_MAX_INSTRUCTIONS);
  int inputsCount = 0;
#if defined(SIMU) || defined(DEBUG)
  const char *filename;
#endif
  ScriptInputsOutputs * sio = NULL;
#if SCRIPT_MIX_FIRST > 0
  if ((scriptType & RUN_MIX_SCRIPT) && (sid.reference >= SCRIPT_MIX_FIRST && sid.reference <= SCRIPT_MIX_LAST)) {
#else
  if ((scriptType & RUN_MIX_SCRIPT) && (sid.reference <= SCRIPT_MIX_LAST)) {
#endif
    ScriptData & sd = g_model.scriptsData[sid.reference-SCRIPT_MIX_FIRST];
    sio = &scriptInputsOutputs[sid.reference-SCRIPT_MIX_FIRST];
    inputsCount = sio->inputsCount;
#if defined(SIMU) || defined(DEBUG)
    filename = sd.file;
#endif
    lua_rawgeti(L, LUA_REGISTRYINDEX, sid.run);
    for (int j=0; j<sio->inputsCount; j++) {
      if (sio->inputs[j].type == 1)
        luaGetValueAndPush((uint8_t)sd.inputs[j]);
      else
        lua_pushinteger(L, sd.inputs[j] + sio->inputs[j].def);
    }
  }
  else if ((scriptType & RUN_FUNC_SCRIPT) && (sid.reference >= SCRIPT_FUNC_FIRST && sid.reference <= SCRIPT_FUNC_LAST)) {
    CustomFunctionData & fn = g_model.customFn[sid.reference-SCRIPT_FUNC_FIRST];
    if (!getSwitch(fn.swtch)) return false;
#if defined(SIMU) || defined(DEBUG)
    filename = fn.play.name;
#endif
    lua_rawgeti(L, LUA_REGISTRYINDEX, sid.run);
  }
  else {
#if defined(SIMU) || defined(DEBUG)
    TelemetryScriptData & script = g_model.frsky.screens[sid.reference-SCRIPT_TELEMETRY_FIRST].script;
    filename = script.file;
#endif
    if ((scriptType & RUN_TELEM_FG_SCRIPT) &&
#if defined(COLORLCD)
        (menuHandlers[0]==menuMainView && sid.reference==SCRIPT_TELEMETRY_FIRST+g_eeGeneral.view-VIEW_TELEM1)) {
#else
        (menuHandlers[0]==menuTelemetryFrsky && sid.reference==SCRIPT_TELEMETRY_FIRST+s_frsky_view)) {
#endif
      lua_rawgeti(L, LUA_REGISTRYINDEX, sid.run);
      lua_pushinteger(L, evt);
      inputsCount = 1;
    }
    else if ((scriptType & RUN_TELEM_BG_SCRIPT) && (sid.background)) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, sid.background);
    }
    else {
      return false;
    }
  }

  if (lua_pcall(L, inputsCount, sio ? sio->outputsCount : 0, 0) == 0) {
    if (sio) {
      for (int j=sio->outputsCount-1; j>=0; j--) {
        if (!lua_isnumber(L, -1)) {
          sid.state = (instructionsPercent > 100 ? SCRIPT_KILLED : SCRIPT_SYNTAX_ERROR);
          TRACE("Script %8s disabled", filename);
          break;
        }
        sio->outputs[j].value = lua_tointeger(L, -1);
        lua_pop(L, 1);
      }
    }
  }
  else {
    if (instructionsPercent > 100) {
      TRACE("Script %8s killed", filename);
      sid.state = SCRIPT_KILLED;
    }
    else {
      TRACE("Script %8s error: %s", filename, lua_tostring(L, -1));
      sid.state = SCRIPT_SYNTAX_ERROR;
    }
  }

  if (sid.state != SCRIPT_OK) {
    luaFree(sid);
  }
  else {
    if (instructionsPercent > sid.instructions) {
      sid.instructions = instructionsPercent;
    }
  }
  return true;
}

void luaDoGc()
{
  if (L) {
    PROTECT_LUA() {
      lua_gc(L, LUA_GCCOLLECT, 0);
#if defined(SIMU) || defined(DEBUG)
      static int lastgc = 0;
      int gc = luaGetMemUsed();
      if (gc != lastgc) {
        lastgc = gc;
        TRACE("GC Use: %dbytes", gc);
      }
#endif
    }
    else {
      // we disable Lua for the rest of the session
      luaDisable();
    }
    UNPROTECT_LUA();
  }
}

bool luaTask(uint8_t evt, uint8_t scriptType, bool allowLcdUsage)
{
  if (luaState == INTERPRETER_PANIC) return false;
  luaLcdAllowed = allowLcdUsage;
  bool scriptWasRun = false;

  // we run either standalone script or permanent scripts
  if (luaState & INTERPRETER_RUNNING_STANDALONE_SCRIPT) {
    // run standalone script
    if ((scriptType & RUN_STNDAL_SCRIPT) == 0) return false;
    PROTECT_LUA() {
      luaDoOneRunStandalone(evt);
      scriptWasRun = true;
    }
    else {
      luaDisable();
      return false;
    }
    UNPROTECT_LUA();
  }
  else {
    // run permanent scripts
    if (luaState & INTERPRETER_RELOAD_PERMANENT_SCRIPTS) {
      luaState = 0;
      luaInit();
      if (luaState == INTERPRETER_PANIC) return false;
      luaLoadPermanentScripts();
      if (luaState == INTERPRETER_PANIC) return false;
    }

    for (int i=0; i<luaScriptsCount; i++) {
      PROTECT_LUA() {
        scriptWasRun |= luaDoOneRunPermanentScript(evt, i, scriptType);
      }
      else {
        luaDisable();
        break;
      }
      UNPROTECT_LUA();
      //todo gc step between scripts
    }
  }
  luaDoGc();
  return scriptWasRun;
}

int luaGetMemUsed()
{
  return (lua_gc(L, LUA_GCCOUNT, 0) << 10) + lua_gc(L, LUA_GCCOUNTB, 0);
}
