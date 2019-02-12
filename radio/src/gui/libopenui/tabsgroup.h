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

#ifndef _TABSGROUP_H_
#define _TABSGROUP_H_

#include <vector>
#include "window.h"
#include "button.h"

class TabsGroup;

class PageTab {
  friend class TabsCarousel;
  friend class TabsGroup;

  public:
    PageTab(std::string title, unsigned icon):
      title(std::move(title)),
      icon(icon)
    {
    }

    virtual ~PageTab()
    {
    }

    virtual void build(Window * window) = 0;

    virtual void checkEvents()
    {
    }

    void destroy()
    {
      if (onPageDestroyed) {
        onPageDestroyed();
      }
      delete this;
    }

    void setOnPageDestroyedHandler(std::function<void()> handler)
    {
      onPageDestroyed = std::move(handler);
    }

  protected:
    std::string title;
    unsigned icon;
    std::function<void()> onPageDestroyed;
};

class TabsCarousel: public Window {
  public:
    TabsCarousel(Window * parent, TabsGroup * menu);

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "TabsCarousel";
    }
#endif

    inline void setCurrentIndex(uint8_t index)
    {
      currentIndex = index;
    }

    inline uint8_t getCurrentIndex()
    {
      return currentIndex;
    }

    void updateInnerWidth();

    void paint(BitmapBuffer * dc) override;

#if defined(TOUCH_HARDWARE)
    bool onTouchEnd(coord_t x, coord_t y) override;
#endif

  protected:
    constexpr static uint8_t padding_left = 3;
    TabsGroup * menu;
    uint8_t currentIndex = 0;
};

class TabsGroupHeader: public Window {
    friend class TabsGroup;

  public:
    TabsGroupHeader(TabsGroup * menu, uint8_t icon);

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "TabsGroupHeader";
    }
#endif

    void paint(BitmapBuffer * dc) override;

    void setTitle(const char * value)
    {
      title = value;
    }

  protected:
#if defined(TOUCH_INTERFACE)
    IconButton back;
#endif
    uint8_t icon;
    TabsCarousel carousel;
    const char * title = nullptr;
};

class TabsGroup: public Window {
    friend class TabsCarousel;

  public:
    TabsGroup(uint8_t icon);

    ~TabsGroup();

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "TabsGroup";
    }
#endif

    void addTab(PageTab * page);

    void removeTab(unsigned index);

    void setCurrentTab(unsigned index)
    {
      if (index < tabs.size()) {
        header.carousel.setCurrentIndex(index);
        setCurrentTab(tabs[index]);
      }
    }

    void checkEvents() override;

    bool onKeyEvent(event_t event) override;

    void paint(BitmapBuffer * dc) override;

#if defined(TOUCH_HARDWARE)
    bool onTouchEnd(coord_t x, coord_t y) override;
#endif

  protected:
    TabsGroupHeader header;
    Window body;
    std::vector<PageTab *> tabs;
    PageTab * currentTab = nullptr;
    void setCurrentTab(PageTab * tab);
};

#endif
