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

class MultimoduleStatus: public StaticText
{
  public:
    MultimoduleStatus(Window * parent, const rect_t & rect, uint8_t moduleIdx):
      StaticText(parent, rect),
      moduleIdx(moduleIdx)
    {
    }

    void checkEvents() override
    {
      char statusText[MULTIMODULE_STATUS_LEN] = {};
      getMultiModuleStatus(moduleIdx).getStatusString(statusText);
      if (text != statusText) {
        setText(statusText);
        invalidate();
      }
    }

  protected:
    uint8_t moduleIdx;
};

class ModelSetupPage: public PageTab {
  public:
    ModelSetupPage();

    void build(FormWindow * window) override;
};
