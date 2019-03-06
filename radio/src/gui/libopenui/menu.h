#include <utility>

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

#ifndef _MENU_H_
#define _MENU_H_

#include <vector>
#include <functional>
#include "mainwindow.h"
#include <string>
#include <keys.h>

class Menu;

class MenuWindow: public Window {
  friend class Menu;

  class MenuLine {
    friend class MenuWindow;

    public:
      MenuLine(std::string text, std::function<void()> onPress):
        text(std::move(text)),
        onPress(std::move(onPress))
      {
      }

      MenuLine(MenuLine &) = delete;

      MenuLine(MenuLine &&) = default;

    protected:
      std::string text;
      std::function<void()> onPress;
  };

  public:
    MenuWindow(Menu * parent);

#if defined(TRACE_WINDOWS_ENABLED)
    std::string getName() override
    {
      return "MenuWindow";
    }
#endif

    void addLine(const std::string & text, std::function<void()> onPress)
    {
      lines.emplace_back(text, onPress);
      invalidate();
    }

    void removeLines()
    {
      lines.clear();
      invalidate();
    }

    void select(int index);

    void paint(BitmapBuffer * dc) override;

    void onKeyEvent(event_t event) override;

#if defined(HARDWARE_TOUCH)
    bool onTouchEnd(coord_t x, coord_t y) override;
#endif

  protected:
    std::vector<MenuLine> lines;
#if defined(HARDWARE_TOUCH)
    int selectedIndex = -1;
#else
    int selectedIndex = 0;
#endif
    static constexpr uint8_t maxLines = 7; // TODO NV14 was 8
    static constexpr uint8_t lineHeight = 30; // TODO NV14 was 40
};

class Menu : public Window {
  public:
    Menu() :
      Window(&mainWindow, {0, 0, LCD_W, LCD_H}, TRANSPARENT),
#if !defined(HARDWARE_TOUCH)
      previousFocus(focusWindow),
#endif
      menuWindow(this)
    {
    }

    ~Menu()
    {
      delete toolbar;
    }

    void deleteLater()
    {
      if (previousFocus) {
        previousFocus->setFocus();
      }
      Window::deleteLater();
    }

#if defined(TRACE_WINDOWS_ENABLED)
    std::string getName() override
    {
      return "Menu";
    }
#endif

    void setToolbar(Window * window)
    {
      toolbar = window;
      menuWindow.setLeft(toolbar->right());
      menuWindow.setTop(toolbar->top());
      menuWindow.setHeight(toolbar->height());
    }

    void addLine(const std::string & text, std::function<void()> onPress);

    void removeLines();

    void select(int index)
    {
      menuWindow.select(index);
    }

    void onKeyEvent(event_t event) override;

#if defined(HARDWARE_TOUCH)
    bool onTouchStart(coord_t x, coord_t y) override
    {
      return true;
    }

    bool onTouchEnd(coord_t x, coord_t y) override;

    bool onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY) override;
#endif

  protected:
    Window * previousFocus;
    MenuWindow menuWindow;
    Window * toolbar = nullptr;
    void updatePosition();
};

#endif
