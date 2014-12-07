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

#ifndef luaapi_h
#define luaapi_h


#define RUN_MIX_SCRIPT        (1 << 0)
#define RUN_FUNC_SCRIPT       (1 << 1)
#define RUN_TELEM_BG_SCRIPT   (1 << 2)
#define RUN_TELEM_FG_SCRIPT   (1 << 3)
#define RUN_STNDAL_SCRIPT     (1 << 4)


#if defined(LUA)
  struct ScriptInput {
    const char *name;
    uint8_t type;
    int16_t min;
    int16_t max;
    int16_t def;
  };
  struct ScriptOutput {
    const char *name;
    int16_t value;
  };
  enum ScriptState {
    SCRIPT_OK,
    SCRIPT_NOFILE,
    SCRIPT_SYNTAX_ERROR,
    SCRIPT_PANIC,
    SCRIPT_KILLED
  };
  enum ScriptReference {
    SCRIPT_MIX_FIRST,
    SCRIPT_MIX_LAST=SCRIPT_MIX_FIRST+MAX_SCRIPTS-1,
    SCRIPT_FUNC_FIRST,
    SCRIPT_FUNC_LAST=SCRIPT_FUNC_FIRST+NUM_CFN-1,
    SCRIPT_TELEMETRY_FIRST,
    SCRIPT_TELEMETRY_LAST=SCRIPT_TELEMETRY_FIRST+MAX_SCRIPTS, // telem0 and telem1 .. telem7
  };
  struct ScriptInternalData {
    uint8_t reference;
    uint8_t state;
    int run;
    int background;
    uint8_t instructions;
  };
  struct ScriptInputsOutputs {
    uint8_t inputsCount;
    ScriptInput inputs[MAX_SCRIPT_INPUTS];
    uint8_t outputsCount;
    ScriptOutput outputs[MAX_SCRIPT_OUTPUTS];
  };
  #define INTERPRETER_RUNNING_STANDALONE_SCRIPT 1
  #define INTERPRETER_RELOAD_PERMANENT_SCRIPTS  2
  #define INTERPRETER_PANIC                     255
  extern uint8_t luaState;
  extern uint8_t luaScriptsCount;
  extern ScriptInternalData standaloneScript;
  extern ScriptInternalData scriptInternalData[MAX_SCRIPTS];
  extern ScriptInputsOutputs scriptInputsOutputs[MAX_SCRIPTS];
  void luaClose();
  bool luaTask(uint8_t evt, uint8_t scriptType, bool allowLcdUsage);
  void luaExec(const char *filename);
  int luaGetMemUsed();
  #define luaGetCpuUsed(idx) scriptInternalData[idx].instructions
  #define LUA_LOAD_MODEL_SCRIPTS()   luaState |= INTERPRETER_RELOAD_PERMANENT_SCRIPTS
  #define LUA_LOAD_MODEL_SCRIPT(idx) luaState |= INTERPRETER_RELOAD_PERMANENT_SCRIPTS
  #define LUA_STANDALONE_SCRIPT_RUNNING() (luaState == INTERPRETER_RUNNING_STANDALONE_SCRIPT)
  // Lua PROTECT/UNPROTECT
  #include <setjmp.h>
  struct our_longjmp {
    struct our_longjmp *previous;
    jmp_buf b;
    volatile int status;  /* error code */
  };
  extern struct our_longjmp * global_lj;
  #define PROTECT_LUA()   { struct our_longjmp lj; \
                          lj.previous = global_lj;  /* chain new error handler */ \
                          global_lj = &lj;  \
                          if (setjmp(lj.b) == 0)
  #define UNPROTECT_LUA() global_lj = lj.previous; }   /* restore old error handler */

  extern uint16_t maxLuaInterval;
  extern uint16_t maxLuaDuration;
#else  // #if defined(LUA)
  #define LUA_LOAD_MODEL_SCRIPTS()
  #define LUA_LOAD_MODEL_SCRIPT(idx)
  #define LUA_STANDALONE_SCRIPT_RUNNING() (0)
#endif

#endif // #ifndef luaapi_h
