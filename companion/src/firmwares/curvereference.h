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

#ifndef CURVEREFERENCE_H
#define CURVEREFERENCE_H

#include <QtCore>

class ModelData;

class CurveReference {
  Q_DECLARE_TR_FUNCTIONS(CurveReference)

  public:
    enum CurveRefType {
      CURVE_REF_DIFF,
      CURVE_REF_EXPO,
      CURVE_REF_FUNC,
      CURVE_REF_CUSTOM
    };

    CurveReference() { clear(); }

    CurveReference(CurveRefType type, int value):
      type(type),
      value(value)
    {
    }

    void clear() { memset(this, 0, sizeof(CurveReference)); }

    CurveRefType type;
    int value;

    QString toString(const ModelData * model = NULL, bool verbose = true) const;
};

#endif // CURVEREFERENCE_H
