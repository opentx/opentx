#include <QFile>
#include "firmwareinterface.h"
#include "storage_eeprom.h"


unsigned long LoadEeprom(RadioData &radioData, const uint8_t *eeprom, const int size)
{
  std::bitset<NUM_ERRORS> errors;

  foreach(EEPROMInterface *eepromInterface, eepromInterfaces) {
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

bool convertEEprom(const QString &sourceEEprom, const QString &destinationEEprom, const QString &firmwareFilename)
{
  Firmware *currentFirmware = GetCurrentFirmware();
  FirmwareInterface firmware(firmwareFilename);
  if (!firmware.isValid())
    return false;

  unsigned int version = firmware.getEEpromVersion();
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
  if (!errors.test(ALL_OK)
    || !currentFirmware->saveEEPROM((uint8_t *)eeprom.data(), *radioData, variant, version)) {
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
