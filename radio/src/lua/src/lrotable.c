/* Read-only tables for Lua */

#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lrotable.h"

/* Local defines */
#define LUAR_FINDFUNCTION     0
#define LUAR_FINDVALUE        1

/* Utility function: find a key in a given table (of functions or constants) */
static luaR_result luaR_findkey(const void *where, const char *key, int type, int *found) {
  const char *pname;
  const luaL_Reg *pf = (luaL_Reg*)where;
  const luaR_value_entry *pv = (luaR_value_entry*)where;
  int isfunction = type == LUAR_FINDFUNCTION;
  *found = 0;
  if(!where)
    return 0;
  while(1) {
    if (!(pname = isfunction ? pf->name : pv->name))
      break;
    if (!strcmp(pname, key)) {
      *found = 1;
      return isfunction ? (luaR_result)(size_t)pf->func : (luaR_result)pv->value;
    }
    pf ++; pv ++;
  }
  return 0;
}

/* Find a global "read only table" in the constant lua_rotable array */
luaR_result luaR_findglobal(const char *name, lu_byte *ptype) {
  unsigned i;    
  *ptype = LUA_TNIL;
  if (strlen(name) > LUA_MAX_ROTABLE_NAME)
    return 0;
  for (i=0; lua_rotable[i].name; i++) {
    if (!strcmp(lua_rotable[i].name, name)) {
      *ptype = LUA_TROTABLE;
      return i+1;
    }
    if (!strncmp(lua_rotable[i].name, "__", 2)) {
      luaR_result result = luaR_findentry((void *)(size_t)(i+1), name, ptype);
      if (result != 0) {
    	return result;
      }
    }
  }
  return 0;
}


int luaR_findfunction(lua_State *L, const luaL_Reg *ptable) {
  int found;
  const char *key = luaL_checkstring(L, 2);
  luaR_result res = luaR_findkey(ptable, key, LUAR_FINDFUNCTION, &found);
  if (found)
    lua_pushlightfunction(L, (void*)(size_t)res);
  else
    lua_pushnil(L);
  return 1; 
}

luaR_result luaR_findentry(void *data, const char *key, lu_byte *ptype) {
  int found;
  unsigned idx = (unsigned)(size_t)data - 1;
  luaR_result res;   
  *ptype = LUA_TNIL;
  /* First look at the functions */
  res = luaR_findkey(lua_rotable[idx].pfuncs, key, LUAR_FINDFUNCTION, &found);
  if (found) {
    *ptype = LUA_TLIGHTFUNCTION;
    return res;
  }
  else {
    /* Then at the values */
    res = luaR_findkey(lua_rotable[idx].pvalues, key, LUAR_FINDVALUE, &found);
    if(found) {
      *ptype = LUA_TNUMBER;
      return res;
    }
  }
  return 0;
}
