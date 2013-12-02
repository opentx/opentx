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
#include "er9xinterface.h"
#include "er9xeeprom.h"
#include "er9xsimulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

#define FILE_GENERAL   0
#define FILE_MODEL(n) (1+n)

Er9xInterface::Er9xInterface():
EEPROMInterface(BOARD_STOCK),
efile(new EFile())
{
}

Er9xInterface::~Er9xInterface()
{
  delete efile;
}

const char * Er9xInterface::getName()
{
  return "Er9x";
}

const int Er9xInterface::getEEpromSize()
{
  QSettings settings("companion9x", "companion9x");
  QString avrMCU = settings.value("mcu", QString("m64")).toString();
  if (avrMCU==QString("m128")) {
    return 2*EESIZE_STOCK;
  }
  return EESIZE_STOCK;
}

const int Er9xInterface::getMaxModels()
{
  return 16;
}

inline void applyStickModeToModel(Er9xModelData & model, unsigned int mode)
{
  for (int i=0; i<2; i++) {
    int stick = applyStickMode(i+1, mode) - 1;
    {
      int tmp = model.trim[i];
      model.trim[i] = model.trim[stick];
      model.trim[stick] = tmp;
    }
    {
      Er9xExpoData tmp = model.expoData[i];
      model.expoData[i] = model.expoData[stick];
      model.expoData[stick] = tmp;
    }
  }
  for (int i=0; i<ER9X_MAX_MIXERS; i++)
    model.mixData[i].srcRaw = applyStickMode(model.mixData[i].srcRaw, mode);
  for (int i=0; i<ER9X_NUM_CSW; i++) {
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

bool Er9xInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  std::cout << "trying er9x xml import... ";

  Er9xGeneral er9xGeneral;
  memset(&er9xGeneral,0,sizeof(er9xGeneral));
  if(!loadGeneralDataXML(&doc, &er9xGeneral)) {
    return false;
  } else {
    radioData.generalSettings=er9xGeneral;
    std::cout << "version " << (unsigned int)er9xGeneral.myVers << " ";
  }
  for(int i=0; i<getMaxModels(); i++)
  {
    Er9xModelData er9xModel;
    memset(&er9xModel,0,sizeof(er9xModel));
    if(loadModelDataXML(&doc, &er9xModel, i)) {
      applyStickModeToModel(er9xModel, radioData.generalSettings.stickMode+1);
      radioData.models[i] = er9xModel;
    }
  }
  std::cout << "ok\n";
  return true;
}

bool Er9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying er9x import... ";

  if (size != getEEpromSize()) {
    std::cout << "wrong size\n";
    return false;
  }

  if (!efile->EeFsOpen(eeprom, size, BOARD_STOCK)) {
    std::cout << "wrong file system\n";
    return false;
  }
    
  efile->openRd(FILE_GENERAL);
  Er9xGeneral er9xGeneral;

  if (efile->readRlc1((uint8_t*)&er9xGeneral, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  std::cout << "version " << (unsigned int)er9xGeneral.myVers << " ";

  switch(er9xGeneral.myVers) {
    case 3:
      std::cout << "(old gruvin9x) ";
    case 4:
//    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
      break;
    default:
      std::cout << "not er9x\n";
      return false;
  }

  efile->openRd(FILE_GENERAL);
  if (!efile->readRlc1((uint8_t*)&er9xGeneral, sizeof(Er9xGeneral))) {
    std::cout << "ko\n";
    return false;
  }
  radioData.generalSettings = er9xGeneral;
  
  for (int i=0; i<getMaxModels(); i++) {
    Er9xModelData er9xModel;
    efile->openRd(FILE_MODEL(i));
    if (!efile->readRlc1((uint8_t*)&er9xModel, sizeof(Er9xModelData))) {
      radioData.models[i].clear();
    }
    else {
      applyStickModeToModel(er9xModel, radioData.generalSettings.stickMode+1);
      radioData.models[i] = er9xModel;
    } 
  }

  std::cout << "ok\n";
  return true;
}

bool Er9xInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  return false;
}

int Er9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  EEPROMWarnings.clear();

  efile->EeFsCreate(eeprom, getEEpromSize(), BOARD_STOCK);

  Er9xGeneral er9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc1(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&er9xGeneral, sizeof(Er9xGeneral));
  if(sz != sizeof(Er9xGeneral)) {
    return 0;
  }

  for (int i=0; i<getMaxModels(); i++) {
    if (!radioData.models[i].isempty()) {
      Er9xModelData er9xModel(radioData.models[i]);
      applyStickModeToModel(er9xModel, radioData.generalSettings.stickMode+1);
      sz = efile->writeRlc1(FILE_MODEL(i), FILE_TYP_MODEL, (uint8_t*)&er9xModel, sizeof(Er9xModelData));
      if(sz != sizeof(Er9xModelData)) {
        return 0;
      }
    }
  }

  return getEEpromSize();
}

int Er9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[2*EESIZE_STOCK];
  efile->EeFsCreate(tmp, getEEpromSize(), BOARD_STOCK);

  Er9xModelData er9xModel(model);
  int sz = efile->writeRlc1(0, FILE_TYP_MODEL, (uint8_t*)&er9xModel, sizeof(Er9xModelData));
  if(sz != sizeof(Er9xModelData)) {
     return -1;
  }
  return efile->size(0);
}

int Er9xInterface::getSize(GeneralSettings &settings)
{
  uint8_t tmp[2*EESIZE_STOCK];
  efile->EeFsCreate(tmp, getEEpromSize(), BOARD_STOCK);
  
  Er9xGeneral er9xGeneral(settings);
  int sz = efile->writeRlc1(0, FILE_TYP_GENERAL, (uint8_t*)&er9xGeneral, sizeof(Er9xGeneral));
  if(sz != sizeof(Er9xGeneral)) {
    return -1;
  }
  return efile->size(0);
}

int Er9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case FlightPhases:
        return 4;
    case Mixes:
      return ER9X_MAX_MIXERS;
    case NumCurves5:
      return ER9X_MAX_CURVE5;
    case NumCurves9:
      return ER9X_MAX_CURVE5;      
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
    case OffsetWeight:
      return 125;
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
    case Timers:
      return 1;
    case Pots:
      return 3;
    case Switches:
      return 7;
    case SwitchesPositions:
      return 9;
    case CustomFunctions:
      return 0;
    case CustomSwitches:
      return 12;
    case CustomAndSwitches:
        return 5;
    case GvarsNum:
        return 7;
    case GvarsOfsNum:
      return 5;
    case CSFunc:
      return 13;
    case Outputs:
      return 16;
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
    case TimerTriggerB:
      return 1;
    case TelemetryMaxMultiplier:
      return 2;
    case MaxVolume:
      return 7;
    case SlowScale:
      return 1;
    case SlowRange:
      return 15;
    case LCDWidth:
      return 128;
    case VoicesAsNumbers:
    case TelemetryInternalAlarm:
    case HasPPMSim:
    case HasCrossTrims:
    case HasStickScroll:
    case HasAltitudeSel:
    case HasContrast:
    case InstantTrimSW:
    case HasVolume:
    case HasBlInvert:
    case ModelVoice:
    case DiffMixers:
    case HasNegCurves:
    case HasFixOffset:
    case Gvars:
    case GvarsHaveSources:
    case GvarsAsSources:
    case GvarsAsWeight:
     return 1;
    case GetThrSwitch:
      return DSW_THR;     
    default:
      return 0;
  }
}

int Er9xInterface::isAvailable(Protocol prot, int port)
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

SimulatorInterface * Er9xInterface::getSimulator()
{
  return new Er9xSimulator(this);
}



void Er9xInterface::appendTextElement(QDomDocument * qdoc, QDomElement * pe, QString name, QString value)
{
    QDomElement e = qdoc->createElement(name);
    QDomText t = qdoc->createTextNode(name);
    t.setNodeValue(value);
    e.appendChild(t);
    pe->appendChild(e);
}

void Er9xInterface::appendNumberElement(QDomDocument * qdoc, QDomElement * pe,QString name, int value, bool forceZeroWrite)
{
  if(value || forceZeroWrite) {
    QDomElement e = qdoc->createElement(name);
    QDomText t = qdoc->createTextNode(name);
    t.setNodeValue(QString("%1").arg(value));
    e.appendChild(t);
    pe->appendChild(e);
  }
}

void Er9xInterface::appendCDATAElement(QDomDocument * qdoc, QDomElement * pe,QString name, const char * data, int size)
{
  QDomElement e = qdoc->createElement(name);
  QDomCDATASection t = qdoc->createCDATASection(name);
  t.setData(QByteArray(data, size).toBase64());
  e.appendChild(t);
  pe->appendChild(e);
}

QDomElement Er9xInterface::getGeneralDataXML(QDomDocument * qdoc, Er9xGeneral * tgen)
{
  QDomElement gd = qdoc->createElement("GENERAL_DATA");
  appendNumberElement(qdoc, &gd, "Version", tgen->myVers, true); // have to write value here
  appendTextElement(qdoc, &gd, "Owner", QString::fromAscii(tgen->ownerName,sizeof(tgen->ownerName)).trimmed());
  appendCDATAElement(qdoc, &gd, "Data", (const char *)tgen,sizeof(Er9xGeneral));
  return gd;
}

QDomElement Er9xInterface::getModelDataXML(QDomDocument * qdoc, Er9xModelData * tmod, int modelNum, int mdver)
{
  QDomElement md = qdoc->createElement("MODEL_DATA");
  md.setAttribute("number", modelNum);
  appendNumberElement(qdoc, &md, "Version", mdver, true); // have to write value here
  appendTextElement(qdoc, &md, "Name", QString::fromAscii(tmod->name,sizeof(tmod->name)).trimmed());
  appendCDATAElement(qdoc, &md, "Data", (const char *)tmod,sizeof(Er9xModelData));
  return md;
}

bool Er9xInterface::loadGeneralDataXML(QDomDocument * qdoc, Er9xGeneral * tgen)
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
      memcpy(tgen, data, sizeof(Er9xGeneral));
      break;
    }
    n = n.nextSibling();
  }
  //check version?
  return true;
}

bool Er9xInterface::loadModelDataXML(QDomDocument * qdoc, Er9xModelData * tmod, int modelNum)
{
  //look for MODEL_DATA with modelNum attribute.
  //if modelNum = -1 then just pick the first one
  QDomNodeList ndl = qdoc->elementsByTagName("MODEL_DATA");

  //cycle through nodes to find correct model number
  QDomNode k = ndl.at(0);
  if(modelNum>=0) {
    while(!k.isNull()) {
      int a = k.toElement().attribute("number").toInt();
      if(a==modelNum)
        break;
      k = k.nextSibling();
    }
  }

  if(k.isNull()) // couldn't find
    return false;


  //load cdata into tgen
  QDomNode n = k.toElement().elementsByTagName("Data").at(0).firstChild();// get all children in Data
  while (!n.isNull()) {
    if (n.isCDATASection()) {
      QString ds = n.toCDATASection().data();
      QByteArray ba = QByteArray::fromBase64(ds.toAscii());
      const char * data = ba.data();
      memcpy(tmod, data, sizeof(Er9xModelData));
      break;
    }
    n = n.nextSibling();
  }
  //check version?
  return true;
}
