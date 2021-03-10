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

#include "curvedata.h"
#include "radiodata.h"

CurveData::CurveData()
{
  clear();
}

void CurveData::clear(int count)
{
  memset(reinterpret_cast<void *>(this), 0, sizeof(CurveData));
  this->count = count;
}

bool CurveData::isEmpty() const
{
  for (int i = 0; i < count; i++) {
    if (points[i].y != 0) {
      return false;
    }
  }
  return true;
}

QString CurveData::nameToString(const int idx) const
{
  return RadioData::getElementName(tr("CV"), idx + 1, name);
}
