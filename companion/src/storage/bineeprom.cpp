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

#include "bineeprom.h"
#include <QFile>
#include "eeprominterface.h"

// TODO should be RleFormat, RawFormat etc.

bool BinEepromFormat::load(RadioData & radioData)
{
  QFile file(filename);
  
  int size = file.size();
  
  if (!file.open(QFile::ReadOnly)) {
    qDebug() << "Unable to open" << filename << file.errorString();
    return false;
  }
  
  QByteArray eeprom(size, 0);
  int result = file.read((char *)eeprom.data(), size);
  if (result != size) {
    setError(QObject::tr("Error reading %1: %2").arg(filename).arg(file.errorString()));
    return false;
  }

  return extract(radioData, eeprom);
}

bool BinEepromFormat::extract(RadioData & radioData, const QByteArray & eeprom)
{
  std::bitset<NUM_ERRORS> errors;
    
  foreach(EEPROMInterface * eepromInterface, eepromInterfaces) {
    std::bitset<NUM_ERRORS> result((unsigned long long)eepromInterface->load(radioData, (uint8_t *)eeprom.data(), eeprom.size()));
    if (result.test(ALL_OK)) {
      if (errors.test(HAS_WARNINGS)) {
        // TODO ShowEepromWarnings(this, QObject::tr("Warning"), errors.to_ulong());
      }
      return true;
    }
    else {
      errors |= result;
    }
  }
  
  setError(QObject::tr("Invalid EEPROM File %1: %2").arg(filename).arg(errors.to_ulong()));
  return false;
}
