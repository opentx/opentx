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

#include "opentx.h"

const BitmapBuffer * Theme::asterisk = NULL;
const BitmapBuffer * Theme::question = NULL;
const BitmapBuffer * Theme::busy = NULL;

std::list<Theme *> & getRegisteredThemes()
{
  static std::list<Theme *> themes;
  return themes;
}

void registerTheme(Theme * theme)
{
  TRACE("register theme %s", theme->getName());
  getRegisteredThemes().push_back(theme);
}

void Theme::init() const
{
  memset(&g_eeGeneral.themeData, 0, sizeof(Theme::PersistentData));
  if (options) {
    int i = 0;
    for (const ZoneOption * option = options; option->name; option++, i++) {
      // TODO compiler bug? The CPU freezes ... g_eeGeneral.themeData.options[i] = &option->deflt;
      memcpy(&g_eeGeneral.themeData.options[i], &option->deflt, sizeof(ZoneOptionValue));
    }
  }
}

void Theme::load() const
{
  TRACE("Theme::load");
  if (!asterisk) asterisk = BitmapBuffer::load(getThemePath("asterisk.bmp"));
  if (!question) question = BitmapBuffer::load(getThemePath("question.bmp"));
  if (!busy) busy = BitmapBuffer::load(getThemePath("busy.bmp"));
}

ZoneOptionValue * Theme::getOptionValue(unsigned int index) const
{
  return &g_eeGeneral.themeData.options[index];
}

const char * Theme::getFilePath(const char * filename) const
{
  static char path[_MAX_LFN+1] = THEMES_PATH "/";
  strcpy(path + sizeof(THEMES_PATH), getName());
  int len = sizeof(THEMES_PATH) + strlen(path + sizeof(THEMES_PATH));
  path[len] = '/';
  strcpy(path+len+1, filename);
  return path;
}

void Theme::drawThumb(uint16_t x, uint16_t y, uint32_t flags)
{
  #define THUMB_WIDTH   51
  #define THUMB_HEIGHT  31
  if (!thumb) {
    thumb = BitmapBuffer::load(getFilePath("thumb.bmp"));
  }
  lcd->drawBitmap(x, y, thumb);
  if (flags == LINE_COLOR) {
    lcdDrawFilledRect(x, y, THUMB_WIDTH, THUMB_HEIGHT, SOLID, OVERLAY_COLOR | OPACITY(10));
  }
}

void Theme::drawBackground() const
{
  lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, TEXT_BGCOLOR);
}

void Theme::drawMessageBox(const char * title, const char * text, const char * action, uint32_t type) const
{

}

Theme * getTheme(const char * name)
{
  std::list<Theme *>::const_iterator it = getRegisteredThemes().cbegin();
  for (; it != getRegisteredThemes().cend(); ++it) {
    if (!strcmp(name, (*it)->getName())) {
      return (*it);
    }
  }
  return NULL;
}

void loadTheme(Theme * new_theme)
{
  TRACE("load theme %s", new_theme->getName());
  theme = new_theme;
  theme->load();
}

void loadTheme()
{
  char name[sizeof(g_eeGeneral.themeName)+1];
  memset(name, 0, sizeof(name));
  strncpy(name, g_eeGeneral.themeName, sizeof(g_eeGeneral.themeName));
  Theme * new_theme = getTheme(name);
  if (new_theme) {
    loadTheme(new_theme);
  }
  else {
    theme->load();
  }
}
