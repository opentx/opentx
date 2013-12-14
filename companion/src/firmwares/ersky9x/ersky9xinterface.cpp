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

#include <iostream>
#include <QMessageBox>
#include "ersky9xinterface.h"
#include "ersky9xeeprom.h"
#include "ersky9xsimulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Ersky9xInterface::Ersky9xInterface():
EEPROMInterface(BOARD_SKY9X),
efile(new EFile())
{
}

Ersky9xInterface::~Ersky9xInterface()
{
  delete efile;
}

const char * Ersky9xInterface::getName()
{
  return "Ersky9x";
}

const int Ersky9xInterface::getEEpromSize()
{
  return EESIZE_SKY9X;
}

const int Ersky9xInterface::getMaxModels()
{
  return 20;
}

inline void applyStickModeToModel(Ersky9xModelData_v10 & model, unsigned int mode)
{
  for (int i=0; i<2; i++) {
    int stick = applyStickMode(i+1, mode) - 1;
    {
      int tmp = model.trim[i];
      model.trim[i] = model.trim[stick];
      model.trim[stick] = tmp;
    }
    {
      Ersky9xExpoData tmp = model.expoData[i];
      model.expoData[i] = model.expoData[stick];
      model.expoData[stick] = tmp;
    }
  }
  for (int i=0; i<ERSKY9X_MAX_MIXERS_V10; i++)
    model.mixData[i].srcRaw = applyStickMode(model.mixData[i].srcRaw, mode);
  for (int i=0; i<ERSKY9X_NUM_CSW_V10; i++) {
    switch (getCSFunctionFamily(model.customSw[i].func)) {
      case CS_FAMILY_VCOMP:
        model.customSw[i].v2 = applyStickMode(model.customSw[i].v2, mode);
        // no break
      case CS_FAMILY_VOFS:
        model.customSw[i].v1 = applyStickMode(model.customSw[i].v1, mode);
        break;
      default:
        break;
    }
  }
  model.swashCollectiveSource = applyStickMode(model.swashCollectiveSource, mode);
}

inline void applyStickModeToModel(Ersky9xModelData_v11 & model, unsigned int mode)
{
  for (int i=0; i<2; i++) {
    int stick = applyStickMode(i+1, mode) - 1;
    {
      int tmp = model.trim[i];
      model.trim[i] = model.trim[stick];
      model.trim[stick] = tmp;
    }
    {
      Ersky9xExpoData tmp = model.expoData[i];
      model.expoData[i] = model.expoData[stick];
      model.expoData[stick] = tmp;
    }
  }
  for (int i=0; i<ERSKY9X_MAX_MIXERS_V11; i++)
    model.mixData[i].srcRaw = applyStickMode(model.mixData[i].srcRaw, mode);
  for (int i=0; i<ERSKY9X_NUM_CSW_V11; i++) {
    switch (getCSFunctionFamily(model.customSw[i].func)) {
      case CS_FAMILY_VCOMP:
        model.customSw[i].v2 = applyStickMode(model.customSw[i].v2, mode);
        // no break
      case CS_FAMILY_VOFS:
        model.customSw[i].v1 = applyStickMode(model.customSw[i].v1, mode);
        break;
      default:
        break;
    }
  }
  model.swashCollectiveSource = applyStickMode(model.swashCollectiveSource, mode);
}

bool Ersky9xInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  std::cout << "trying ersky9x xml import... ";

  Ersky9xGeneral ersky9xGeneral;
  memset(&ersky9xGeneral,0,sizeof(ersky9xGeneral));
  if(!loadGeneralDataXML(&doc, &ersky9xGeneral)) {
    return false;
  }
  else {
    radioData.generalSettings=ersky9xGeneral;
    std::cout << "version " << (unsigned int)ersky9xGeneral.myVers << " ";
  }
  for(int i=0; i<getMaxModels(); i++) {
    if (ersky9xGeneral.myVers == 10) {
      if (!loadModelDataXML<Ersky9xModelData_v10>(&doc, &radioData.models[i], i, radioData.generalSettings.stickMode+1)) {
        std::cout << "ko\n";
        return false;
      }
    }
    else {
      if (!loadModelDataXML<Ersky9xModelData_v11>(&doc, &radioData.models[i], i, radioData.generalSettings.stickMode+1)) {
        std::cout << "ko\n";
        return false;
      }
    }
  }
  std::cout << "ok\n";
  return true;
}

bool Ersky9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying ersky9x import... ";

  if (size != EESIZE_SKY9X) {
    std::cout << "wrong size\n";
    return false;
  }

  if (!efile->EeFsOpen(eeprom, size, BOARD_SKY9X)) {
    std::cout << "wrong file system\n";
    return false;
  }
    
  efile->openRd(FILE_GENERAL);
  Ersky9xGeneral ersky9xGeneral;

  if (efile->readRlc2((uint8_t*)&ersky9xGeneral, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  std::cout << "version " << (unsigned int)ersky9xGeneral.myVers << " ";

  switch(ersky9xGeneral.myVers) {
    case 10:
      break;
    case 11:
      break;
    default:
      std::cout << "not ersky9x\n";
      return false;
  }
  efile->openRd(FILE_GENERAL);
  if (!efile->readRlc2((uint8_t*)&ersky9xGeneral, sizeof(Ersky9xGeneral))) {
    std::cout << "ko\n";
    return false;
  }
  radioData.generalSettings = ersky9xGeneral;
  
  for (int i=0; i<getMaxModels(); i++) {
    uint8_t buffer[4096];
    uint size;
    memset(buffer,0,sizeof(buffer));
    efile->openRd(FILE_MODEL(i));
    
//    if (!efile->readRlc2((uint8_t*)&ersky9xModel, sizeof(Ersky9xModelData))) {
    size=efile->readRlc2(buffer, 4096);
    if (!size) {
      radioData.models[i].clear();
    }
    else {
      if (size<720) {
        Ersky9xModelData_v10 ersky9xModel;
        memcpy(&ersky9xModel, buffer, sizeof(ersky9xModel));
        applyStickModeToModel(ersky9xModel, radioData.generalSettings.stickMode+1);
        radioData.models[i] = ersky9xModel;
      } else {
        Ersky9xModelData_v11 ersky9xModel;
        memcpy(&ersky9xModel, buffer, sizeof(ersky9xModel));
        applyStickModeToModel(ersky9xModel, radioData.generalSettings.stickMode+1);
        radioData.models[i] = ersky9xModel;
      }
    } 
  }

  std::cout << "ok\n";
  return true;
}

bool Ersky9xInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  return false;
}

int Ersky9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  EEPROMWarnings.clear();

  efile->EeFsCreate(eeprom, EESIZE_SKY9X, BOARD_SKY9X);

  Ersky9xGeneral ersky9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&ersky9xGeneral, sizeof(Ersky9xGeneral));
  if(sz != sizeof(Ersky9xGeneral)) {
    return 0;
  }

  for (int i=0; i<getMaxModels(); i++) {
    if (!radioData.models[i].isempty()) {
      Ersky9xModelData_v11 ersky9xModel(radioData.models[i]);
      applyStickModeToModel(ersky9xModel, radioData.generalSettings.stickMode+1);
      sz = efile->writeRlc2(FILE_MODEL(i), FILE_TYP_MODEL, (uint8_t*)&ersky9xModel, sizeof(Ersky9xModelData_v11));
      if(sz != sizeof(Ersky9xModelData_v11)) {
        return 0;
      }
    }
  }

  if (!EEPROMWarnings.isEmpty())
    QMessageBox::warning(NULL,
        QObject::tr("Warning"),
        QObject::tr("EEPROM saved with these warnings:") + "\n- " + EEPROMWarnings.remove(EEPROMWarnings.length()-1, 1).replace("\n", "\n- "));

  return EESIZE_SKY9X;
}

int Ersky9xInterface::getSize(ModelData &model)
{
  return 0;
}

int Ersky9xInterface::getSize(GeneralSettings &settings)
{
  return 0;
}

int Ersky9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case Mixes:
      return ERSKY9X_MAX_MIXERS_V11;
    case NumCurves5:
      return ERSKY9X_MAX_CURVE5;
    case NumCurves9:
      return ERSKY9X_MAX_CURVE9;
    case MixFmTrim:
      return 1;      
    case PPMExtCtrl:
      return 1;
    case ModelTrainerEnable:
      return 1;
    case Timer2ThrTrig:
      return 1;
    case TrainerSwitch:
      return 1;
    case BandgapMeasure:
      return 1;
    case PotScrolling:
      return 1;
    case SoundMod:
      return 1;
    case SoundPitch:
      return 1;
    case Haptic:
      return 1;
    case OwnerName:
      return 10;
    case HasInputFilter:
      return 0;
    case ModelVoice:
      return 1;
    case Timers:
      return 2;
    case Pots:
      return 3;
    case GvarsNum:
      return 7;
    case GvarsOfsNum:
      return 5;
    case Switches:
      return 7;
    case SwitchesPositions:
      return 9;
    case CustomFunctions:
      return 0;
    case CustomSwitches:
      return ERSKY9X_NUM_CSW_V11;
    case CustomAndSwitches:
      return 1;
    case CSFunc:
      return 13;
    case Outputs:
      return ERSKY9X_NUM_CHNOUT_V11;
    case ExtraChannels:
      return 0;
    case Simulation:
      return 1;
    case gsSwitchMask:
      return 1;
    case BLonStickMove:
      return 1;
    case Telemetry:
      return TM_HASTELEMETRY|TM_HASWSHH;
    case TelemetryUnits:
      return 1;
    case OptrexDisplay:
      return 1;
    case TimerTriggerB:
      return 2;
    case HasAltitudeSel:
    case HasCurrentCalibration:
    case HasVolume:
    case HasBrightness:
    case HasContrast:
      return 1;
    case OffsetWeight:
      return 125;
    case MaxVolume:
      return 23;
    case Gvars:
    case GvarsHaveSources:
    case GvarsAsSources:
    case GvarsAsWeight:
      return 1;
    case InstantTrimSW:
      return 1;
    case TelemetryMaxMultiplier:
      return 2;
    case LCDWidth:
      return 128;
    case GetThrSwitch:
      return DSW_THR;      
    default:
      return 0;
  }
}

int Ersky9xInterface::isAvailable(Protocol prot, int port)
{
  switch (prot) {
    case PPM:
    case DSM2:
    case PXX_DJT:
    case PPM16:
      return 1;
    default:
      return 0;
  }
}

SimulatorInterface * Ersky9xInterface::getSimulator()
{
  return new Ersky9xSimulator(this);
}



void Ersky9xInterface::appendTextElement(QDomDocument * qdoc, QDomElement * pe, QString name, QString value)
{
    QDomElement e = qdoc->createElement(name);
    QDomText t = qdoc->createTextNode(name);
    t.setNodeValue(value);
    e.appendChild(t);
    pe->appendChild(e);
}

void Ersky9xInterface::appendNumberElement(QDomDocument * qdoc, QDomElement * pe,QString name, int value, bool forceZeroWrite)
{
  if(value || forceZeroWrite) {
    QDomElement e = qdoc->createElement(name);
    QDomText t = qdoc->createTextNode(name);
    t.setNodeValue(QString("%1").arg(value));
    e.appendChild(t);
    pe->appendChild(e);
  }
}

void Ersky9xInterface::appendCDATAElement(QDomDocument * qdoc, QDomElement * pe,QString name, const char * data, int size)
{
  QDomElement e = qdoc->createElement(name);
  QDomCDATASection t = qdoc->createCDATASection(name);
  t.setData(QByteArray(data, size).toBase64());
  e.appendChild(t);
  pe->appendChild(e);
}

QDomElement Ersky9xInterface::getGeneralDataXML(QDomDocument * qdoc, Ersky9xGeneral * tgen)
{
  QDomElement gd = qdoc->createElement("GENERAL_DATA");
  appendNumberElement(qdoc, &gd, "Version", tgen->myVers, true); // have to write value here
  appendTextElement(qdoc, &gd, "Owner", QString::fromAscii(tgen->ownerName,sizeof(tgen->ownerName)).trimmed());
  appendCDATAElement(qdoc, &gd, "Data", (const char *)tgen,sizeof(Ersky9xGeneral));
  return gd;
}

QDomElement Ersky9xInterface::getModelDataXML(QDomDocument * qdoc, Ersky9xModelData_v11 * tmod, int modelNum, int mdver)
{
  QDomElement md = qdoc->createElement("MODEL_DATA");
  md.setAttribute("number", modelNum);
  appendNumberElement(qdoc, &md, "Version", mdver, true); // have to write value here
  appendTextElement(qdoc, &md, "Name", QString::fromAscii(tmod->name,sizeof(tmod->name)).trimmed());
  appendCDATAElement(qdoc, &md, "Data", (const char *)tmod,sizeof(Ersky9xModelData_v11));
  return md;
}

bool Ersky9xInterface::loadGeneralDataXML(QDomDocument * qdoc, Ersky9xGeneral * tgen)
{
  //look for "GENERAL_DATA" tag
  QDomElement gde = qdoc->elementsByTagName("GENERAL_DATA").at(0).toElement();

  if(gde.isNull()) // couldn't find
    return false;

  //load cdata into tgen
  QDomNode n = gde.elementsByTagName("Data").at(0).firstChild();// get all children in Data
  while (!n.isNull()) {
    if (n.isCDATASection()) {
      QString ds = n.toCDATASection().data();
      QByteArray ba = QByteArray::fromBase64(ds.toAscii());
      const char * data = ba.data();
      memcpy(tgen, data, std::min((unsigned int)ba.size(), (unsigned int)sizeof(Ersky9xGeneral)));
      break;
    }
    n = n.nextSibling();
  }
  //check version?
  return true;
}

template <class T>
bool Ersky9xInterface::loadModelDataXML(QDomDocument * qdoc, ModelData *model, int modelNum, int stickMode)
{
  T ersky9xModel;
  memset(&ersky9xModel, 0, sizeof(ersky9xModel));

  //look for MODEL_DATA with modelNum attribute.
  //if modelNum = -1 then just pick the first one
  QDomNodeList ndl = qdoc->elementsByTagName("MODEL_DATA");

  //cycle through nodes to find correct model number
  QDomNode k = ndl.at(0);
  if (modelNum>=0) {
    while(!k.isNull()) {
      int a = k.toElement().attribute("number").toInt();
      if(a==modelNum)
        break;
      k = k.nextSibling();
    }
  }

  if (k.isNull()) // couldn't find
    return false;

  //load cdata into tgen
  QDomNode n = k.toElement().elementsByTagName("Data").at(0).firstChild();// get all children in Data
  while (!n.isNull()) {
    if (n.isCDATASection()) {
      QString ds = n.toCDATASection().data();
      QByteArray ba = QByteArray::fromBase64(ds.toAscii());
      const char * data = ba.data();
      memcpy(&ersky9xModel, data, std::min(ba.size(), (int)sizeof(ersky9xModel)));
      break;
    }
    n = n.nextSibling();
  }

  applyStickModeToModel(ersky9xModel, stickMode);
  *model = ersky9xModel;
  return true;
}
