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

#include <ctype.h>
#include <stdio.h>
#include "opentx.h"
#include "lua/lua_api.h"

/*luadoc
@function lcd.refresh()

Refresh the LCD screen

@status current Introduced in 2.2.0

@notice This function only works in stand-alone and telemetry scripts.
*/
static int luaLcdRefresh(lua_State *L)
{
  if (luaLcdAllowed) lcdRefresh();
  return 0;
}

/*luadoc
@function lcd.clear()

Clear the LCD screen

@status current Introduced in 2.0.0

@notice This function only works in stand-alone and telemetry scripts.
*/
static int luaLcdClear(lua_State *L)
{
  if (luaLcdAllowed) lcdClear();
  return 0;
}


/*luadoc
@function lcd.drawPoint(x, y)

Draw a single pixel at (x,y) position

@param x (positive number) x position

@param y (positive number) y position

@notice Taranis has an LCD display width of 212 pixels and height of 64 pixels.
Position (0,0) is at top left. Y axis is negative, top line is 0,
bottom line is 63. Drawing on an existing black pixel produces white pixel (TODO check this!)

@status current Introduced in 2.0.0
*/
static int luaLcdDrawPoint(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  lcdDrawPoint(x, y);
  return 0;
}

/*luadoc
@function lcd.drawLine(x1, y1, x2, y2, pattern, flags)

Draw a straight line on LCD

@param x1,y1 (positive numbers) starting coordinate

@param x2,y2 (positive numbers) end coordinate

@param pattern TODO

@param flags TODO

@notice If the start or the end of the line is outside the LCD dimensions, then the
whole line will not be drawn (starting from OpenTX 2.1.5)

@status current Introduced in 2.0.0
*/
static int luaLcdDrawLine(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x1 = luaL_checkinteger(L, 1);
  int y1 = luaL_checkinteger(L, 2);
  int x2 = luaL_checkinteger(L, 3);
  int y2 = luaL_checkinteger(L, 4);
  int pat = luaL_checkinteger(L, 5);
  int flags = luaL_checkinteger(L, 6);

  if (pat == SOLID) {
    if (x1 == x2) {
      lcdDrawSolidVerticalLine(x1, y2 >= y1 ? y1 : y1+1, y2 >= y1 ? y2-y1+1 : y2-y1-1, flags);
      return 0;
    }
    else if (y1 == y2) {
      lcdDrawSolidHorizontalLine(x2 >= x1 ? x1 : x1+1, y1, x2 >= x1 ? x2-x1+1 : x2-x1-1, flags);
      return 0;
    }
  }

  lcdDrawLine(x1, y1, x2, y2, pat, flags);
  return 0;
}

#if !defined(COLORLCD)
/*luadoc
@function lcd.getLastPos()

Returns the last x position from previous output

@retval number (integer) x position

@notice Only available on Taranis and X7D

@status current Introduced in 2.0.0
*/
static int luaLcdGetLastPos(lua_State *L)
{
  lua_pushinteger(L, lcdLastPos);
  return 1;
}
#endif

/*luadoc
@function lcd.drawText(x, y, text [, flags])

Draw a text beginning at (x,y)

@param x,y (positive numbers) starting coordinate

@param text (string) text to display

@param flags (unsigned number) drawing flags. All values can be
combined together using the + character. ie BLINK + DBLSIZE.
See the Appendix for available characters in each font set.
 * `0 or not specified` normal font
 * `XXLSIZE` jumbo sized font
 * `DBLSIZE` double size font
 * `MIDSIZE` mid sized font
 * `SMLSIZE` small font
 * `INVERS` inverted display
 * `BLINK` blinking text

@status current Introduced in 2.0.0
*/
static int luaLcdDrawText(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  const char * s = luaL_checkstring(L, 3);
  unsigned int att = luaL_optunsigned(L, 4, 0);
  lcdDrawText(x, y, s, att);
  return 0;
}

/*luadoc
@function lcd.drawTimer(x, y, value [, flags])

Display a value formatted as time at (x,y)

@param x,y (positive numbers) starting coordinate

@param value (number) time in seconds

@param flags (unsigned number) drawing flags:
 * `0 or not specified` normal representation (minutes and seconds)
 * `TIMEHOUR` display hours
 * other general LCD flag also apply

@status current Introduced in 2.0.0
*/
static int luaLcdDrawTimer(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int seconds = luaL_checkinteger(L, 3);
  unsigned int att = luaL_optunsigned(L, 4, 0);
#if defined(COLORLCD)
  drawTimer(x, y, seconds, att|LEFT);
#else
  drawTimer(x, y, seconds, att|LEFT, att);
#endif
  return 0;
}

/*luadoc
@function lcd.drawNumber(x, y, value [, flags])

Display a number at (x,y)

@param x,y (positive numbers) starting coordinate

@param value (number) value to display

@param flags (unsigned number) drawing flags:
 * `0 or not specified` normal representation
 * `PREC1` display with one decimal place (number 386 is displayed as 38.6)
 * `PREC2` display with tow decimal places (number 386 is displayed as 3.86)
 * other general LCD flag also apply

@status current Introduced in 2.0.0
*/
static int luaLcdDrawNumber(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int val = luaL_checkinteger(L, 3);
  unsigned int att = luaL_optunsigned(L, 4, 0);
  lcdDrawNumber(x, y, val, att);
  return 0;
}

/*luadoc
@function lcd.drawChannel(x, y, source, flags)

Display a telemetry value at (x,y)

@param x,y (positive numbers) starting coordinate

@param source can be a source identifier (number) or a source name (string).
See getValue()

@param flags (unsigned number) drawing flags

@status current Introduced in 2.0.6, changed in 2.1.0 (only telemetry sources are valid)
*/
static int luaLcdDrawChannel(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int channel = -1;
  if (lua_isnumber(L, 3)) {
    channel = luaL_checkinteger(L, 3);
  }
  else {
    const char * what = luaL_checkstring(L, 3);
    LuaField field;
    bool found = luaFindFieldByName(what, field);
    if (found) {
      channel = field.id;
    }
  }
  unsigned int att = luaL_optunsigned(L, 4, 0);
  getvalue_t value = getValue(channel);
  drawSensorCustomValue(x, y, (channel-MIXSRC_FIRST_TELEM)/3, value, att);
  return 0;
}

/*luadoc
@function lcd.drawSwitch(x, y, switch, flags)

Draw a text representation of switch at (x,y)

@param x,y (positive numbers) starting coordinate

@param switch (number) number of switch to display, negative number
displays negated switch

@param flags (unsigned number) drawing flags, only SMLSIZE, BLINK and INVERS.

@status current Introduced in 2.0.0
*/
static int luaLcdDrawSwitch(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int s = luaL_checkinteger(L, 3);
  unsigned int att = luaL_optunsigned(L, 4, 0);
  drawSwitch(x, y, s, att);
  return 0;
}

/*luadoc
@function lcd.drawSource(x, y, source [, flags])

Displays the name of the corresponding input as defined by the source at (x,y)

@param x,y (positive numbers) starting coordinate

@param source (number) source index

@param flags (unsigned number) drawing flags

@status current Introduced in 2.0.0
*/
static int luaLcdDrawSource(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int s = luaL_checkinteger(L, 3);
  unsigned int att = luaL_optunsigned(L, 4, 0);
  drawSource(x, y, s, att);
  return 0;
}

#if defined(COLORLCD)

#define LUA_BITMAPHANDLE          "BITMAP*"

/*luadoc
@function Bitmap.open(name)

Loads a bitmap in memory, for later use with lcd.drawBitmap()

@param name (string) full path to the bitmap on SD card (i.e. “/IMAGES/test.bmp”)

@notice Only available on Horus

@status current Introduced in 2.2.0
*/
static int luaOpenBitmap(lua_State * L)
{
  const char * filename = luaL_checkstring(L, 1);

  BitmapBuffer ** ptr = (BitmapBuffer **)lua_newuserdata(L, sizeof(BitmapBuffer *));
  *ptr = BitmapBuffer::load(filename);
  TRACE("luaOpenBitmap: %p", *ptr);

  if (*ptr == NULL && G(L)->gcrunning) {
    luaC_fullgc(L, 1);  /* try to free some memory... */
    *ptr = BitmapBuffer::load(filename);  /* try again */
     TRACE("luaOpenBitmap: %p (second try)", *ptr);
  }

  luaL_getmetatable(L, LUA_BITMAPHANDLE);
  lua_setmetatable(L, -2);

  return 1;
}

static BitmapBuffer * checkBitmap(lua_State * L, int index)
{
  BitmapBuffer ** b = (BitmapBuffer **)luaL_checkudata(L, index, LUA_BITMAPHANDLE);
  if (!*b) luaL_error(L, "null Image");
  return *b;
}


/*luadoc
@function Bitmap.getSize(name)

Return width, heigh of a bitmap in memory

@param bitmap (pointer) point to a bitmap previously opened with Bipmap.open()

@notice Only available on Horus

@status current Introduced in 2.2.0
*/
static int luaGetBitmapSize(lua_State * L)
{
  BitmapBuffer * b = checkBitmap(L, 1);
  if (b) {
    lua_pushinteger(L, b->getWidth());
    lua_pushinteger(L, b->getHeight());
  }
  else {
    lua_pushinteger(L, 0);
    lua_pushinteger(L, 0);
  }
  return 2;
}

static int luaDestroyBitmap(lua_State * L)
{
  BitmapBuffer * ptr = checkBitmap(L, 1);
  TRACE("luaDestroyBitmap: %p", ptr);
  delete ptr;
  return 0;
}

const luaL_Reg bitmapFuncs[] = {
  { "open", luaOpenBitmap },
  { "getSize", luaGetBitmapSize },
  { "__gc", luaDestroyBitmap },
  { NULL, NULL }
};

void registerBitmapClass(lua_State * L)
{
  luaL_newmetatable(L, LUA_BITMAPHANDLE);
  luaL_setfuncs(L, bitmapFuncs, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_setglobal(L, "Bitmap");
}

/*luadoc
@function lcd.drawBitmap(bitmap, x, y [, scale])

Displays a bitmap at (x,y)

@param bitmap (pointer) point to a bitmap previously opened with Bipmap.open()

@param x,y (positive numbers) starting coordinates

@param scale (positive numbers) scale in %, 50 divides size by two, 100 is unchanged, 200 doubles size

@notice Only available on Horus

@status current Introduced in 2.2.0
*/
static int luaLcdDrawBitmap(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  const BitmapBuffer * bitmap = checkBitmap(L, 1);
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);
  unsigned scale = luaL_optunsigned(L, 4, 0);

  if (bitmap) {
    lcd->drawBitmap(x, y, bitmap, 0, 0, 0, 0, (float) scale/100);
  }

  return 0;
}
#elif LCD_DEPTH > 1
/*luadoc
@function lcd.drawPixmap(x, y, name)

Draw a bitmap at (x,y)

@param x,y (positive numbers) starting coordinates

@param name (string) full path to the bitmap on SD card (i.e. “/IMAGES/test.bmp”)

@notice Only available on Taranis

@status current Introduced in 2.0.0
*/
static int luaLcdDrawPixmap(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  const char * filename = luaL_checkstring(L, 3);

  uint8_t bitmap[BITMAP_BUFFER_SIZE(LCD_W/2, LCD_H)]; // width max is LCD_W/2 pixels for saving stack and avoid a malloc here
  if (lcdLoadBitmap(bitmap, filename, LCD_W/2, LCD_H)) {
    lcdDrawBitmap(x, y, bitmap);
  }

  return 0;
}
#endif

/*luadoc
@function lcd.drawRectangle(x, y, w, h [, flags [, t]])

Draw a rectangle from top left corner (x,y) of specified width and height

@param x,y (positive numbers) top left corner position

@param w (number) width in pixels

@param h (number) height in pixels

@param flags (unsigned number) drawing flags

@param t (number) thickness in pixels, defaults to 1 (only on Horus)

@status current Introduced in 2.0.0, changed in 2.2.0
*/
static int luaLcdDrawRectangle(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int w = luaL_checkinteger(L, 3);
  int h = luaL_checkinteger(L, 4);
  unsigned int flags = luaL_optunsigned(L, 5, 0);
#if defined(PCBHORUS)
  unsigned int t = luaL_optunsigned(L, 6, 1);
  lcdDrawRect(x, y, w, h, t, 0xff, flags);
#else
  lcdDrawRect(x, y, w, h, 0xff, flags);
#endif
  return 0;
}

/*luadoc
@function lcd.drawFilledRectangle(x, y, w, h [, flags])

Draw a solid rectangle from top left corner (x,y) of specified width and height

@param x,y (positive numbers) top left corner position

@param w (number) width in pixels

@param h (number) height in pixels

@param flags (unsigned number) drawing flags

@status current Introduced in 2.0.0
*/
static int luaLcdDrawFilledRectangle(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int w = luaL_checkinteger(L, 3);
  int h = luaL_checkinteger(L, 4);
  unsigned int flags = luaL_optunsigned(L, 5, 0);
  lcdDrawFilledRect(x, y, w, h, SOLID, flags);
  return 0;
}


/*luadoc
@function lcd.drawGauge(x, y, w, h, fill, maxfill [, flags])

Draw a simple gauge that is filled based upon fill value

@param x,y (positive numbers) top left corner position

@param w (number) width in pixels

@param h (number) height in pixels

@param fill (number) amount of fill to apply

@param maxfill (number) total value of fill

@param flags (unsigned number) drawing flags

@status current Introduced in 2.0.0, changed in 2.2.0
*/
static int luaLcdDrawGauge(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int w = luaL_checkinteger(L, 3);
  int h = luaL_checkinteger(L, 4);
  int num = luaL_checkinteger(L, 5);
  int den = luaL_checkinteger(L, 6);
  unsigned int flags = luaL_optunsigned(L, 7, 0);
#if defined(PCBHORUS)
  lcdDrawRect(x, y, w, h, 1, 0xff, flags);
#else
  lcdDrawRect(x, y, w, h, 0xff, flags);
#endif
  uint8_t len = limit((uint8_t)1, uint8_t(w*num/den), uint8_t(w));
  lcdDrawSolidFilledRect(x+1, y+1, len, h-2, flags);
  return 0;
}


#if LCD_DEPTH > 1 && !defined(COLORLCD)
/*luadoc
@function lcd.drawScreenTitle(title, page, pages)

Draw a title bar

@param title (string) text for the title

@param page (number) page number

@param pages (number) total number of pages. Only used as indicator on
the right side of title bar. (i.e. idx=2, cnt=5, display `2/5`)

@notice Only available on Taranis

@status current Introduced in 2.0.0
*/
static int luaLcdDrawScreenTitle(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  const char * str = luaL_checkstring(L, 1);
  int idx = luaL_checkinteger(L, 2);
  int cnt = luaL_checkinteger(L, 3);

  if (cnt) drawScreenIndex(idx-1, cnt, 0);
  lcdDrawFilledRect(0, 0, LCD_W, FH, SOLID, FILL_WHITE|GREY_DEFAULT);
  title(str);

  return 0;
}
#endif

#if LCD_DEPTH > 1 && !defined(COLORLCD)
/*luadoc
@function lcd.drawCombobox(x, y, w, list, idx [, flags])

Draw a combo box

@param x,y (positive numbers) top left corner position

@param w (number) width of combo box in pixels

@param list (table) combo box elements, each element is a string

@param idx (integer) index of entry to highlight

@param page (number) page number

@param flags (unsigned number) drawing flags, the flags can not be combined:
 * `BLINK` combo box is expanded
 * `INVERS` combo box collapsed, text inversed
 * `0 or not present` combo box collapsed, text normal

@notice Only available on Taranis

@status current Introduced in 2.0.0
*/
static int luaLcdDrawCombobox(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int x = luaL_checkinteger(L, 1);
  int y = luaL_checkinteger(L, 2);
  int w = luaL_checkinteger(L, 3);
  luaL_checktype(L, 4, LUA_TTABLE);
  int count = luaL_len(L, 4);  /* get size of table */
  int idx = luaL_checkinteger(L, 5);
  unsigned int flags = luaL_optunsigned(L, 6, 0);
  if (idx >= count) {
    // TODO error
  }
  if (flags & BLINK) {
    lcdDrawFilledRect(x, y, w-9, count*9+2, SOLID, ERASE);
    lcdDrawRect(x, y, w-9, count*9+2);
    for (int i=0; i<count; i++) {
      lua_rawgeti(L, 4, i+1);
      const char * item = luaL_checkstring(L, -1);
      lcdDrawText(x+2, y+2+9*i, item, 0);
    }
    lcdDrawFilledRect(x+1, y+1+9*idx, w-11, 9);
    lcdDrawFilledRect(x+w-10, y, 10, 11, SOLID, ERASE);
    lcdDrawRect(x+w-10, y, 10, 11);
  }
  else if (flags & INVERS) {
    lcdDrawFilledRect(x, y, w, 11);
    lcdDrawFilledRect(x+w-9, y+1, 8, 9, SOLID, ERASE);
    lua_rawgeti(L, 4, idx+1);
    const char * item = luaL_checkstring(L, -1);
    lcdDrawText(x+2, y+2, item, INVERS);
  }
  else {
    lcdDrawFilledRect(x, y, w, 11, SOLID, ERASE);
    lcdDrawRect(x, y, w, 11);
    lcdDrawFilledRect(x+w-10, y+1, 9, 9, SOLID);
    lua_rawgeti(L, 4, idx+1);
    const char * item = luaL_checkstring(L, -1);
    lcdDrawText(x+2, y+2, item, 0);
  }

  lcdDrawSolidHorizontalLine(x+w-8, y+3, 6);
  lcdDrawSolidHorizontalLine(x+w-8, y+5, 6);
  lcdDrawSolidHorizontalLine(x+w-8, y+7, 6);

  return 0;
}
#endif

#if defined(COLORLCD)
/*luadoc
@function lcd.setColor(area, color)

Set a color for specific area

@param area (unsigned number) specific screen area in the list bellow
 * `CUSTOM_COLOR`
 * `TEXT_COLOR`
 * `TEXT_BGCOLOR`
 * `TEXT_INVERTED_COLOR`
 * `TEXT_INVERTED_BGCOLOR`
 * `LINE_COLOR`
 * `SCROLLBOX_COLOR`
 * `MENU_TITLE_BGCOLOR`
 * `MENU_TITLE_COLOR`
 * `MENU_TITLE_DISABLE_COLOR`
 * `HEADER_COLOR`
 * `ALARM_COLOR`
 * `WARNING_COLOR`
 * `TEXT_DISABLE_COLOR`
 * `HEADER_COLOR`
 * `CURVE_AXIS_COLOR`
 * `CURVE_CURSOR_COLOR`
 * `TITLE_BGCOLOR`
 * `TRIM_BGCOLOR`
 * `TRIM_SHADOW_COLOR`
 * `MAINVIEW_PANES_COLOR`
 * `MAINVIEW_GRAPHICS_COLOR`
 * `HEADER_BGCOLOR`
 * `HEADER_ICON_BGCOLOR`
 * `HEADER_CURRENT_BGCOLOR`
 * `OVERLAY_COLOR`

@param color (number) color in 5/6/5 rgb format. The following prefined colors are available
 * `WHITE`
 * `GREY`
 * `LIGHTGREY`
 * `DARKGREY`
 * `BLACK`
 * `YELLOW`
 * `BLUE`
 * `RED`
 * `DARKRED`

@notice Only available on Horus

@status current Introduced in 2.2.0
*/
static int luaLcdSetColor(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  unsigned int index = luaL_checkunsigned(L, 1) >> 16;
  unsigned int color = luaL_checkunsigned(L, 2);
  lcdColorTable[index] = color;
  return 0;
}

/*luadoc
@function lcd.RGB(r, g, b)

Returns a 5/6/5 rgb color code, that can be used with lcd.setColor

@param r (integer) a number between 0x00 and 0xff that expresses te amount of red in the color

@param g (integer) a number between 0x00 and 0xff that expresses te amount of green in the color

@param b (integer) a number between 0x00 and 0xff that expresses te amount of blue in the color

@retval number (integer) rgb color expressed in 5/6/5 format

@notice Only available on Horus

@status current Introduced in 2.2.0
*/
static int luaRGB(lua_State *L)
{
  if (!luaLcdAllowed) return 0;
  int r = luaL_checkinteger(L, 1);
  int g = luaL_checkinteger(L, 2);
  int b = luaL_checkinteger(L, 3);
  lua_pushinteger(L, RGB(r, g, b));
  return 1;
}
#endif

const luaL_Reg lcdLib[] = {
  { "refresh", luaLcdRefresh },
  { "clear", luaLcdClear },
  { "drawPoint", luaLcdDrawPoint },
  { "drawLine", luaLcdDrawLine },
  { "drawRectangle", luaLcdDrawRectangle },
  { "drawFilledRectangle", luaLcdDrawFilledRectangle },
  { "drawText", luaLcdDrawText },
  { "drawTimer", luaLcdDrawTimer },
  { "drawNumber", luaLcdDrawNumber },
  { "drawChannel", luaLcdDrawChannel },
  { "drawSwitch", luaLcdDrawSwitch },
  { "drawSource", luaLcdDrawSource },
  { "drawGauge", luaLcdDrawGauge },
#if defined(COLORLCD)
  { "drawBitmap", luaLcdDrawBitmap },
  { "setColor", luaLcdSetColor },
  { "RGB", luaRGB },
#elif LCD_DEPTH > 1
  { "getLastPos", luaLcdGetLastPos },
  { "drawPixmap", luaLcdDrawPixmap },
  { "drawScreenTitle", luaLcdDrawScreenTitle },
  { "drawCombobox", luaLcdDrawCombobox },
#else
  { "getLastPos", luaLcdGetLastPos },
#endif
  { NULL, NULL }  /* sentinel */
};
