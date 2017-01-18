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

#include "radiodata.h"
#include "eeprominterface.h"

// TODO here we will move a lot of functions from eeprominterface.cpp when no merge risk

void ModelData::convert(Firmware * before, Firmware * after)
{
  // Here we can add explicit conversions when moving from one board to another
}

void GeneralSettings::convert(Firmware * before, Firmware * after)
{
  // Here we can add explicit conversions when moving from one board to another
}

void RadioData::convert(Firmware * before, Firmware * after)
{
  generalSettings.convert(before, after);
  for (unsigned i=0; i<models.size(); i++) {
    models[i].convert(before, after);
  }
  
  if (categories.size() == 0) {
    categories.push_back(CategoryData(QObject::tr("Models").toStdString().c_str()));
    for (unsigned i=0; i<models.size(); i++) {
      models[i].category = 0;
    }
  }
}