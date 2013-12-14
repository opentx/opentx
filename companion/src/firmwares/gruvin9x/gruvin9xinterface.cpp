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
#include "gruvin9xinterface.h"
#include "gruvin9xeeprom.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

#define FILE_GENERAL   0
#define FILE_MODEL(n) (1+n)

Gruvin9xInterface::Gruvin9xInterface(BoardEnum board):
EEPROMInterface(board),
efile(new EFile())
{
}

Gruvin9xInterface::~Gruvin9xInterface()
{
  delete efile;
}

const char * Gruvin9xInterface::getName()
{
  if (board == BOARD_STOCK)
    return "Gruvin9x";
  else
    return "Gruvin9x v4";
}

const int Gruvin9xInterface::getEEpromSize()
{
  if (board == BOARD_STOCK) {
    QSettings settings("companion9x", "companion9x");
    QString avrMCU = settings.value("mcu", QString("m64")).toString();
    if (avrMCU==QString("m128")) {
      return EESIZE_STOCK*2;
    }
    return EESIZE_STOCK;
  } else {
    return EESIZE_GRUVIN9X;
  }
}

const int Gruvin9xInterface::getMaxModels()
{
  return 16;
}

template <class T>
void Gruvin9xInterface::loadModel(ModelData &model, unsigned int stickMode, int version)
{
  T _model;
  if ((version == 2 && efile->readRlc2((uint8_t*)&_model, sizeof(T))) ||
      (version == 1 && efile->readRlc1((uint8_t*)&_model, sizeof(T)))) {
    model = _model;
    if (stickMode) {
      applyStickModeToModel(model, stickMode);
    }
  }
  else {
    model.clear();
  }
}

template <class T>
bool Gruvin9xInterface::loadGeneral(GeneralSettings &settings, int version)
{
  T _settings;
  if ((version == 2 && efile->readRlc2((uint8_t*)&_settings, sizeof(T))) ||
      (version == 1 && efile->readRlc1((uint8_t*)&_settings, sizeof(T)))) {
    settings = _settings;
    return true;
  }

  std::cerr << "error when loading general settings";
  return false;
}

bool Gruvin9xInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  return false;
}


bool Gruvin9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying " << getName() << " import... ";

  if (size != this->getEEpromSize()) {
    std::cout << "wrong size\n";
    return false;
  }

  if (!efile->EeFsOpen(eeprom, size, BOARD_STOCK)) {
    std::cout << "wrong file system\n";
    return false;
  }

  efile->openRd(FILE_GENERAL);

  uint8_t version;
  if (efile->readRlc2(&version, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  if (version == 0) {
    efile->openRd(FILE_GENERAL);
    if (efile->readRlc1(&version, 1) != 1) {
      std::cout << "no\n";
      return false;
    }
  }

  std::cout << "version " << (unsigned int)version << " ";

  switch(version) {
    case 5:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
      // subtrims(16bits) + function switches added
    case 106:
      // trims(10bits), no subtrims
      break;
    default:
      std::cout << "not gruvin9x\n";
      return false;
  }

  efile->openRd(FILE_GENERAL);
  if (version == 5) {
    if (!loadGeneral<Gruvin9xGeneral_v103>(radioData.generalSettings, 1))
      return false;
  }
  else if (version < 104) {
    if (!loadGeneral<Gruvin9xGeneral_v103>(radioData.generalSettings))
      return false;
  }
  else if (version <= 106) {
    if (!loadGeneral<Gruvin9xGeneral_v104>(radioData.generalSettings))
      return false;
  }
  else {
    std::cout << "ko\n";
    return false;
  }
  
  for (int i=0; i<getMaxModels(); i++) {
    efile->openRd(FILE_MODEL(i));
    if (version == 5) {
      loadModel<Gruvin9xModelData_v102>(radioData.models[i], radioData.generalSettings.stickMode+1, 1);
    }
    else if (version < 103) {
      loadModel<Gruvin9xModelData_v102>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else if (version == 103) {
      loadModel<Gruvin9xModelData_v103>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else if (version == 105) {
      loadModel<Gruvin9xModelData_v105>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else if (version == 106) {
      loadModel<Gruvin9xModelData_v106>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else {
      std::cout << "ko\n";
      return false;
    }
  }

  std::cout << "ok\n";
  return true;
}

bool Gruvin9xInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  return false;
}

int Gruvin9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  EEPROMWarnings.clear();

  int size = getEEpromSize();

  efile->EeFsCreate(eeprom, size, BOARD_STOCK);

  Gruvin9xGeneral gruvin9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(Gruvin9xGeneral));
  if(sz != sizeof(Gruvin9xGeneral)) {
    return 0;
  }

  for (int i=0; i<getMaxModels(); i++) {
    if (!radioData.models[i].isempty()) {
      ModelData model = radioData.models[i];
      if (1/*version < */) {
        applyStickModeToModel(model, radioData.generalSettings.stickMode+1);
      }
      Gruvin9xModelData gruvin9xModel(model);
      sz = efile->writeRlc2(FILE_MODEL(i), FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
      if(sz != sizeof(Gruvin9xModelData)) {
        return 0;
      }
    }
  }

  return size;
}

int Gruvin9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE_GRUVIN9X];
  efile->EeFsCreate(tmp, EESIZE_GRUVIN9X, BOARD_STOCK);

  Gruvin9xModelData gruvin9xModel(model);
  int sz = efile->writeRlc2(0, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
  if(sz != sizeof(Gruvin9xModelData)) {
     return -1;
  }
  return efile->size(0);
}

int Gruvin9xInterface::getSize(GeneralSettings &settings)
{
  uint8_t tmp[EESIZE_GRUVIN9X];
  efile->EeFsCreate(tmp, EESIZE_GRUVIN9X, BOARD_STOCK);

  Gruvin9xGeneral gruvin9xGeneral(settings);
  int sz = efile->writeRlc1(0, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(gruvin9xGeneral));
  if(sz != sizeof(gruvin9xGeneral)) {
    return -1;
  }
  return efile->size(0);
}

int Gruvin9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case OwnerName:
      return 0;
    case NumCurves5:
      return G9X_MAX_CURVE5;
    case NumCurves9:
      return G9X_MAX_CURVE9;
    case Mixes:
      return G9X_MAX_MIXERS;
    case FlightPhases:
      return G9X_MAX_PHASES;
    case FlightPhasesHaveFades:
      return 1;
    case Timers:
      return 2;
    case Pots:
      return 3;
    case Switches:
      return 7;
    case SwitchesPositions:
      return 9;
    case CustomFunctions:
      return 12;
    case CustomSwitches:
      return 12;
    case CSFunc:
      return 13;
    case Outputs:
      return 16;
    case ExtraChannels:
      return 0;
    case ExtendedTrims:
      return 500;
    case Simulation:
      return true;
    case OffsetWeight:
      return 125;
    case HasExpoCurves:
    case HasContrast:
      return true;           
    case Telemetry:
      return TM_HASTELEMETRY|TM_HASWSHH;
    case TelemetryMaxMultiplier:
      return 1;
    case SlowScale:
      return 2;
    case SlowRange:
      return 15;
    case LCDWidth:
      return 128;      
    case HasSDLogs:
    case Haptic:
      return 1;
    case GetThrSwitch:
      return DSW_THR; 
    default:
      return false;
  }
}

int Gruvin9xInterface::isAvailable(Protocol proto, int port)
{
  switch (proto) {
    case PPM:
    case SILV_A:
    case SILV_B:
    case SILV_C:
    case CTP1009:
    case DSM2:
      return 1;
    default:
      return 0;
  }
}

SimulatorInterface * Gruvin9xInterface::getSimulator()
{
  return NULL;
}
