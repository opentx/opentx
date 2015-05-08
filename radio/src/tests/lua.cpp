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

#include <math.h>
#include <gtest/gtest.h>
#include "gtests.h"

#if defined(LUA)

#define SWAP_DEFINED
#include "opentx.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

extern const char * zchar2string(const char * zstring, int size);
#define EXPECT_ZSTREQ(c_string, z_string)   EXPECT_STREQ(c_string, zchar2string(z_string, sizeof(z_string)))
extern void luaInit();

::testing::AssertionResult __luaExecStr(const char * str)
{
  extern lua_State * L;
  if (!L) luaInit();
  if (!L) return ::testing::AssertionFailure() << "No Lua state!";
  if (luaL_dostring(L, str)) {
    return ::testing::AssertionFailure() << "lua error: " << lua_tostring(L, -1);
  }
  return ::testing::AssertionSuccess();
}

#define luaExecStr(test)  EXPECT_TRUE(__luaExecStr(test))

TEST(Lua, testSetModelInfo)
{
  luaExecStr("info = model.getInfo()");
  // luaExecStr("print('model name: '..info.name..' id: '..info.id)");
  luaExecStr("info.name = 'modelA'");
  luaExecStr("model.setInfo(info)");
  // luaExecStr("print('model name: '..info.name..' id: '..info.id)");
  EXPECT_ZSTREQ("modelA", g_model.header.name);

  luaExecStr("info.name = 'Model 1'");
  luaExecStr("model.setInfo(info)");
  // luaExecStr("print('model name: '..info.name..' id: '..info.id)");
  EXPECT_ZSTREQ("Model 1", g_model.header.name);
}

TEST(Lua, testPanicProtection)
{
  bool passed = false;
  PROTECT_LUA() {
    PROTECT_LUA() {
      //simulate panic
      longjmp(global_lj->b, 1);
    }
    else {
      //we should come here
      passed = true;
    }
    UNPROTECT_LUA();
  }
  else {
    // an not here
    // TRACE("testLuaProtection: test 1 FAILED");
    FAIL() << "Failed test 1";
  }
  UNPROTECT_LUA()

  EXPECT_EQ(passed, true);

  passed = false;

  PROTECT_LUA() {
    PROTECT_LUA() {
      int a = 5;
      a = a; // avoids the warning
    }
    else {
      //we should not come here
      // TRACE("testLuaProtection: test 2 FAILED");
      FAIL() << "Failed test 2";
    }
    UNPROTECT_LUA()
    //simulate panic
    longjmp(global_lj->b, 1);
  }
  else {
    // we should come here
    passed = true;
  }
  UNPROTECT_LUA()

  EXPECT_EQ(passed, true);
}

TEST(Lua, testModelInputs)
{
  MODEL_RESET();
  luaExecStr("noInputs = model.getInputsCount(0)");
  luaExecStr("if noInputs > 0 then error('getInputsCount()') end");

  // add one line on Input4
  luaExecStr("model.insertInput(3, 0, {name='test1', source=MIXSRC_Thr, weight=56, offset=3, switch=2})");
  EXPECT_EQ(3, g_model.expoData[0].chn);
  EXPECT_ZSTREQ("test1", g_model.expoData[0].name);
  EXPECT_EQ(MIXSRC_Thr, g_model.expoData[0].srcRaw);
  EXPECT_EQ(56, g_model.expoData[0].weight);
  EXPECT_EQ(3, g_model.expoData[0].offset);
  EXPECT_EQ(2, g_model.expoData[0].swtch);

  // add another one before existing line on Input4
  luaExecStr("model.insertInput(3, 0, {name='test2', source=MIXSRC_Rud, weight=-56})");
  EXPECT_EQ(3, g_model.expoData[0].chn);
  EXPECT_ZSTREQ("test2", g_model.expoData[0].name);
  EXPECT_EQ(MIXSRC_Rud, g_model.expoData[0].srcRaw);
  EXPECT_EQ(-56, g_model.expoData[0].weight);
  EXPECT_EQ(0, g_model.expoData[0].offset);
  EXPECT_EQ(0, g_model.expoData[0].swtch);

  EXPECT_EQ(3, g_model.expoData[1].chn);
  EXPECT_ZSTREQ("test1", g_model.expoData[1].name);
  EXPECT_EQ(MIXSRC_Thr, g_model.expoData[1].srcRaw);
  EXPECT_EQ(56, g_model.expoData[1].weight);
  EXPECT_EQ(3, g_model.expoData[1].offset);
  EXPECT_EQ(2, g_model.expoData[1].swtch);

  // add another line after existing lines on Input4
  luaExecStr("model.insertInput(3, model.getInputsCount(3), {name='test3', source=MIXSRC_Ail, weight=100})");
  EXPECT_EQ(3, g_model.expoData[0].chn);
  EXPECT_ZSTREQ("test2", g_model.expoData[0].name);
  EXPECT_EQ(MIXSRC_Rud, g_model.expoData[0].srcRaw);
  EXPECT_EQ(-56, g_model.expoData[0].weight);
  EXPECT_EQ(0, g_model.expoData[0].offset);
  EXPECT_EQ(0, g_model.expoData[0].swtch);

  EXPECT_EQ(3, g_model.expoData[1].chn);
  EXPECT_ZSTREQ("test1", g_model.expoData[1].name);
  EXPECT_EQ(MIXSRC_Thr, g_model.expoData[1].srcRaw);
  EXPECT_EQ(56, g_model.expoData[1].weight);
  EXPECT_EQ(3, g_model.expoData[1].offset);
  EXPECT_EQ(2, g_model.expoData[1].swtch);

  EXPECT_EQ(3, g_model.expoData[2].chn);
  EXPECT_ZSTREQ("test3", g_model.expoData[2].name);
  EXPECT_EQ(MIXSRC_Ail, g_model.expoData[2].srcRaw);
  EXPECT_EQ(100, g_model.expoData[2].weight);
  EXPECT_EQ(0, g_model.expoData[2].offset);
  EXPECT_EQ(0, g_model.expoData[2].swtch);

  // verify number of lines for Input4
  luaExecStr("noInputs = model.getInputsCount(3)");
  luaExecStr("if noInputs ~= 3 then error('getInputsCount()') end");

}

#endif   // #if defined(LUA)
