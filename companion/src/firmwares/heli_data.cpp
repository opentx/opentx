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

#include "heli_data.h"
#include "compounditemmodels.h"

QString SwashRingData::typeToString() const
{
  return typeToString(type);
}

// static
FieldRange SwashRingData::getValueRange()
{
  FieldRange result;

  result.min = 0;
  result.max = 100;

  return result;
}

// static
FieldRange SwashRingData::getWeightRange()
{
  FieldRange result;

  result.min = -100;
  result.max = 100;

  return result;
}

//  static
QString SwashRingData::typeToString(const int value)
{
  switch(value) {
    case SWASHRING_TYPE_OFF:
      return tr("OFF");
    case SWASHRING_TYPE_120:
      return tr("120");
    case SWASHRING_TYPE_120X:
      return tr("120X");
    case SWASHRING_TYPE_140:
      return tr("140");
    case SWASHRING_TYPE_90:
      return tr("90");
    default:
      return CPN_STR_UNKNOWN_ITEM;
  }
}

//  static
AbstractStaticItemModel * SwashRingData::typeItemModel()
{
  AbstractStaticItemModel * mdl = new AbstractStaticItemModel();
  mdl->setName("swashringdata.type");

  for (int i = 0; i < SWASHRING_TYPE_COUNT; i++) {
    mdl->appendToItemList(typeToString(i), i);
  }

  mdl->loadItemList();
  return mdl;
}
