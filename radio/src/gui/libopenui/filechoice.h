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

#ifndef _FILECHOICE_H_
#define _FILECHOICE_H_

#include "form.h"
#include <string>

class FileChoice: public FormField {
  public:
    FileChoice(Window * parent, const rect_t & rect, std::string folder, const char * extension, int maxlen, std::function<std::string()> getValue, std::function<void(std::string)> setValue);

#if defined(TRACE_WINDOWS_ENABLED)
    std::string getName() override
    {
      return "FileChoice";
    }
#endif

    void paint(BitmapBuffer * dc) override;

#if defined(HARDWARE_KEYS)
    void onKeyEvent(event_t event) override;
#endif

#if defined(HARDWARE_TOUCH)
    bool onTouchEnd(coord_t x, coord_t y) override;
#endif

  protected:
    std::string folder;
    const char * extension;
    int maxlen;
    std::function<std::string()> getValue;
    std::function<void(std::string)> setValue;

    void openMenu();
};

#endif // _FILECHOICE_H_
