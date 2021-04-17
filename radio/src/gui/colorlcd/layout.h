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

#pragma once

#include <list>
#include "widgets_container.h"
#include "lcd.h"
#include "topbar.h"

#define MAX_LAYOUT_ZONES               10
#define MAX_LAYOUT_OPTIONS             10
constexpr coord_t TRIM_LINE_WIDTH = 8;
constexpr coord_t TRIM_SQUARE_SIZE = 17;
constexpr coord_t MAIN_ZONE_BORDER = 10;

class LayoutFactory;
class ViewMainDecoration;

#define LAYOUT_COMMON_OPTIONS \
  { STR_TOP_BAR, ZoneOption::Bool },         \
  { STR_FLIGHT_MODE, ZoneOption::Bool },     \
  { STR_SLIDERS, ZoneOption::Bool },         \
  { STR_TRIMS, ZoneOption::Bool },           \
  { STR_MIRROR, ZoneOption::Bool }

#define LAYOUT_OPTIONS_END \
  { nullptr, ZoneOption::Bool }

typedef WidgetsContainer<MAX_LAYOUT_ZONES, MAX_LAYOUT_OPTIONS> LayoutBase;

class Layout: public LayoutBase
{
  friend class LayoutFactory;

  public:

    // Common 'ZoneOptionValue's among all layouts
    enum {
      OPTION_TOPBAR = 0,
      OPTION_FM,
      OPTION_SLIDERS,
      OPTION_TRIMS,
      OPTION_MIRRORED,

      OPTION_LAST_DEFAULT=OPTION_MIRRORED
    };
  
    Layout(const LayoutFactory * factory, PersistentData * persistentData);

#if defined(DEBUG_WINDOWS)
    std::string getName() const override
    {
      return "Layout";
    }

    void paint(BitmapBuffer * dc) override;
#endif

    void create() override;
  
    inline const LayoutFactory * getFactory() const
    {
      return factory;
    }

    void checkEvents() override;
  
    bool hasTopbar() const {
      return getOptionValue(OPTION_TOPBAR)->boolValue;
    }

    bool hasFlightMode() const {
      return getOptionValue(OPTION_FM)->boolValue;
    }

    bool hasSliders() const {
      return getOptionValue(OPTION_SLIDERS)->boolValue;
    }

    bool hasTrims() const {
      return getOptionValue(OPTION_TRIMS)->boolValue;
    }

    bool isMirrored() const {
      return getOptionValue(OPTION_MIRRORED)->boolValue;
    }

    // Set decoration visibility
    void setTrimsVisible(bool visible);
    void setSlidersVisible(bool visible);
    void setFlightModeVisible(bool visible);

    // Re-calculate positions
    void adjustDecoration();

    // Updates settings for trims, sliders, pots, etc...
    void decorate();

  protected:
    const LayoutFactory * factory  = nullptr;
    ViewMainDecoration* decoration = nullptr;

    // Decoration settings bitmask to detect updates
    uint8_t  decorationSettings = 0;

    // Last time we refreshed the window
    uint32_t lastRefresh = 0;
  
    // Get the available space for widgets
    rect_t getMainZone() const;
};

void registerLayout(const LayoutFactory * factory);

class LayoutFactory
{
  public:
    LayoutFactory(const char * id, const char * name):
      id(id),
      name(name)
    {
      registerLayout(this);
    }
    const char * getId() const { return id; }
    const char * getName() const { return name; }
    virtual void drawThumb(BitmapBuffer * dc, uint16_t x, uint16_t y, LcdFlags flags) const = 0;
    virtual const ZoneOption * getOptions() const = 0;
    virtual Layout * create(Layout::PersistentData * persistentData) const = 0;
    virtual Layout * load(Layout::PersistentData * persistentData) const = 0;

  protected:
    const char * id;
    const char * name;
};

template<class T>
class BaseLayoutFactory: public LayoutFactory
{
  public:
    BaseLayoutFactory(const char * id, const char * name, const uint8_t * bitmap, const ZoneOption * options):
      LayoutFactory(id, name),
      bitmap(bitmap),
      options(options)
    {
    }

    void drawThumb(BitmapBuffer * dc, uint16_t x, uint16_t y, uint32_t flags) const override
    {
      dc->drawBitmapPattern(x, y, bitmap, flags);
    }

    const ZoneOption * getOptions() const override
    {
      return options;
    }

    Layout * create(Layout::PersistentData * persistentData) const override
    {
      Layout * layout = new T(this, persistentData);
      if (layout) {
        layout->create();
      }
      return layout;
    }

    Layout * load(Layout::PersistentData * persistentData) const override
    {
      Layout * layout = new T(this, persistentData);
      if (layout) {
        layout->load();
      }
      return layout;
    }

  protected:
    const uint8_t * bitmap;
    const ZoneOption * options;
};

Layout * loadLayout(const char * name, Layout::PersistentData * persistentData);

// intented for new models
void loadDefaultLayout();

// intended for existing models
void loadCustomScreens();

// delete all custom screens from memory
void deleteCustomScreens();

Layout* createCustomScreen(const LayoutFactory* factory, unsigned customScreenIndex);

// Remove custom screen from the model
void disposeCustomScreen(unsigned idx);

std::list<const LayoutFactory *> & getRegisteredLayouts();
