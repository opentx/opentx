#include "converteeprom.h"
#include "eeprominterface.h"
#include "firmwareinterface.h"
#include <QFile>

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
  std::bitset<NUM_ERRORS> errors(LoadEeprom(*radioData, (uint8_t *)eeprom.data(), eeprom_size));
  if (!errors.test(NO_ERROR)
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
