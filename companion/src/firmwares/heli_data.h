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

#include "rawsource.h"
#include "datahelpers.h"

#include <QtCore>

class AbstractStaticItemModel;

class SwashRingData {

  Q_DECLARE_TR_FUNCTIONS(SwashRingData)

  public:
    enum
    {
      SWASHRING_TYPE_OFF,
      SWASHRING_TYPE_120,
      SWASHRING_TYPE_120X,
      SWASHRING_TYPE_140,
      SWASHRING_TYPE_90,
      SWASHRING_TYPE_COUNT
    };

    SwashRingData() { clear(); }

    int elevatorWeight;
    int aileronWeight;
    int collectiveWeight;
    unsigned int  type;
    RawSource collectiveSource;
    RawSource aileronSource;
    RawSource elevatorSource;
    unsigned int  value;

    void clear() { memset(reinterpret_cast<void *>(this), 0, sizeof(SwashRingData)); }
    QString typeToString() const;

    static FieldRange getValueRange();
    static FieldRange getWeightRange();
    static QString typeToString(const int value);
    static AbstractStaticItemModel * typeItemModel();
};

