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

#include "curvereference.h"

#include "helpers.h"     // for Helpers::getAdjustmentString()
#include "radiodata.h"  // for ModelData

QString CurveReference::toString(const ModelData * model, bool verbose) const
{
  if (value == 0) {
    return "----";
  }

  QString ret;
  unsigned idx = abs(value) - 1;

  switch(type) {
    case CURVE_REF_DIFF:
      ret = tr("Diff(%1)").arg(Helpers::getAdjustmentString(value, model));
      break;

    case CURVE_REF_EXPO:
      ret = tr("Expo(%1)").arg(Helpers::getAdjustmentString(value, model));
      break;

    case CURVE_REF_FUNC:
      ret = QString("x>0" "x<0" "|x|" "f>0" "f<0" "|f|").mid(3*(value-1), 3);
      if (verbose)
        ret = tr("Function(%1)").arg(ret);
      break;

    default:
      if (model)
        ret = model->curves[idx].nameToString(idx);
      else
        ret = CurveData().nameToString(idx);
      if (verbose)
        ret = tr("Curve(%1)").arg(ret);
      if (value < 0)
        ret.prepend(CPN_STR_SW_INDICATOR_REV);
      break;
  }

  return ret;
}
