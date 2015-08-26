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

#ifndef NDEBUG
#include <iostream>
#endif

#include "th9xinterface.h"
#include "th9xeeprom.h"
#include "th9xsimulator.h"
#include "file.h"
#include "appdata.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

#define FILE_GENERAL   0
#define FILE_MODEL(n) (1+n)

Th9xInterface::Th9xInterface():
EEPROMInterface(BOARD_STOCK),
efile(new RleFile())
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
  QString avrMCU = g.mcu();
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

bool Th9xInterface::load(RadioData &radioData, const uint8_t *eeprom, int size)
{
  #ifndef NDEBUG
  std::cout << "trying th9x import... ";
  #endif

  if (size != getEEpromSize()) {
    #ifndef NDEBUG
    std::cout << "wrong size\n";
    #endif
    return false;
  }

  if (!efile->EeFsOpen((uint8_t *)eeprom, size, BOARD_STOCK)) {
    #ifndef NDEBUG
    std::cout << "wrong file system\n";
    #endif
    return false;
  }

  efile->openRd(FILE_GENERAL);
  Th9xGeneral th9xGeneral;

  if (efile->readRlc2((uint8_t*)&th9xGeneral, 1) != 1) {
    #ifndef NDEBUG
    std::cout << "no\n";
    #endif
    return false;
  }

  #ifndef NDEBUG
  std::cout << "version " << (unsigned int)th9xGeneral.myVers << " ";
  #endif

  switch(th9xGeneral.myVers) {
    case 6:
      break;
    default:
      #ifndef NDEBUG
      std::cout << "not th9x\n";
      #endif
      return false;
  }

  efile->openRd(FILE_GENERAL);
  int len = efile->readRlc2((uint8_t*)&th9xGeneral, sizeof(Th9xGeneral));
  if (len != sizeof(Th9xGeneral)) {
    #ifndef NDEBUG
    std::cout << "not th9x\n";
    #endif
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

  #ifndef NDEBUG
  std::cout << "ok\n";
  #endif
  return true;
}

bool Th9xInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  return false;
}

int Th9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  #ifndef NDEBUG
  std::cout << "NO!\n";
  #endif
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
