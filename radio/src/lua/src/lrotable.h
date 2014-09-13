// Read-only tables for Lua

#ifndef lrotable_h
#define lrotable_h

#include "lua.h"
#include "llimits.h"
#include "lauxlib.h"

typedef lua_Number luaR_result;

// A number entry in the read only table
typedef struct
{
  const char *name;
  lua_Number value;
} luaR_value_entry;

extern const luaR_value_entry baselib_vals[];
extern const luaR_value_entry mathlib_vals[];
extern const luaR_value_entry opentxConstants[];

// A mapping between table name and its entries
typedef struct
{
  const char *name;
  const luaL_Reg *pfuncs;
  const luaR_value_entry *pvalues;
} luaR_table;

extern const luaR_table lua_rotable[];

luaR_result luaR_findglobal(const char *key, lu_byte *ptype);
int luaR_findfunction(lua_State *L, const luaL_Reg *ptable);
luaR_result luaR_findentry(void *data, const char *key, lu_byte *ptype);

#endif
