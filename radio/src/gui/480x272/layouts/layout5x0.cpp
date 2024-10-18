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

const uint8_t LBM_LAYOUT_5x0[] = {
#include "mask_layout5x0.lbm"
};

const ZoneOption OPTIONS_LAYOUT_5x0[] = {
  { NULL, ZoneOption::Bool }
};

const Zone ZONES_LAYOUT_5x0[5] = {
  /* X,Y,W,H */
  
  { 10, 10, 84, 252 },  
  { 104, 10, 272, 55 },
  { 104, 75, 272, 121 },
  { 104, 206, 272, 55},
  { 386, 10, 84, 252 }
};

class Layout5x0: public Layout
{
  public:
    Layout5x0(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    virtual void create()
    {
      Layout::create();
    }

    virtual unsigned int getZonesCount() const
    {
      return DIM(ZONES_LAYOUT_5x0);
    }

    virtual Zone getZone(unsigned int index) const
    {
      return ZONES_LAYOUT_5x0[index];
    }

    virtual void refresh();
};

void Layout5x0::refresh()
{
  theme->drawBackground();
  Layout::refresh();
}

BaseLayoutFactory<Layout5x0> layout5x0("Layout5x0", LBM_LAYOUT_5x0, OPTIONS_LAYOUT_5x0);

