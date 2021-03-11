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

#include "datahelpers.h"

QString DataHelpers::getElementName(const QString & prefix, const unsigned int index, const char * name, const bool padding)
{
  QString result = prefix;
  result.append(padding ? QString("%1").arg(index, 2, 10, QChar('0')) : QString("%1").arg(index));
  if (name && QString(name).trimmed().length() > 0)
    result.append(":" + QString(name).trimmed());

  return result;
}
