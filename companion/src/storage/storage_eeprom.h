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

#ifndef storage_eeprom_h
#define storage_eeprom_h

#include <QDomDocument>
#include <QString>
#include "eeprominterface.h"

unsigned long LoadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index);
unsigned long LoadEeprom(RadioData &radioData, const uint8_t *eeprom, int size);
unsigned long LoadEepromXml(RadioData &radioData, QDomDocument &doc);
bool convertEEprom(const QString &sourceEEprom, const QString &destinationEEprom, const QString &firmware);

#endif
