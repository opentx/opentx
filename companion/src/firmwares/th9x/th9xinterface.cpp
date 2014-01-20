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
#include "th9xinterface.h"
#include "th9xeeprom.h"
#include "th9xsimulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

#define FILE_GENERAL   0
#define FILE_MODEL(n) (1+n)

Th9xInterface::Th9xInterface():
EEPROMInterface(BOARD_STOCK),
efile(new EFile())
{
}

Th9xInterface::~Th9xInterface()
{
  delete efile;
}

const char * Th9xInterface::getName()
{
  return "Th9x";
}

const int Th9xInterface::getEEpromSize()
{
  QSettings settings("companion", "companion");
  QString avrMCU = settings.value("mcu", QString("m64")).toString();
  if (avrMCU==QString("m128")) {
    return 2*EESIZE_STOCK;
  }
  return EESIZE_STOCK;
}

const int Th9xInterface::getMaxModels()
{
  return 16;
}

bool Th9xInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  return false;
}

bool Th9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying th9x import... ";

  if (size != getEEpromSize()) {
    std::cout << "wrong size\n";
    return false;
  }

  if (!efile->EeFsOpen(eeprom, size, BOARD_STOCK)) {
    std::cout << "wrong file system\n";
    return false;
  }
    
  efile->openRd(FILE_GENERAL);
  Th9xGeneral th9xGeneral;

  if (efile->readRlc2((uint8_t*)&th9xGeneral, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  std::cout << "version " << (unsigned int)th9xGeneral.myVers << " ";

  switch(th9xGeneral.myVers) {
    case 6:
      break;
    default:
      std::cout << "not th9x\n";
      return false;
  }

  efile->openRd(FILE_GENERAL);
  int len = efile->readRlc2((uint8_t*)&th9xGeneral, sizeof(Th9xGeneral));
  if (len != sizeof(Th9xGeneral)) {
    std::cout << "not th9x\n";
    return false;
  }
  radioData.generalSettings = th9xGeneral;
  
  for (int i=0; i<getMaxModels(); i++) {
    Th9xModelData th9xModel;
    efile->openRd(FILE_MODEL(i));
    if (!efile->readRlc2((uint8_t*)&th9xModel, sizeof(Th9xModelData))) {
      radioData.models[i].clear();
    }
    else {
      radioData.models[i] = th9xModel;
    } 
  }

  std::cout << "ok\n";
  return true;
}

bool Th9xInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  return false;
}

int Th9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  std::cout << "NO!\n";
  // TODO a warning

  return 0;
}

int Th9xInterface::getSize(ModelData &model)
{
  return 0;
}

int Th9xInterface::getSize(GeneralSettings &settings)
{
  return 0;
}

int Th9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case Mixes:
      return TH9X_MAX_MIXERS;
    case OwnerName:
      return 0;
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
      return TH9X_MAX_SWITCHES;
    case Outputs:
      return 8;
    case ExtraChannels:
      return 4;
    case OffsetWeight:
      return 125;      
    case Simulation:
      return 1;
    case HasContrast:  
    case HasInputFilter:
      return 1;
    case CSFunc:
      return 13;
    case SlowScale:
      return 1;
    case SlowRange:
      return 10;
    case LCDWidth:
      return 128;
    case GetThrSwitch:
      return DSW_THR;
    default:
      return 0;
  }
}

int Th9xInterface::isAvailable(Protocol proto, int port)
{
  switch (proto) {
    case PPM:
    case SILV_A:
    case SILV_B:
    case SILV_C:
    case CTP1009:
      return 1;
    default:
      return 0;
  }
}


SimulatorInterface * Th9xInterface::getSimulator()
{
  return new Th9xSimulator(this);
}
