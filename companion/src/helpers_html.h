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

#ifndef _HELPERS_HTML_H_
#define _HELPERS_HTML_H_

#include <QString>
#include <QStringList>

QString tdAlign(const QString & s, const QString & align, const QString & color, bool bold);
QString doTC(const QString & s, const QString & color = "", bool bold = false);
QString doTR(const QString & s, const QString & color = "", bool bold = false);
QString doTL(const QString & s, const QString & color = "", bool bold = false);
QString fv(const QString & name, const QString & value, const QString & color = "green");
QString doTableCell(const QString & s, const unsigned int width = 0, const QString & align = "", const QString & color = "", bool bold = false);
QString doTableRow(const QStringList & strl, const unsigned int width = 0, const QString & align = "", const QString & color = "", bool bold = false);

#endif // _HELPERS_HTML_H_
