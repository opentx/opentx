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

#include "hexeeprom.h"
#include "eeprominterface.h"
#include "hexinterface.h"
#include <QFile>

bool HexEepromFormat::load(RadioData & radioData)
{
  QFile file(filename);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Unable to open" << filename << file.errorString();
    return false;
  }
  
  QTextStream inputStream(&file);
  // TODO remove EESIZE_MAX
  QByteArray eeprom(EESIZE_MAX, 0);
  int eeprom_size = HexInterface(inputStream).load((uint8_t *)eeprom.data(), EESIZE_MAX);
  if (!eeprom_size) {
    setError(QObject::tr("Invalid EEPROM File %1").arg(filename));
    return false;
  }
  eeprom.resize(eeprom_size);
  
  return extract(radioData, eeprom);
}

bool HexEepromFormat::writeToFile(const uint8_t * eeprom, uint32_t size)
{
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    setError(QObject::tr("Cannot open file %1:\n%2.").arg(filename).arg(file.errorString()));
    return false;
  }
  
  QTextStream outputStream(&file);
  if (!HexInterface(outputStream).save(eeprom, size)) {
    setError(QObject::tr("Error writing file %1:\n%2.").arg(filename).arg(file.errorString()));
    return false;
  }
  
  return true;
}
