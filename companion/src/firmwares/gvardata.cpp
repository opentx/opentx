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

#include "gvardata.h"

QString GVarData::unitToString() const
{
  switch (unit) {
    case GVAR_UNIT_NUMBER:
      return QObject::tr("");
    case GVAR_UNIT_PERCENT:
      return QObject::tr("%");
    default:
      return QObject::tr("?");  //  highlight unknown value
  }
}

QString GVarData::precToString() const
{
  switch (prec) {
    case GVAR_PREC_MUL10:
      return QObject::tr("0._");
    case GVAR_PREC_MUL1:
      return QObject::tr("0.0");
    default:
      return QObject::tr("?.?");  //  highlight unknown value
  }
}

int GVarData::multiplierSet()
{
  return (prec == 0 ? 1 : 10);
}

float GVarData::multiplierGet() const
{
  return (prec == 0 ? 1 : 0.1);
}

void GVarData::setMin(float val)
{
  min = (val * multiplierSet()) - GVAR_MIN_VALUE;
}

void GVarData::setMax(float val)
{
  max = GVAR_MAX_VALUE - (val * multiplierSet());
}

int GVarData::getMin() const
{
  return GVAR_MIN_VALUE + min;
}

int GVarData::getMax() const
{
  return GVAR_MAX_VALUE - max;
}

float GVarData::getMinPrec() const
{
  return getMin() * multiplierGet();
}

float GVarData::getMaxPrec() const
{
  return getMax() * multiplierGet();
}
