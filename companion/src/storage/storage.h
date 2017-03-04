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

#include "radiodata.h"
#include <QString>
#include <QDebug>

enum StorageType
{
  STORAGE_TYPE_UNKNOWN,
  STORAGE_TYPE_BIN,
  STORAGE_TYPE_HEX,
  STORAGE_TYPE_EEPE,
  STORAGE_TYPE_EEPM,
  STORAGE_TYPE_XML,
  STORAGE_TYPE_SDCARD,
  STORAGE_TYPE_OTX
};

StorageType getStorageType(const QString & filename);

class StorageFormat
{
  public:
    StorageFormat(const QString & filename, uint8_t version=0):
      filename(filename),
      version(version),
      board(Board::BOARD_UNKNOWN)
    {
    }
    
    virtual bool load(RadioData & radioData) = 0;
    virtual bool write(const RadioData & radioData) = 0;

    QString error() {
      return _error;
    }
    
    QString warning() {
      return _warning;
    }

    virtual QString name() = 0;

    virtual Board::Type getBoard()
    {
      return board;
    }

    virtual bool isBoardCompatible(Board::Type board)
    {
      if (this->board == board)
        return true;

      if (IS_TARANIS_X9D(this->board) && IS_TARANIS_X9D(board))
        return true;

      return false;
    }

  protected:
    void setError(const QString & error)
    {
      qDebug() << qPrintable(QString("[%1] error: %2").arg(name()).arg(error));
      _error = error;
    }
    
    void setWarning(const QString & warning)
    {
      qDebug() << qPrintable(QString("[%1] warning: %2").arg(name()).arg(warning));
      _warning = warning;
    }
    
    QString filename;
    uint8_t version;
    QString _error;
    QString _warning;
    Board::Type board;
};

class StorageFactory
{
  public:
    StorageFactory()
    {
    }
    virtual QString name() = 0;
    virtual bool probe(const QString & filename) = 0;
    virtual StorageFormat * instance(const QString & filename) = 0;
};

template <class T>
class DefaultStorageFactory : public StorageFactory
{
  public:
    DefaultStorageFactory(const QString & name):
      StorageFactory(),
      _name(name)
    {
    }
    
    virtual QString name()
    {
      return _name;
    }
    
    virtual bool probe(const QString & filename)
    {
      return filename.toLower().endsWith("." + _name);
    }
    
    virtual StorageFormat * instance(const QString & filename)
    {
      return new T(filename);
    }
    
    QString _name;
};

class Storage : public StorageFormat
{
  public:
    Storage(const QString & filename):
      StorageFormat(filename)
    {
    }
    
    virtual QString name() { return "storage"; };
    
    void setError(const QString & error)
    {
      _error = error;
    }
    
    void setWarning(const QString & warning)
    {
      _warning = warning;
    }
    
    virtual bool load(RadioData & radioData);
    virtual bool write(const RadioData & radioData);
};

void registerStorageFactories();

#if 0
unsigned long LoadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index);
unsigned long LoadEeprom(RadioData &radioData, const uint8_t *eeprom, int size);
unsigned long LoadEepromXml(RadioData &radioData, QDomDocument &doc);
#endif

bool convertEEprom(const QString & sourceEEprom, const QString & destinationEEprom, const QString & firmware);

#endif // _STORAGE_H_
