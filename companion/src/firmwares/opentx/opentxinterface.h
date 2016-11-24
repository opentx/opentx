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

#ifndef _OPENTXINTERFACE_H_
#define _OPENTXINTERFACE_H_

#include "eeprominterface.h"

class RleFile;

class OpenTxEepromInterface : public EEPROMInterface
{
  public:

    OpenTxEepromInterface(BoardEnum board);

    virtual ~OpenTxEepromInterface();

    virtual const int getEEpromSize();

    virtual const int getMaxModels();

    virtual unsigned long load(RadioData &, const uint8_t *eeprom, int size);

    virtual unsigned long loadBackup(RadioData &, uint8_t *eeprom, int esize, int index);

    virtual unsigned long loadxml(RadioData & radioData, QDomDocument & doc);
    
    virtual bool loadRadioSettings(GeneralSettings & model, const QByteArray & data);
    
    virtual bool loadModel(ModelData & model, const QByteArray & data);
    
    virtual int save(uint8_t * eeprom, RadioData & radioData, uint8_t version=0, uint32_t variant=0);

    virtual int getSize(const ModelData &);

    virtual int getSize(const GeneralSettings &);

  protected:

    const char * getName();

    EepromLoadErrors checkVersion(unsigned int version);

    bool checkVariant(unsigned int version, unsigned int variant);
    
    bool loadModel(uint8_t version, ModelData &model, uint8_t *data, int index, unsigned int variant);

    template <class T>
    bool saveModel(unsigned int index, ModelData & model, uint8_t version, uint32_t variant);

    bool loadRadioSettings(GeneralSettings & settings, uint8_t version);

    template <class T>
    bool saveRadioSettings(GeneralSettings & settings, BoardEnum board, uint8_t version, uint32_t variant);

    RleFile *efile;

};

class OpenTxFirmware: public Firmware {
  public:
    OpenTxFirmware(const QString & id, OpenTxFirmware * parent):
      Firmware(parent, id, parent->getName(), parent->getBoard(), parent->eepromInterface)
    {
    }

    OpenTxFirmware(const QString & id, const QString & name, const BoardEnum board):
      Firmware(id, name, board, new OpenTxEepromInterface(board))
    {
      addLanguage("en");
      addLanguage("cz");
      addLanguage("de");
      addLanguage("es");
      addLanguage("fr");
      addLanguage("it");
      addLanguage("nl");
      addLanguage("pl");
      addLanguage("pt");
      addLanguage("se");

      addTTSLanguage("en");
      addTTSLanguage("cz");
      addTTSLanguage("de");
      addTTSLanguage("es");
      addTTSLanguage("fr");
      addTTSLanguage("hu");
      addTTSLanguage("it");
      addTTSLanguage("nl");
      addTTSLanguage("pl");
      addTTSLanguage("pt");
      addTTSLanguage("se");
      addTTSLanguage("sk");
    }

    virtual Firmware * getFirmwareVariant(const QString & id);

    virtual QString getStampUrl();

    virtual QString getReleaseNotesUrl();

    virtual QString getFirmwareUrl();

    virtual int getCapability(Capability);
    
    virtual Switch getSwitch(unsigned int index);
    
    virtual QTime getMaxTimerStart();

    virtual bool isTelemetrySourceAvailable(int source);

    virtual int isAvailable(PulsesProtocol proto, int port=0);

  protected:

    QString getFirmwareBaseUrl();

};

void registerOpenTxFirmwares();
void unregisterOpenTxFirmwares();

#endif // _OPENTXINTERFACE_H_
