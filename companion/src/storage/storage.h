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

#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <QString>
#include <QDebug>
#include "radiodata.h"

class StorageFormat
{
  public:
    StorageFormat(const QString & filename, uint8_t version=0):
      filename(filename),
      version(version)
    {
    }
    
    virtual bool load(RadioData & radioData) = 0;
    // TODO virtual bool write(const RadioData & radioData) = 0;

    QString error() {
      return _error;
    }
    
    QString warning() {
      return _warning;
    }
    
  protected:
    void setError(const QString & error)
    {
      qDebug() << "error:" << error;
      _error = error;
    }
    
    void setWarning(const QString & warning)
    {
      if (!warning.isEmpty())
        qDebug() << "warning:" << warning;
      _warning = warning;
    }
    
    QString filename;
    uint8_t version;
    QString _error;
    QString _warning;
};

class StorageFactory
{
  public:
    StorageFactory()
    {
    }
    virtual const char * name() = 0;
    virtual StorageFormat * instance(const QString & filename) = 0;
};

template <class T>
class DefaultStorageFactory : public StorageFactory
{
  public:
    DefaultStorageFactory(const char * name):
      StorageFactory(),
      _name(name)
    {
    }
    
    virtual const char * name()
    {
      return _name;
    }
    
    virtual StorageFormat * instance(const QString & filename)
    {
      return new T(filename);
    }
    
    const char * _name;
};

extern QList<StorageFactory *> registeredStorageFactories; // TODO needed?

void registerStorageFactories();

class Storage : public StorageFormat
{
  public:
    Storage(const QString & filename):
      StorageFormat(filename)
    {
    }
    
    virtual bool load(RadioData & radioData);
};

#if 0
unsigned long LoadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index);
unsigned long LoadEeprom(RadioData &radioData, const uint8_t *eeprom, int size);
unsigned long LoadEepromXml(RadioData &radioData, QDomDocument &doc);
#endif

bool convertEEprom(const QString & sourceEEprom, const QString & destinationEEprom, const QString & firmware);

#endif // _STORAGE_H_
