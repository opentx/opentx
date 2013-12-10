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
#ifndef open9x_interface_h
#define open9x_interface_h

#include "eeprominterface.h"

class EFile;

class Open9xInterface : public EEPROMInterface
{
  public:

    Open9xInterface(BoardEnum board);

    virtual ~Open9xInterface();

    virtual const char * getName();

    virtual const int getEEpromSize();

    virtual const int getMaxModels();

    virtual bool load(RadioData &, uint8_t *eeprom, int size);

    virtual bool loadBackup(RadioData &, uint8_t *eeprom, int esize, int index);
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc);

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint32_t variant=0, uint8_t version=0);

    virtual int getSize(ModelData &);

    virtual int getSize(GeneralSettings &);
    
    virtual int getCapability(const Capability);
    
    virtual int isAvailable(Protocol proto, int port=0);
    
    virtual SimulatorInterface * getSimulator();

  protected:

    bool checkVersion(unsigned int version);

    bool checkVariant(unsigned int version, unsigned int variant);

    template <class T>
    bool loadModel(ModelData &model, uint8_t *data, int index, unsigned int stickMode=0);

    template <class T>
    bool loadModelVariant(unsigned int index, ModelData &model, uint8_t *data, unsigned int version, unsigned int variant);

    bool loadModel(uint8_t version, ModelData &model, uint8_t *data, int index, unsigned int variant, unsigned int stickMode=0);

    template <class T>
    bool saveModel(unsigned int index, ModelData &model);

    template <class T>
    bool saveModelVariant(unsigned int index, ModelData &model, unsigned int version, unsigned int variant);

    template <class T>
    bool loadGeneral(GeneralSettings &settings, unsigned int version);

    template <class T>
    bool saveGeneral(GeneralSettings &settings, BoardEnum board, uint32_t version, uint32_t variant);

    EFile *efile;

};

class Open9xFirmware: public FirmwareInfo {
  public:
    Open9xFirmware(const QString & id, const QString & name, EEPROMInterface * eepromInterface, const QString & url = QString(), const QString & stamp = QString(), const QString & rnurl = QString(), bool voice = false):
      FirmwareInfo(id, name, eepromInterface, url, stamp, rnurl, voice)
    {
      addLanguage("en");
      addLanguage("fr");
      addLanguage("it");
      addLanguage("de");
      addLanguage("se");
      addLanguage("cz");
      addLanguage("es");
      addLanguage("pt");

      addTTSLanguage("en");
      addTTSLanguage("fr");
      addTTSLanguage("it");
      addTTSLanguage("de");
      addTTSLanguage("se");
      addTTSLanguage("cz");
      addTTSLanguage("sk");
      addTTSLanguage("pt");
      addTTSLanguage("es");
    }
    
    virtual unsigned int getEepromVersion(unsigned int revision) {
      switch(this->eepromInterface->getBoard()) {
        case BOARD_SKY9X:
          if (revision == 0)
            return 212;
          if (revision >= 1217)
            return 212;
          if (revision >= 1174)
            return 211;
          if (revision >= 1031)
            return 210;
          if (revision >= 791)
            return 209;
          if (revision >= 641)
            return 208;
          break;
        case BOARD_GRUVIN9X:
          if (revision == 0)
            return 211;
          if (revision >= 1217)
            return 211;
          if (revision >= 1174)
            return 210;
          if (revision >= 791)
            return 209;
          if (revision >= 641)
            return 208;
          if (revision >= 547)
            return 207;
          break;
        default:
          if (revision == 0)
            return 211;
          if (revision >= 1217)
            return 211;
          if (revision >= 1174)
            return 210;
          if (revision >= 791)
            return 209;
          if (revision >= 641)
            return 208;
          break;
      }
      if (revision >= 321)
        return 205;
      else if (revision >= 217)
        return 204;
      else if (revision >= 184)
        return 203;
      else
        return 202;
    }
};

void RegisterOpen9xFirmwares();

#endif
