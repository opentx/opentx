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

#ifndef _MODEL_TELEMETRY_H
#define _MODEL_TELEMETRY_H

#include "tabsgroup.h"

class ModelTelemetryPage: public PageTab {
  public:
    ModelTelemetryPage();

    virtual void build(Window * window) override
    {
      build(window, -1);
    }

    void checkEvents() override;

  protected:
    Window * window = nullptr;
    void editSensor(Window * window, uint8_t index);
    int lastKnownIndex = 0;
    void build(Window * window, int8_t focusSensorIndex=-1);
    void rebuild(Window * window, int8_t focusSensorIndex=-1);
};

#endif //_MODEL_TELEMETRY_H