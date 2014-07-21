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

#if defined(LUA)

#define SWAP_DEFINED
#include "opentx.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void luaExecStr(const char * str)
{
  extern lua_State * L;
  luaL_dostring(L, str);
}

TEST(Lua, testSetModelId)
{
  luaExecStr("info = model.getInfo()");
  luaExecStr("info.id = 2");
  luaExecStr("model.setInfo(info)");
  EXPECT_EQ(g_model.header.modelId, 2);
}

TEST(Lua, testSetTelemetryChannel)
{
  luaExecStr("channel = model.getTelemetryChannel(0)");
  luaExecStr("channel.range = 100.0");
  luaExecStr("channel.offset = -10.0");
  luaExecStr("channel.alarm1 = 60");
  luaExecStr("channel.alarm2 = 50");
  luaExecStr("model.setTelemetryChannel(0, channel)");
  EXPECT_EQ(g_model.frsky.channels[0].multiplier, 2);
  EXPECT_EQ(g_model.frsky.channels[0].ratio, 250);
  EXPECT_EQ(g_model.frsky.channels[0].offset, -26);
  EXPECT_EQ(g_model.frsky.channels[0].alarms_value[0], 179);
  EXPECT_EQ(g_model.frsky.channels[0].alarms_value[1], 153);
}

#endif
