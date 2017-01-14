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
#include "eepe.h"
#include "otx.h"
#include "sdcard.h"
#include "firmwareinterface.h"
#include <QFile>

void registerStorageFactory(StorageFactory * factory);

QList<StorageFactory *> registeredStorageFactories;

void registerStorageFactory(StorageFactory * factory)
{
  qDebug() << "register storage" << factory->name();
  registeredStorageFactories.push_back(factory);
}

void registerStorageFactories()
{
  registerStorageFactory(new DefaultStorageFactory<BinEepromFormat>("bin"));
  registerStorageFactory(new DefaultStorageFactory<EepeFormat>("eepe"));
  registerStorageFactory(new DefaultStorageFactory<HexEepromFormat>("hex"));
  registerStorageFactory(new DefaultStorageFactory<OtxFormat>("otx"));
  registerStorageFactory(new DefaultStorageFactory<SdcardFormat>("sdcard"));
}

bool Storage::load(RadioData & radioData)
{
  QFile file(filename);
  if (!file.exists()) {
    setError(QObject::tr("Unable to find file %1!").arg(filename));
    return false;
  }
  
  foreach(StorageFactory * factory, registeredStorageFactories) {
    StorageFormat * format = factory->instance(filename);
    if (format->load(radioData)) {
      setWarning(format->warning());
      return true;
    }
    else {
      setError(format->error());
    }
  }

  return false;
}

#include "eeprominterface.h"

bool convertEEprom(const QString & sourceEEprom, const QString & destinationEEprom, const QString & firmwareFilename)
{
  Firmware * currentFirmware = GetCurrentFirmware();
  FirmwareInterface firmware(firmwareFilename);
  if (!firmware.isValid())
    return false;
  
  uint8_t version = firmware.getEEpromVersion();
  unsigned int variant = firmware.getEEpromVariant();
  
  QSharedPointer<RadioData> radioData = QSharedPointer<RadioData>(new RadioData());
  Storage storage(sourceEEprom);
  if (!storage.load(*radioData))
    return false;
  
  QByteArray eeprom(EESIZE_MAX, 0);
  int size = currentFirmware->saveEEPROM((uint8_t *)eeprom.data(), *radioData, version, variant);
  if (size == 0) {
    return false;
  }
  
  QFile destinationFile(destinationEEprom);
  if (!destinationFile.open(QIODevice::WriteOnly))
    return false;
  
  int result = destinationFile.write(eeprom.constData(), size);
  destinationFile.close();
  return (result == size);
}

#if 0
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
#endif
