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

#ifndef _THEME_H_
#define _THEME_H_

class Theme;

void registerTheme(const Theme * theme);

class Theme
{
  public:
    Theme(const char * name, const uint8_t * bitmap):
      name(name),
      bitmap(bitmap)
    {
      registerTheme(this);
    }

    const char * getName() const
    {
      return name;
    }

    virtual void drawThumb(uint16_t x, uint16_t y, uint32_t flags) const
    {
      extern void lcdDrawBitmap(int x, int y, const uint8_t * bitmap, int offset=0, int height=0, int scale=0);
      lcdDrawBitmap(x, y, bitmap);
    }

    virtual void load() const = 0;

    virtual void drawBackground() const = 0;

    virtual void drawTopbarBackground(const uint8_t * icon) const = 0;

  protected:
    const char * name;
    const uint8_t * bitmap;
};

extern const Theme * theme;

#define MAX_REGISTERED_THEMES          10
extern unsigned int countRegisteredThemes;
extern const Theme * registeredThemes[MAX_REGISTERED_THEMES]; // TODO dynamic

#endif // _THEME_H_
