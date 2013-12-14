/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef xml_interface_h
#define xml_interface_h

#include "eeprominterface.h"
#include <QTextStream>

class XmlInterface
{
  public:

    XmlInterface(QTextStream & stream);

    bool load(RadioData &);
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc);

    bool save(RadioData &radioData);

  protected:

    QTextStream & stream;


};

#endif
