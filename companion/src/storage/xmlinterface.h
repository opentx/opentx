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

#ifndef _XMLINTERFACE_H_
#define _XMLINTERFACE_H_

#include "eeprominterface.h"
#include <QTextStream>
#include <QDomDocument>

class XmlInterface
{
  public:

    XmlInterface(QTextStream & stream);

    bool load(RadioData &);
    
    virtual bool loadxml(RadioData & radioData, QDomDocument & doc);

    bool save(RadioData &radioData);

  protected:

    QTextStream & stream;


};

#endif // _XMLINTERFACE_H_
