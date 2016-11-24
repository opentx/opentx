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

#include <QFile>
#include "firmwareinterface.h"
#include "storage_eeprom.h"

unsigned long LoadEeprom(RadioData & radioData, const uint8_t * eeprom, const int size)
{
  std::bitset<NUM_ERRORS> errors;

  foreach(EEPROMInterface * eepromInterface, eepromInterfaces) {
    std::bitset<NUM_ERRORS> result((unsigned long long)eepromInterface->load(radioData, eeprom, size));
    if (result.test(ALL_OK)) {
      return result.to_ulong();
    }
    else {
      errors |= result;
    }
  }

  if (errors.none()) {
    errors.set(UNKNOWN_ERROR);
  }
  return errors.to_ulong();
}

unsigned long LoadBackup(RadioData & radioData, uint8_t * eeprom, int size, int index)
{
  std::bitset<NUM_ERRORS> errors;

  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    std::bitset<NUM_ERRORS> result((unsigned long long)eepromInterface->loadBackup(radioData, eeprom, size, index));
    if (result.test(ALL_OK)) {
      return result.to_ulong();
    }
    else {
      errors |= result;
    }
  }

  if (errors.none()) {
    errors.set(UNKNOWN_ERROR);
  }
  return errors.to_ulong();
}


unsigned long LoadEepromXml(RadioData & radioData, QDomDocument & doc)
{
  std::bitset<NUM_ERRORS> errors;

  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
    std::bitset<NUM_ERRORS> result((unsigned long long)eepromInterface->loadxml(radioData, doc));
    if (result.test(ALL_OK)) {
      return result.to_ulong();
    }
    else {
      errors |= result;
    }
  }

  if (errors.none()) {
    errors.set(UNKNOWN_ERROR);
  }
  return errors.to_ulong();
}

bool convertEEprom(const QString & sourceEEprom, const QString & destinationEEprom, const QString & firmwareFilename)
{
  Firmware * currentFirmware = GetCurrentFirmware();
  FirmwareInterface firmware(firmwareFilename);
  if (!firmware.isValid())
    return false;

  uint8_t version = firmware.getEEpromVersion();
  unsigned int variant = firmware.getEEpromVariant();

  QFile sourceFile(sourceEEprom);
  int eeprom_size = sourceFile.size();
  if (!eeprom_size)
    return false;

  if (!sourceFile.open(QIODevice::ReadOnly))
    return false;

  QByteArray eeprom(eeprom_size, 0);
  long result = sourceFile.read(eeprom.data(), eeprom_size);
  sourceFile.close();

  QSharedPointer<RadioData> radioData = QSharedPointer<RadioData>(new RadioData());
  std::bitset<NUM_ERRORS> errors((unsigned long long)LoadEeprom(*radioData, (uint8_t *)eeprom.data(), eeprom_size));
  if (!errors.test(ALL_OK) || !currentFirmware->saveEEPROM((uint8_t *)eeprom.data(), *radioData, version, variant)) {
    return false;
  }

  QFile destinationFile(destinationEEprom);
  if (!destinationFile.open(QIODevice::WriteOnly))
    return false;

  result = destinationFile.write(eeprom.constData(), eeprom_size);
  destinationFile.close();
  if (result != eeprom_size)
    return false;

  return true;
}
