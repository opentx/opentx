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

#define MAX_THEME_OPTIONS              5

class Theme;
void registerTheme(const Theme * theme);

#define MESSAGEBOX_TYPE_INFO           0
#define MESSAGEBOX_TYPE_QUESTION       1
#define MESSAGEBOX_TYPE_WARNING        2
#define MESSAGEBOX_TYPE_ALERT          4

class Theme
{
  public:
    struct PersistentData {
      ZoneOptionValue options[MAX_THEME_OPTIONS];
    };

    Theme(const char * name, const uint8_t * bitmap, const ZoneOption * options=NULL):
      name(name),
      bitmap(bitmap),
      options(options)
    {
      registerTheme(this);
    }

    inline const char * getName() const
    {
      return name;
    }

    inline const ZoneOption * getOptions() const
    {
      return options;
    }

    void init() const;

    ZoneOptionValue * getOptionValue(unsigned int index) const;

    virtual void drawThumb(uint16_t x, uint16_t y, uint32_t flags) const;

    virtual void load() const = 0;

    virtual void drawBackground() const;

    virtual void drawTopbarBackground(const uint8_t * icon) const = 0;

    virtual void drawMessageBox(const char * title, const char * text, const char * action, uint32_t flags) const;

  protected:
    const char * name;
    const uint8_t * bitmap;
    const ZoneOption * options;
};

extern const Theme * theme;

#define MAX_REGISTERED_THEMES          10
extern unsigned int countRegisteredThemes;
void registerTheme(const Theme * theme);
extern const Theme * registeredThemes[MAX_REGISTERED_THEMES]; // TODO dynamic

const Theme * getTheme(const char * name);
void loadTheme(const Theme * theme);
void loadTheme();

#endif // _THEME_H_
