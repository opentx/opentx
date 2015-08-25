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
#ifndef er9x_interface_h
#define er9x_interface_h
#include <QtXml>
#include "eeprominterface.h"
#include "er9xeeprom.h"
class RleFile;

class Er9xInterface : public EEPROMInterface
{
  public:

    Er9xInterface();

    virtual ~Er9xInterface();

    virtual const char * getName();
    
    virtual const int getEEpromSize();

    virtual const int getMaxModels();

    virtual bool load(RadioData &, const uint8_t * eeprom, int size);

    virtual bool loadBackup(RadioData &, uint8_t * eeprom, int esize, int index);
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc);

    virtual int save(uint8_t * eeprom, RadioData & radioData, uint32_t variant=0, uint8_t version=0);

    virtual int getSize(const ModelData &);
    
    virtual int getSize(const GeneralSettings &settings);

    virtual int isAvailable(Protocol proto, int port=0);

  protected:

    RleFile *efile;

  private:
    void appendTextElement(QDomDocument * qdoc, QDomElement * pe, QString name, QString value);
    
    void appendNumberElement(QDomDocument * qdoc, QDomElement * pe,QString name, int value, bool forceZeroWrite = false);
    
    void appendCDATAElement(QDomDocument * qdoc, QDomElement * pe,QString name, const char * data, int size);
    
    QDomElement getGeneralDataXML(QDomDocument * qdoc, Er9xGeneral * tgen);   //parse out data to XML format
    
    QDomElement getModelDataXML(QDomDocument * qdoc, Er9xModelData * tmod, int modelNum, int mdver); //parse out data to XML format
    
    bool loadGeneralDataXML(QDomDocument * qdoc, Er9xGeneral * tgen); // get data from XML
    
    bool loadModelDataXML(QDomDocument * qdoc, Er9xModelData * tmod, int modelNum = -1); // get data from XML

};

#endif
