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
#include "bin_allocator.h"
#include "lua/lua_api.h"

#define WIDGET_SCRIPTS_MAX_INSTRUCTIONS    (10000/100)
#define MANUAL_SCRIPTS_MAX_INSTRUCTIONS    (20000/100)
#define LUA_WARNING_INFO_LEN               64

lua_State *lsWidgets = NULL;
extern int custom_lua_atpanic(lua_State *L);

#define LUA_FULLPATH_MAXLEN                (LEN_FILE_PATH_MAX + LEN_SCRIPT_FILENAME + LEN_FILE_EXTENSION_MAX)  // max length (example: /SCRIPTS/THEMES/mytheme.lua)

void exec(int function, int nresults=0)
{
  if (lsWidgets == 0) return;

  if (function) {
    luaSetInstructionsLimit(lsWidgets, WIDGET_SCRIPTS_MAX_INSTRUCTIONS);
    lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, function);
    if (lua_pcall(lsWidgets, 0, nresults, 0) != 0) {
      TRACE("Error in theme  %s", lua_tostring(lsWidgets, -1));
    }
  }
}

ZoneOption * createOptionsArray(int reference)
{
  if (reference == 0) {
    return NULL;
  }

  int count = 0;
  lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, reference);
  for (lua_pushnil(lsWidgets); lua_next(lsWidgets, -2); lua_pop(lsWidgets, 1)) {
    count++;
  }

  ZoneOption * options = (ZoneOption *)malloc(sizeof(ZoneOption) * (count+1));
  if (!options) {
    return NULL;
  }

  lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, reference);
  ZoneOption * option = options;
  for (lua_pushnil(lsWidgets); lua_next(lsWidgets, -2); lua_pop(lsWidgets, 1)) {
    luaL_checktype(lsWidgets, -2, LUA_TNUMBER); // key is number
    luaL_checktype(lsWidgets, -1, LUA_TTABLE); // value is table
    uint8_t field = 0;
    for (lua_pushnil(lsWidgets); lua_next(lsWidgets, -2) && field<5; lua_pop(lsWidgets, 1), field++) {
      switch (field) {
        case 0:
          luaL_checktype(lsWidgets, -2, LUA_TNUMBER); // key is number
          luaL_checktype(lsWidgets, -1, LUA_TSTRING); // value is string
          option->name = lua_tostring(lsWidgets, -1);
          break;
        case 1:
          luaL_checktype(lsWidgets, -2, LUA_TNUMBER); // key is number
          luaL_checktype(lsWidgets, -1, LUA_TNUMBER); // value is number
          option->type = (ZoneOption::Type)lua_tointeger(lsWidgets, -1);
          break;
        case 2:
          luaL_checktype(lsWidgets, -2, LUA_TNUMBER); // key is number
          luaL_checktype(lsWidgets, -1, LUA_TNUMBER); // value is number
          option->deflt.signedValue = lua_tointeger(lsWidgets, -1);
          break;
        case 3:
          luaL_checktype(lsWidgets, -2, LUA_TNUMBER); // key is number
          luaL_checktype(lsWidgets, -1, LUA_TNUMBER); // value is number
          option->min.signedValue = lua_tointeger(lsWidgets, -1);
          break;
        case 4:
          luaL_checktype(lsWidgets, -2, LUA_TNUMBER); // key is number
          luaL_checktype(lsWidgets, -1, LUA_TNUMBER); // value is number
          option->max.signedValue = lua_tointeger(lsWidgets, -1);
          break;
      }
    }
    option++;
  }

  option->name = NULL; // sentinel

  return options;
}

class LuaTheme: public Theme
{
  friend void luaLoadThemeCallback();

  public:
    LuaTheme(const char * name, int options):
      Theme(name, createOptionsArray(options)),
      loadFunction(0),
      drawBackgroundFunction(0),
      drawTopbarBackgroundFunction(0),
      drawAlertBoxFunction(0)
    {
    }

    virtual void load() const
    {
      luaLcdAllowed = true;
      exec(loadFunction);
    }

    virtual void drawBackground() const
    {
      exec(drawBackgroundFunction);
    }

    virtual void drawTopbarBackground(uint8_t icon) const
    {
      exec(drawTopbarBackgroundFunction);
    }

#if 0
    virtual void drawAlertBox(const char * title, const char * text, const char * action) const
    {
      exec(drawAlertBoxFunction);
    }
#endif

  protected:
    int loadFunction;
    int drawBackgroundFunction;
    int drawTopbarBackgroundFunction;
    int drawAlertBoxFunction;
};

void luaLoadThemeCallback()
{
  TRACE("luaLoadThemeCallback()");
  const char * name=NULL;
  int themeOptions=0, loadFunction=0, drawBackgroundFunction=0, drawTopbarBackgroundFunction=0;

  luaL_checktype(lsWidgets, -1, LUA_TTABLE);

  for (lua_pushnil(lsWidgets); lua_next(lsWidgets, -2); lua_pop(lsWidgets, 1)) {
    const char * key = lua_tostring(lsWidgets, -2);
    if (!strcmp(key, "name")) {
      name = luaL_checkstring(lsWidgets, -1);
    }
    else if (!strcmp(key, "options")) {
      themeOptions = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "load")) {
      loadFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "drawBackground")) {
      drawBackgroundFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "drawTopbarBackground")) {
      drawTopbarBackgroundFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
  }

  if (name) {
    LuaTheme * theme = new LuaTheme(name, themeOptions);
    theme->loadFunction = loadFunction;
    theme->drawBackgroundFunction = drawBackgroundFunction;
    theme->drawTopbarBackgroundFunction = drawTopbarBackgroundFunction;
    TRACE("Loaded Lua theme %s", name);
  }
}

class LuaWidget: public Widget
{
  public:
    LuaWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData, int widgetData):
      Widget(factory, zone, persistentData),
      widgetData(widgetData)
    {
    }

    virtual ~LuaWidget()
    {
      luaL_unref(lsWidgets, LUA_REGISTRYINDEX, widgetData);
    }

    virtual void update() const;

    virtual void refresh();

    virtual void background();

  protected:
    int widgetData;
};

void l_pushtableint(const char * key, int value)
{
  lua_pushstring(lsWidgets, key);
  lua_pushinteger(lsWidgets, value);
  lua_settable(lsWidgets, -3);
}

class LuaWidgetFactory: public WidgetFactory
{
  friend void luaLoadWidgetCallback();
  friend class LuaWidget;

  public:
    LuaWidgetFactory(const char * name, int widgetOptions, int createFunction):
      WidgetFactory(name, createOptionsArray(widgetOptions)),
      createFunction(createFunction),
      updateFunction(0),
      refreshFunction(0),
      backgroundFunction(0)
    {
    }

    virtual Widget * create(const Zone & zone, Widget::PersistentData * persistentData, bool init=true) const
    {
      if (lsWidgets == 0) return 0;
      if (init) {
        initPersistentData(persistentData);
      }

      luaSetInstructionsLimit(lsWidgets, WIDGET_SCRIPTS_MAX_INSTRUCTIONS);
      lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, createFunction);

      lua_newtable(lsWidgets);
      l_pushtableint("x", zone.x);
      l_pushtableint("y", zone.y);
      l_pushtableint("w", zone.w);
      l_pushtableint("h", zone.h);

      lua_newtable(lsWidgets);
      int i = 0;
      for (const ZoneOption * option = options; option->name; option++, i++) {
        l_pushtableint(option->name, persistentData->options[i].signedValue);
      }

      if (lua_pcall(lsWidgets, 2, 1, 0) != 0) {
        TRACE("Error in widget %s create() function: %s", getName(), lua_tostring(lsWidgets, -1));
      }
      int widgetData = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      Widget * widget = new LuaWidget(this, zone, persistentData, widgetData);
      return widget;
    }

  protected:
    int createFunction;
    int updateFunction;
    int refreshFunction;
    int backgroundFunction;
};

void LuaWidget::update() const
{
  if (lsWidgets == 0) return;

  luaSetInstructionsLimit(lsWidgets, WIDGET_SCRIPTS_MAX_INSTRUCTIONS);
  LuaWidgetFactory * factory = (LuaWidgetFactory *)this->factory;
  lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, factory->updateFunction);
  lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, widgetData);

  lua_newtable(lsWidgets);
  int i = 0;
  for (const ZoneOption * option = getOptions(); option->name; option++, i++) {
    l_pushtableint(option->name, persistentData->options[i].signedValue);
  }

  if (lua_pcall(lsWidgets, 2, 0, 0) != 0) {
    TRACE("Error in widget %s update() function: %s", factory->getName(), lua_tostring(lsWidgets, -1));
  }
}

void LuaWidget::refresh()
{
  if (lsWidgets == 0) return;

  luaSetInstructionsLimit(lsWidgets, WIDGET_SCRIPTS_MAX_INSTRUCTIONS);
  LuaWidgetFactory * factory = (LuaWidgetFactory *)this->factory;
  lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, factory->refreshFunction);
  lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, widgetData);
  if (lua_pcall(lsWidgets, 1, 0, 0) != 0) {
    TRACE("Error in widget %s refresh() function: %s", factory->getName(), lua_tostring(lsWidgets, -1));
  }
}

void LuaWidget::background()
{
  if (lsWidgets == 0) return;

  luaSetInstructionsLimit(lsWidgets, WIDGET_SCRIPTS_MAX_INSTRUCTIONS);
  LuaWidgetFactory * factory = (LuaWidgetFactory *)this->factory;
  if (factory->backgroundFunction) {
    lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, factory->backgroundFunction);
    lua_rawgeti(lsWidgets, LUA_REGISTRYINDEX, widgetData);
    if (lua_pcall(lsWidgets, 1, 0, 0) != 0) {
      TRACE("Error in widget %s background() function: %s", factory->getName(), lua_tostring(lsWidgets, -1));
    }
  }
}

void luaLoadWidgetCallback()
{
  TRACE("luaLoadWidgetCallback()");
  const char * name=NULL;
  int widgetOptions=0, createFunction=0, updateFunction=0, refreshFunction=0, backgroundFunction=0;

  luaL_checktype(lsWidgets, -1, LUA_TTABLE);

  for (lua_pushnil(lsWidgets); lua_next(lsWidgets, -2); lua_pop(lsWidgets, 1)) {
    const char * key = lua_tostring(lsWidgets, -2);
    if (!strcmp(key, "name")) {
      name = luaL_checkstring(lsWidgets, -1);
    }
    else if (!strcmp(key, "options")) {
      widgetOptions = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "create")) {
      createFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "update")) {
      updateFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "refresh")) {
      refreshFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
    else if (!strcmp(key, "background")) {
      backgroundFunction = luaL_ref(lsWidgets, LUA_REGISTRYINDEX);
      lua_pushnil(lsWidgets);
    }
  }

  if (name && createFunction) {
    LuaWidgetFactory * factory = new LuaWidgetFactory(name, widgetOptions, createFunction);
    factory->updateFunction = updateFunction;
    factory->refreshFunction = refreshFunction;
    factory->backgroundFunction = backgroundFunction;
    TRACE("Loaded Lua widget %s", name);
  }
}

void luaLoadFile(const char * filename, void (*callback)())
{
  if (lsWidgets == NULL || callback == NULL)
    return;

  TRACE("luaLoadFile(%s)", filename);

  luaSetInstructionsLimit(lsWidgets, MANUAL_SCRIPTS_MAX_INSTRUCTIONS);

  PROTECT_LUA() {
    if (luaLoadScriptFileToState(lsWidgets, filename, LUA_SCRIPT_LOAD_MODE) == SCRIPT_OK) {
      if (lua_pcall(lsWidgets, 0, 1, 0) == LUA_OK && lua_istable(lsWidgets, -1)) {
        (*callback)();
      }
      else {
        TRACE("luaLoadFile(%s): Error parsing script: %s", filename, lua_tostring(lsWidgets, -1));
      }
    }
  }
  else {
    // luaDisable();
    lsWidgets = 0;
    return;
  }
  UNPROTECT_LUA();
}

void luaLoadFiles(const char * directory, void (*callback)())
{
  char path[LUA_FULLPATH_MAXLEN+1];
  FILINFO fno;
  DIR dir;

  strcpy(path, directory);
  TRACE("luaLoadFiles() %s", path);

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */

  if (res == FR_OK) {
    int pathlen = strlen(path);
    path[pathlen++] = '/';
    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      uint8_t len = strlen(fno.fname);
      if (len > 0 && fno.fname[0]!='.' && (fno.fattrib & AM_DIR)) {
        strcpy(&path[pathlen], fno.fname);
        strcat(&path[pathlen], "/main.lua");
        if (isFileAvailable(path)) {
          luaLoadFile(path, callback);
        }
      }
    }
  }
  else {
    TRACE("f_opendir(%s) failed, code=%d", path, res);
  }

  f_closedir(&dir);
}

void luaInitThemesAndWidgets()
{
  TRACE("luaInitThemesAndWidgets");

#if defined(USE_BIN_ALLOCATOR)
  lsWidgets = lua_newstate(bin_l_alloc, NULL);   //we use our own allocator!
#else
  lsWidgets = lua_newstate(l_alloc, NULL);   //we use Lua default allocator
#endif
  if (lsWidgets) {
    // install our panic handler
    lua_atpanic(lsWidgets, &custom_lua_atpanic);

    // protect libs and constants registration
    PROTECT_LUA() {
      luaRegisterLibraries(lsWidgets);
    }
    else {
      // if we got panic during registration
      // we disable Lua for this session
      // luaDisable();
      lsWidgets = 0;
    }
    UNPROTECT_LUA();
    TRACE("lsWidgets %p", lsWidgets);
    luaLoadFiles(THEMES_PATH, luaLoadThemeCallback);
    luaLoadFiles(WIDGETS_PATH, luaLoadWidgetCallback);
  }
}
