/* Read-only tables for Lua */

#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lrotable.h"

/* Local defines */
#define LUAR_FINDFUNCTION     0
#define LUAR_FINDVALUE        1

/* Utility function: find a key in a given table (of functions or constants) */
static luaR_result luaR_findkey(const void * where, const char * key, int type, TValue * found) {
  const char *pname;
  const luaL_Reg *pf = (luaL_Reg*)where;
  const luaR_value_entry *pv = (luaR_value_entry*)where;
  int isfunction = type == LUAR_FINDFUNCTION;
  if(!where)
    return 0;
  while(1) {
    if (!(pname = isfunction ? pf->name : pv->name))
      break;
    if (!strcmp(pname, key)) {
      if (isfunction) {
        setlfvalue(found, pf->func);
      }
      else {
        setnvalue(found, pv->value);
      }
      return 1;
    }
    pf ++; pv ++;
  }
  return 0;
}

/* Find a global "read only table" in the constant lua_rotable array */
luaR_result luaR_findglobal(const char * name, TValue * val) {
  unsigned i;
  if (strlen(name) > LUA_MAX_ROTABLE_NAME) {
    TRACE_LUA_INTERNALS("luaR_findglobal() '%s' = NAME TOO LONG", name);
    return 0;
  }
  for (i=0; lua_rotable[i].name; i++) {
    if (!strcmp(lua_rotable[i].name, name)) {
      setrvalue(val, (void *)(size_t)(i+1));
      TRACE_LUA_INTERNALS("luaR_findglobal() '%s' = TABLE %d", name, i);
      return 1;
    }
    if (!strncmp(lua_rotable[i].name, "__", 2)) {
      if (luaR_findentry((void *)(size_t)(i+1), name, val)) {
        TRACE_LUA_INTERNALS("luaR_findglobal() '%s' = FOUND in table '%s'", name, lua_rotable[i].name);
        return 1;
      }
    }
  }
  TRACE_LUA_INTERNALS("luaR_findglobal() '%s' = NOT FOUND", name);
  return 0;
}


luaR_result luaR_findentry(void *data, const char * key, TValue * val) {
  unsigned idx = (unsigned)(size_t)data - 1;
  /* First look at the functions */
  if (luaR_findkey(lua_rotable[idx].pfuncs, key, LUAR_FINDFUNCTION, val)) {
    TRACE_LUA_INTERNALS("luaR_findentry(%d, '%s') = FUNCTION %p", idx, key, lfvalue(val));
    return 1;
  }
  else if (luaR_findkey(lua_rotable[idx].pvalues, key, LUAR_FINDVALUE, val)) {
    /* Then at the values */
    TRACE_LUA_INTERNALS("luaR_findentry(%d, '%s') = NUMBER %g", idx, key, nvalue(val));
    return 1;
  }
  TRACE_LUA_INTERNALS("luaR_findentry(%d, '%s') = NOT FOUND", idx, key);
  return 0;
}
