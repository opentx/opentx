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

#ifndef opentx_interface_h
#define opentx_interface_h

#include "eeprominterface.h"

#define OPENTX_FIRMWARE_DOWNLOADS        "http://downloads-21.open-tx.org/firmware"
#define OPENTX_NIGHT_FIRMWARE_DOWNLOADS  "http://downloads-21.open-tx.org/nightly/firmware"

class RleFile;

class OpenTxEepromInterface : public EEPROMInterface
{
  public:

    OpenTxEepromInterface(BoardEnum board);

    virtual ~OpenTxEepromInterface();

    virtual const int getEEpromSize();

    virtual const int getMaxModels();

    virtual bool load(RadioData &, const uint8_t *eeprom, int size);

    virtual bool loadBackup(RadioData &, uint8_t *eeprom, int esize, int index);
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc);

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint32_t variant=0, uint8_t version=0);

    virtual int getSize(ModelData &);

    virtual int getSize(GeneralSettings &);
    
    virtual int isAvailable(Protocol proto, int port=0);
    
  protected:

    const char * getName();

    bool checkVersion(unsigned int version);

    bool checkVariant(unsigned int version, unsigned int variant);

    template <class T>
    bool loadModel(ModelData &model, uint8_t *data, int index, unsigned int stickMode=0);

    template <class T>
    bool loadModelVariant(unsigned int index, ModelData &model, uint8_t *data, unsigned int version, unsigned int variant);

    bool loadModel(uint8_t version, ModelData &model, uint8_t *data, int index, unsigned int variant, unsigned int stickMode=0);

    template <class T>
    bool saveModel(unsigned int index, ModelData &model, unsigned int version, unsigned int variant);

    template <class T>
    bool loadGeneral(GeneralSettings &settings, unsigned int version);

    template <class T>
    bool saveGeneral(GeneralSettings &settings, BoardEnum board, uint32_t version, uint32_t variant);

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

    virtual int getCapability(const Capability);

    virtual bool isTelemetrySourceAvailable(int source);
    
};

void registerOpenTxFirmwares();

#endif
