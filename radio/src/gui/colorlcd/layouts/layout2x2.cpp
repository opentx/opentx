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
#include "sliders.h"
#include "trims.h"

constexpr coord_t border = 10;

const uint8_t LBM_LAYOUT_2x2[] = {
#include "mask_layout2x2.lbm"
};

const ZoneOption OPTIONS_LAYOUT_2x2[] = {
  LAYOUT_COMMON_OPTIONS,
  LAYOUT_OPTIONS_END
};

class Layout2x2: public Layout
{
  public:
    Layout2x2(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    void create() override
    {
      Layout::create();
      persistentData->options[0].value.boolValue = true;
      persistentData->options[1].value.boolValue = true;
      persistentData->options[2].value.boolValue = true;
      persistentData->options[3].value.boolValue = true;
      persistentData->options[4].value.boolValue = false;
      persistentData->options[5].value.boolValue = false;
      decorate();
    }

    unsigned int getZonesCount() const override
    {
      return 4;
    }

    rect_t getZone(unsigned int index) const override
    {
      rect_t zone = getMainZone({border, border, LCD_W - 2 * border, LCD_H - 2 * border});

      zone.w /= 2;
      zone.h /= 2;

      if (index == 1 || index == 3)
        zone.y += zone.h;

      if ((!isMirrored() && index > 1) || (isMirrored() && index < 2))
        zone.x += zone.w;

      return zone;
    }
};

BaseLayoutFactory<Layout2x2> layout2x2("Layout2x2", "2 x 2", LBM_LAYOUT_2x2, OPTIONS_LAYOUT_2x2);
