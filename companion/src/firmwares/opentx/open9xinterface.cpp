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
#include "open9xinterface.h"
#include "open9xeeprom.h"
#include "open9xGruvin9xeeprom.h"
#include "open9xSky9xeeprom.h"
#include "open9xsimulator.h"
#include "open9xM128simulator.h"
#include "open9xGruvin9xsimulator.h"
#include "open9xSky9xsimulator.h"
#include "opentxTaranisSimulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)

template <typename T, size_t N>
inline
size_t SizeOfArray( T(&)[ N ] )
{
  return N;
}

QStringList o9xservers;

Open9xInterface::Open9xInterface(BoardEnum board):
  EEPROMInterface(board),
  efile(new EFile())
{
  o9xservers.clear();
  o9xservers << "93.51.182.154" << "open9x.9xforums.com";
}

Open9xInterface::~Open9xInterface()
{
  delete efile;
}

const char * Open9xInterface::getName()
{
  switch (board) {
    case BOARD_STOCK:
      return "openTx for 9X board";
    case BOARD_M128:
      return "openTx for M128 / 9X board";
    case BOARD_GRUVIN9X:
      return "openTx for Gruvin9x board / 9X";
    case BOARD_TARANIS:
      return "openTx for FrSky Taranis";
    case BOARD_TARANIS_REV4a:
      return "openTx for FrSky Taranis Rev4a";
    case BOARD_SKY9X:
      return "openTx for Sky9x board / 9X";
    default:
      return "openTx for an unknown board";
  }
}

const int Open9xInterface::getEEpromSize()
{
  switch (board) {
    case BOARD_STOCK:
      return EESIZE_STOCK;
    case BOARD_M128:
      return EESIZE_M128;
    case BOARD_GRUVIN9X:
      return EESIZE_GRUVIN9X;
    case BOARD_SKY9X:
      return EESIZE_SKY9X;
    case BOARD_TARANIS:
      return EESIZE_TARANIS;
    case BOARD_TARANIS_REV4a:
      return EESIZE_TARANIS_REV4a;
    default:
      return 0;
  }
}

const int Open9xInterface::getMaxModels()
{
  if (IS_ARM(board))
    return 60;
  else if (board == BOARD_M128)
    return 30;
  else if (board == BOARD_GRUVIN9X)
    return 30;
  else
    return 16;
}

template <class T>
bool Open9xInterface::loadModel(ModelData &model, uint8_t *data, int index, unsigned int stickMode)
{
  T _model;
  
  if (!data) {
    // load from EEPROM
    efile->openRd(FILE_MODEL(index));
    int sz = efile->readRlc2((uint8_t*)&_model, sizeof(T));
    if (sz) {
      model = _model;
      if (sz < (int)sizeof(T)) {
        std::cout << " size(" << model.name << ") " << sz << " < " << (int)sizeof(T) << " ";
      }
      if (stickMode) {
        applyStickModeToModel(model, stickMode);
      }
    }
    else {
      model.clear();
    }
  }
  else {
    // load from SD Backup, size is stored in index
    if ((unsigned int)index < sizeof(T))
      return false;
    memcpy((uint8_t*)&_model, data, sizeof(T));
    model = _model;
  }

  return true;
}

template <class T>
bool Open9xInterface::loadModelVariant(unsigned int index, ModelData &model, uint8_t *data, unsigned int version, unsigned int variant)
{
  T open9xModel(model, board, version, variant);

  if (!data) {
    // load from EEPROM
    QByteArray eepromData(sizeof(model), 0);  // ModelData should be always bigger than the EEPROM struct
    efile->openRd(FILE_MODEL(index));
    int numbytes = efile->readRlc2((uint8_t *)eepromData.data(), eepromData.size());
    if (numbytes) {
      open9xModel.Import(eepromData);
      // open9xModel.Dump();
      model.used = true;
    }
    else {
      model.clear();
    }
  }
  else {
    // load from SD Backup, size is stored in index
    QByteArray eepromData((char *)data, index);
    open9xModel.Import(eepromData);
  }

  return true;
}

bool Open9xInterface::loadModel(uint8_t version, ModelData &model, uint8_t *data, int index, unsigned int variant, unsigned int stickMode)
{
  if (version == 201) {
    return loadModel<Open9xModelData_v201>(model, data, index, stickMode);
  }
  else if (version == 202) {
    return loadModel<Open9xModelData_v202>(model, data, index, 0 /*no more stick mode messed*/);
  }
  else if (version == 203) {
    return loadModel<Open9xModelData_v203>(model, data, index, 0 /*no more stick mode messed*/);
  }
  else if (version == 204) {
    return loadModel<Open9xModelData_v204>(model, data, index, 0 /*no more stick mode messed*/);
  }
  else if (version == 205) {
    return loadModel<Open9xModelData_v205>(model, data, index, 0 /*no more stick mode messed*/);
  }
  else if (board == BOARD_GRUVIN9X && version == 207) {
    return loadModel<Open9xGruvin9xModelData_v207>(model, data, index, 0 /*no more stick mode messed*/);
  }
  else if (version == 208) {
    if (board == BOARD_GRUVIN9X) {
      return loadModel<Open9xGruvin9xModelData_v208>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else if (board == BOARD_SKY9X) {
      return loadModel<Open9xArmModelData_v208>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else {
      return loadModel<Open9xModelData_v208>(model, data, index, 0 /*no more stick mode messed*/);
    }
  }
  else if (version == 209) {
    if (board == BOARD_GRUVIN9X) {
      return loadModel<Open9xGruvin9xModelData_v209>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else if (board == BOARD_SKY9X) {
      return loadModel<Open9xArmModelData_v209>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else {
      return loadModel<Open9xModelData_v209>(model, data, index, 0 /*no more stick mode messed*/);
    }
  }
  else if (version == 210) {
    if (board == BOARD_GRUVIN9X) {
      return loadModel<Open9xGruvin9xModelData_v210>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else if (board == BOARD_SKY9X) {
      return loadModel<Open9xArmModelData_v210>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else {
      return loadModel<Open9xModelData_v210>(model, data, index, 0 /*no more stick mode messed*/);
    }
  }
  else if (version == 211) {
    if (board == BOARD_GRUVIN9X) {
      return loadModel<Open9xGruvin9xModelData_v211>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else if (board == BOARD_SKY9X) {
      return loadModel<Open9xArmModelData_v211>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else {
      return loadModel<Open9xModelData_v211>(model, data, index, 0 /*no more stick mode messed*/);
    }
  }
  else if (version == 212) {
    if (board == BOARD_SKY9X) {
      return loadModel<Open9xArmModelData_v212>(model, data, index);
    }
    else {
      return loadModelVariant<Open9xModelDataNew>(index, model, data, version, variant);
    }
  }
  else if (version >= 213) {
    return loadModelVariant<Open9xModelDataNew>(index, model, data, version, variant);
  }

  std::cout << " ko\n";
  return false;
}

template <class T>
bool Open9xInterface::loadGeneral(GeneralSettings &settings, unsigned int version)
{
  QByteArray eepromData(sizeof(settings), 0); // GeneralSettings should be always bigger than the EEPROM struct
  T open9xSettings(settings, board, version);
  efile->openRd(FILE_GENERAL);
  int sz = efile->readRlc2((uint8_t *)eepromData.data(), eepromData.size());
  if (sz) {
    open9xSettings.Import(eepromData);
    return checkVariant(settings.version, settings.variant);
  }

  std::cout << " error when loading general settings";
  return false;
}

template <class T>
bool Open9xInterface::saveGeneral(GeneralSettings &settings, BoardEnum board, uint32_t version, uint32_t variant)
{
  T open9xSettings(settings, board, version, variant);
  // open9xSettings.Dump();
  QByteArray eeprom;
  open9xSettings.Export(eeprom);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (const uint8_t*)eeprom.constData(), eeprom.size());
  return (sz == eeprom.size());
}

template <class T>
bool Open9xInterface::saveModel(unsigned int index, ModelData &model)
{
  T open9xModel(model);
  int sz = efile->writeRlc2(FILE_MODEL(index), FILE_TYP_MODEL, (uint8_t*)&open9xModel, sizeof(T));
  return (sz == sizeof(T));
}

template <class T>
bool Open9xInterface::saveModelVariant(unsigned int index, ModelData &model, unsigned int version, unsigned int variant)
{
  T open9xModel(model, board, version, variant);
  // open9xModel.Dump();
  QByteArray eeprom;
  open9xModel.Export(eeprom);
  int sz = efile->writeRlc2(FILE_MODEL(index), FILE_TYP_MODEL, (const uint8_t*)eeprom.constData(), eeprom.size());
  return (sz == eeprom.size());
}

bool Open9xInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  return false;
}

bool Open9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying " << getName() << " import...";

  if (size != getEEpromSize()) {    
    if (size==4096) {
      int notnull=false;
      for (int i=2048; i<4096; i++) {
        if (eeprom[i]!=255) {
          notnull=true;
        }
      }
      if (notnull) {
        std::cout << " wrong size (" << size << ")\n";
        return false;
      } else {
        QMessageBox::warning(NULL,"companion9x", QObject::tr("Your radio probably uses a wrong firmware,\n eeprom size is 4096 but only the first 2048 are used"));
        size=2048;
      }
    } else {
      std::cout << " wrong size (" << size << ")\n";
      return false;
    }
  }

  if (!efile->EeFsOpen(eeprom, size, board)) {
    std::cout << " wrong file system\n";
    return false;
  }

  efile->openRd(FILE_GENERAL);
  
  uint8_t version;
  if (efile->readRlc2(&version, 1) != 1) {
    std::cout << " no\n";
    return false;
  }

  std::cout << " version " << (unsigned int)version;

  if (!checkVersion(version)) {
    std::cout << " not open9x\n";
    return false;
  }

  if (!loadGeneral<Open9xGeneralDataNew>(radioData.generalSettings, version)) {
    std::cout << " ko\n";
    return false;
  }
  
  std::cout << " variant " << radioData.generalSettings.variant;
  for (int i=0; i<getMaxModels(); i++) {
    if (!loadModel(version, radioData.models[i], NULL, i, radioData.generalSettings.variant, radioData.generalSettings.stickMode+1)) {
      std::cout << " ko\n";
      return false;
    }
  }
  std::cout << " ok\n";
  return true;
}

int Open9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  EEPROMWarnings.clear();

  if (!version) {
    switch(board) {
      case BOARD_TARANIS:
      case BOARD_TARANIS_REV4a:
      case BOARD_SKY9X:
        version = 215;
        break;
      case BOARD_GRUVIN9X:
        version = 214;
        break;
      case BOARD_M128:
        version = 215;
        break;
      case BOARD_STOCK:
        version = 213;
        break;
    }
  }

  int size = getEEpromSize();

  efile->EeFsCreate(eeprom, size, board);

  int result = 0;

  if (board == BOARD_M128) {
    variant |= M128_VARIANT;
  }
  
  result = saveGeneral<Open9xGeneralDataNew>(radioData.generalSettings, board, version, variant);

  if (!result)
    return 0;

  for (int i=0; i<getMaxModels(); i++) {
    if (!radioData.models[i].isempty()) {
      result = 0;
      switch(version) {
        case 202:
          result = saveModel<Open9xModelData_v202>(i, radioData.models[i]);
          break;
        case 203:
          result = saveModel<Open9xModelData_v203>(i, radioData.models[i]);
          break;
        case 204:
          result = saveModel<Open9xModelData_v204>(i, radioData.models[i]);
          break;
        case 205:
          result = saveModel<Open9xModelData_v205>(i, radioData.models[i]);
          break;
        case 207:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xGruvin9xModelData_v207>(i, radioData.models[i]);
          break;
        case 208:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xGruvin9xModelData_v208>(i, radioData.models[i]);
          else if (board == BOARD_SKY9X)
            result = saveModel<Open9xArmModelData_v208>(i, radioData.models[i]);
          else
            result = saveModel<Open9xModelData_v208>(i, radioData.models[i]);
          break;
        case 209:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xGruvin9xModelData_v209>(i, radioData.models[i]);
          else if (board == BOARD_SKY9X)
            result = saveModel<Open9xArmModelData_v209>(i, radioData.models[i]);
          else
            result = saveModel<Open9xModelData_v209>(i, radioData.models[i]);
          break;
        case 210:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xGruvin9xModelData_v210>(i, radioData.models[i]);
          else if (board == BOARD_SKY9X)
            result = saveModel<Open9xArmModelData_v210>(i, radioData.models[i]);
          else
            result = saveModel<Open9xModelData_v210>(i, radioData.models[i]);
          break;
        case 211:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xGruvin9xModelData_v211>(i, radioData.models[i]);
          else if (board == BOARD_SKY9X)
            result = saveModel<Open9xArmModelData_v211>(i, radioData.models[i]);
          else
            result = saveModel<Open9xModelData_v211>(i, radioData.models[i]);
          break;
        case 212:
          if (board == BOARD_SKY9X)
            result = saveModel<Open9xArmModelData_v212>(i, radioData.models[i]);
          else
            result = saveModelVariant<Open9xModelDataNew>(i, radioData.models[i], version, variant);
          break;
        default:
          result = saveModelVariant<Open9xModelDataNew>(i, radioData.models[i], version, variant);
          break;
      }
      if (!result)
        return false;
    }
  }

  if (!EEPROMWarnings.isEmpty())
    QMessageBox::warning(NULL,
        QObject::tr("Warning"),
        QObject::tr("EEPROM saved with these warnings:") + "\n- " + EEPROMWarnings.remove(EEPROMWarnings.length()-1, 1).replace("\n", "\n- "));

  return size;
}

int Open9xInterface::getSize(ModelData &model)
{
  if (board == BOARD_SKY9X)
    return 0;

  if (model.isempty())
    return 0;

  // TODO something better
  uint8_t tmp[EESIZE_RLC_MAX];
  efile->EeFsCreate(tmp, EESIZE_RLC_MAX, board);

  Open9xModelDataNew open9xModel(model, board, 255, GetCurrentFirmwareVariant());
  // open9xModel.Dump();

  QByteArray eeprom;
  open9xModel.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_MODEL, (const uint8_t*)eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

int Open9xInterface::getSize(GeneralSettings &settings)
{
  if (board == BOARD_SKY9X)
    return 0;

  uint8_t tmp[EESIZE_RLC_MAX];
  efile->EeFsCreate(tmp, EESIZE_RLC_MAX, board);

  Open9xGeneralDataNew open9xGeneral(settings, board, 255, GetCurrentFirmwareVariant());
  // open9xGeneral.Dump();

  QByteArray eeprom;
  open9xGeneral.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_GENERAL, (const uint8_t*)eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

int Open9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case OwnerName:
      return 0;
    case ModelImage:
      if (IS_TARANIS(board))
        return 1;
      else
        return 0;
    case SimulatorType:
      if (IS_TARANIS(board))
        return 1;
      else
        return 0;
    case HasBeeper:
      if (IS_ARM(board))
        return 0;
      else
        return 1;
    case HasPxxCountry:
      return 1;
    case HasGeneralUnits:
      if (IS_ARM(board))
        return 1;
      else
        return 0;
    case HasNegAndSwitches:
      if (IS_ARM(board))
        return 1;
      else
        return 0;
    case PPMExtCtrl:
      return 1;
    case PPMFrameLength:
      return 40;
    case FlightPhases:
      if (IS_ARM(board))
        return 9;
      else if (board==BOARD_GRUVIN9X)
        return 6;
      else
        return 5;
    case FlightPhasesAreNamed:
    case FlightPhasesHaveFades:
    case Gvars:
    case GvarsInCS:
    case GvarsAsWeight:
    case ExpoIsCurve:
    case HasFAIMode:
      return 1;
    case GvarsAreNamed:
    case GvarsFlightPhases:
      return ((IS_ARM(board)||(board==BOARD_GRUVIN9X)) ? 1 : 0);
    case Mixes:
      return (IS_ARM(board) ? O9X_ARM_MAX_MIXERS : O9X_MAX_MIXERS);
    case OffsetWeight:
      return (IS_ARM(board) ? 500 : 245);
    case Timers:
      return 2;
    case PermTimers:
      if (board == BOARD_GRUVIN9X || board == BOARD_SKY9X || IS_TARANIS(board) )
        return 1;
      else
        return 0;
    case minuteBeep:
        return 1;
    case countdownBeep:
        return 1;
    case Pots:
      return (IS_TARANIS(board) ? 4 : 3);
    case Switches:
      return (IS_TARANIS(board) ? 8 : 7);
    case SwitchesPositions:
      return (IS_TARANIS(board) ? 22 : 9);
    case CustomFunctions:
      if (IS_ARM(board))
        return 32;
      else if (board==BOARD_GRUVIN9X||board==BOARD_M128)
        return 24;
      else
        return 16;
    case CustomSwitches:
      if (IS_ARM(board))
        return 32;
      else if (board==BOARD_GRUVIN9X||board==BOARD_M128)
        return 15;
      else
        return 12;
    case CustomAndSwitches:
      if (IS_ARM(board))
        return getCapability(CustomSwitches);
      else
        return 15/*4bits*/-9/*sw positions*/;
    case CustomSwitchesExt:
      return (IS_ARM(board) ? true : false);
    case RotaryEncoders:
      if (board == BOARD_GRUVIN9X)
        return 2;
      else if (board == BOARD_SKY9X)
        return 1;
      else
        return 0;
    case Outputs:
      return (IS_ARM(board) ? O9X_ARM_NUM_CHNOUT : O9X_NUM_CHNOUT);
    case NumCurvePoints:
      return (IS_ARM(board) ? 512 : 104);
    case VoicesAsNumbers:
      return (IS_ARM(board) ? 0 : 1);
    case VoicesMaxLength:
      return (IS_ARM(board) ? (IS_TARANIS(board) ? 10 :  6) : 0);
    case MultiLangVoice:
      return (IS_ARM(board) ? 1 : 0);
    case SoundPitch:
      return 1;
    case Haptic:
      if (IS_TARANIS(board))
        return 0;
      else
        return 1;
    case ModelTrainerEnable:
      if (IS_TARANIS(board))
        return 1;
      else
        return 0;
    case HasFuncRepeat:
    case HasContrast:
      return 1;      
    case HapticLength:
    case HapticMode:
      if (IS_TARANIS(board))
        return 0;
      else
        return 1;
    case Beeperlen:
      return 1;
    case MaxVolume:
      return (IS_ARM(board) ? 23 : 7);
    case HasSoundMixer:
      return (IS_ARM(board) ? 1 : 0);
    case ExtraChannels:
      return 0;
    case ExtraInputs:
      return 1;
    case HasNegCurves:
      return 1;
    case HasExpoCurves:
      return true;
    case ExpoCurve5:
    case ExpoCurve9:
      return 4;
    case ExtendedTrims:
      return 500;
    case ExtraTrims:
      return 1;
    case Simulation:
      return 1;
    case DSM2Indexes:
      return 1;
    case FSSwitch:
      return 1;
    case HasTTrace:
      return 1;
    case CustomCurves:
      return 1;
    case MixesWithoutExpo:
      return 1;
    case NumCurves:
      return (IS_ARM(board) ? 16 : 8);
    case HasMixerNames:
      return (IS_ARM(board) ? (IS_TARANIS(board) ? 8 : 6) : false);
    case HasExpoNames:
      return (IS_ARM(board) ? (IS_TARANIS(board) ? 8 : 6) : false);
    case HasChNames:
      return (IS_TARANIS(board) ? 1 : 0);
    case HasCvNames:
      return (IS_TARANIS(board) ? 1 : 0);
    case NoTimerDirs:
      return 1;
    case NoThrExpo:
      return 1;
    case Telemetry:
      return TM_HASTELEMETRY|TM_HASOFFSET|TM_HASWSHH;
    case TelemetryBars:
      return 1;
    case TelemetryCSFields:
      if (IS_TARANIS(board)) {
        return 36;
      } else {
        return IS_ARM(board) ? 24 : 16;
      }
    case TelemetryColsCSFields:
      if (IS_TARANIS(board)) {
        return 3;
      } else {
        return 2;
      }
    case NoTelemetryProtocol:
      if (IS_TARANIS(board)) {
        return 1;
      } else {
        return 0;
      }      
    case TelemetryUnits:
      return 0;
    case TelemetryBlades:
      return 1;
    case TelemetryRSSIModel:
      return 1;
    case TelemetryAlarm:
      return 0;
    case TelemetryTimeshift:
      return 1;
    case TelemetryMaxMultiplier:
      return (IS_ARM(board) ? 32 : 8);
    case DiffMixers:
      return 1;
    case PPMCenter:
      return 1;
    case SYMLimits:
      return 1;
    case OptrexDisplay:
      return (board==BOARD_SKY9X ? true : false);
    case HasVario:
      return (IS_TARANIS(board) ? 28 : 31);
    case HasVarioSink:
      return ((board == BOARD_GRUVIN9X || IS_ARM(board)) ? true : false);
    case HasVariants:
      if (IS_TARANIS(board))
        return 0;
      else
        return 1;
    case HasFailsafe:
      if (IS_TARANIS(board)) 
        return 32; // 
      return (board==BOARD_SKY9X ? 16 : 0);
    case NumModules:
      return (IS_ARM(board) ? 2 : 1);
    case HasPPMStart:
      return (IS_ARM(board) ? true : false);
    case HasCurrentCalibration:
      return (board==BOARD_SKY9X ? true : false);
    case HasVolume:
      return (IS_ARM(board) ? true : false);
    case HasBrightness:
      return (IS_ARM(board) ? true : false);
    case PerModelTimers:
    case PerModelThrottleWarning:
    case PerModelThrottleInvert:
      return 1;
    case pmSwitchMask:
      return 1;
    case SlowScale:
      return (IS_ARM(board) ? 10 : 2);
    case SlowRange:
      return (IS_ARM(board) ? 250 : 15);
    case CSFunc:
      return 18;
    case GvarsNum:
    case GvarsOfsNum:  
      return 5;
    case HasSDLogs:
      return ((board == BOARD_GRUVIN9X || IS_ARM(board)) ? true : false);
    case LCDWidth:
      return (IS_TARANIS(board) ? 212 : 128) ;
    case GetThrSwitch:
      return (IS_TARANIS(board) ?DSW_SF1 : DSW_THR) ;
    default:
      return 0;
  }
}

int Open9xInterface::isAvailable(Protocol proto, int port)
{
  if (IS_TARANIS(board)) {
    switch (port) {
      case 0:
        switch (proto) {
          case OFF:
          case PXX_XJT_X16:
          case PXX_XJT_D8:
          case PXX_XJT_LR12:
            return 1;
          default:
            return 0;
        }
        break;
      case 1:
        switch (proto) {
          case OFF:
          case PPM:
          case PXX_XJT_X16:
          case PXX_XJT_D8:
          case PXX_XJT_LR12:
          case PXX_DJT:
            return 1;
          default:
            return 0;
        }
        break;
      case 2:
        switch (proto) {
          case PPM:
            return 1;
          default:
            return 0;
        }
        break;
      default:
        return 0;
    }
  }
  else if (board==BOARD_SKY9X) {
    switch (port) {
      case 0:
        switch (proto) {
          case PPM:
          case PXX_DJT:
          case LP45:
          case DSM2:
          case DSMX:
            return 1;
          default:
            return 0;
        }
        break;
      case 1:
        switch (proto) {
          case PPM:
            return 1;
          default:
            return 0;
        }
        break;
      default:
        return 0;
    }   
  }
  else {
    switch (proto) {
      case PPM:
      case DSMX:
      case LP45:
      case DSM2:
      case PXX_DJT:
      case PPM16:
      case PPMSIM:
        return 1;
      default:
        return 0;
    }
  }
}

SimulatorInterface * Open9xInterface::getSimulator()
{
  switch (board) {
    case BOARD_STOCK:
      return new Open9xSimulator(this);
    case BOARD_M128:
      return new Open9xM128Simulator(this);
    case BOARD_GRUVIN9X:
      return new Open9xGruvin9xSimulator(this);
    case BOARD_SKY9X:
      return new Open9xSky9xSimulator(this);
    case BOARD_TARANIS:
      return new OpentxTaranisSimulator(this);
    case BOARD_TARANIS_REV4a:
      return new OpentxTaranisSimulator(this);
    default:
      return NULL;
  }
}

template<typename T, size_t SIZE>
size_t getSizeA(T (&)[SIZE]) {
    return SIZE;
}

bool Open9xInterface::checkVersion(unsigned int version)
{
  switch(version) {
    case 201:
      // first version
      break;
    case 202:
      // channel order is now always RUD - ELE - THR - AIL
      // changes in timers
      // ppmFrameLength added
      // thrTraceSrc added
      break;
    case 203:
      // mixers changed (for the trims use for change the offset of a mix)
      // telemetry offset raised to -127 +127
      // function switches now have a param on 4 bits
      break;
    case 204:
      // telemetry changes (bars)
      break;
    case 205:
      // mixer changes (differential, negative curves)...
      break;
    // case 206:
    case 207:
      // V4: Rotary Encoders position in FlightPhases
      break;
    case 208:
      // Trim value in 16bits
      // FrSky A1/A2 offset on 12bits
      // ARM: More Mixers / Expos / CSW / FSW / CHNOUT
      break;
    case 209:
      // Add TrmR, TrmE, TrmT, TrmA as Mix sources
      // Trims are now OFF / ON / Rud / Ele / Thr / Ail
      break;
    case 210:
      // Add names in Mixes / Expos
      // Add a new telemetry screen
      // Add support for Play Track <filename>
      break;
    case 211:
      // Curves big change
      break;
    case 212:
      // Big changes in mixers / limitse
      break;
    case 213:
      // GVARS / Variants introduction
      break;
    case 214:
      // Massive EEPROM change!
      break;
    case 215:
      // M128 revert because too much RAM used!
      break;
    default:
      return false;
  }

  return true;
}

bool Open9xInterface::checkVariant(unsigned int version, unsigned int variant)
{
  if (board == BOARD_M128 && !(variant & 0x8000)) {
    if (version == 212) {
      uint8_t tmp[1000];
      for (int i=1; i<31; i++) {
        efile->openRd(i);
        int sz = efile->readRlc2(tmp, sizeof(tmp));
        if (sz == 849) {
          std::cout << " warning: M128 variant not set (but model size seems ok)";
          return true;
        }
      }
    }
    std::cout << " error when loading M128 general settings (wrong variant)";
    return false;
  }
  else {
    return true;
  }
}

bool Open9xInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  std::cout << "trying " << getName() << " backup import...";

  if (esize < 8 || memcmp(eeprom, "o9x", 3) != 0 || eeprom[3] != 0x30+board) {
    std::cout << " no\n";
    return false;
  }

  uint8_t version = eeprom[4];
  uint8_t bcktype = eeprom[5];
  uint16_t size = ((uint16_t)eeprom[7] << 8) + eeprom[6];
  uint16_t variant = ((uint16_t)eeprom[9] << 8) + eeprom[8];

  std::cout << " version " << (unsigned int)version << " ";

  if (!checkVersion(version)) {
    std::cout << " not open9x\n";
    return false;
  }

  if (size > esize-8) {
    std::cout << " wrong size\n";
    return false;
  }

  if (bcktype=='M') {
    if (!loadModel(version, radioData.models[index], &eeprom[8], size, variant)) {
      std::cout << " ko\n";
      return false;
    }
  }
  else {
    std::cout << " backup type not supported\n";
    return false;
  }

  std::cout << " ok\n";
  return true;
}

QString geturl( int board)
{
    QString url="http://";
    QSettings settings("companion9x", "companion9x");
    int server = settings.value("fwserver", 0).toInt();
    if (server >= o9xservers.count()) {
      server = 0;
      settings.setValue("fwserver",server);
    }
    url.append(o9xservers.at(server));
    switch(board) {
      case BOARD_STOCK:
      case BOARD_M128:
      case BOARD_GRUVIN9X:
        url.append("/getfw.php?fw=%1.hex");
        break;
      case BOARD_SKY9X:
      case BOARD_TARANIS:
      case BOARD_TARANIS_REV4a:
        url.append("/getfw.php?fw=%1.bin");
        break;
      default:
        url.clear();
        break;
    }
    return url;
}

QString getstamp( int board)
{
    QString url="http://";
    QSettings settings("companion9x", "companion9x");
    int server = settings.value("fwserver",0).toInt();
    if (server >=o9xservers.count()) {
      server=0;
      settings.setValue("fwserver",server);
    }
    url.append(o9xservers.at(server));
    url.append("/binaries/stamp-opentx-");
    switch(board) {
      case BOARD_STOCK:
        url.append("stock.txt");
        break;
      case BOARD_M128:
        url.append("stock128.txt");
        break;
      case BOARD_GRUVIN9X:
        url.append("v4.txt");
        break;
      case BOARD_SKY9X:
        url.append("arm.txt");
        break;
      case BOARD_TARANIS:
      case BOARD_TARANIS_REV4a:
        url.append("taranis.txt");
        break;
      default:
        url.clear();
        break;
    }
    return url;
}

QString getrnurl( int board)
{
    QString url="http://";
    QSettings settings("companion9x", "companion9x");
    int server = settings.value("fwserver",0).toInt();
    if (server >=o9xservers.count()) {
      server=0;
      settings.setValue("fwserver",server);
    }
    url.append(o9xservers.at(server));
    url.append("/docs/releasenotes-");
    switch(board) {
      case BOARD_STOCK:
      case BOARD_M128:
      case BOARD_GRUVIN9X:
      case BOARD_SKY9X:
        url.append("9x.txt");
        break;
      case BOARD_TARANIS:
      case BOARD_TARANIS_REV4a:
        url.append("taranis.txt");
        break;
      default:
        url.clear();
        break;
    }
    return url;
}

void RegisterOpen9xFirmwares()
{
  Open9xFirmware * open9x;

  Option ext_options[] = { { "frsky", QObject::tr("Support for frsky telemetry mod"), FRSKY_VARIANT }, { "telemetrez", QObject::tr("Support for telemetry easy board"), FRSKY_VARIANT }, { "jeti", QObject::tr("Support for jeti telemetry mod"), 0 }, { "ardupilot", QObject::tr("Support for receiving ardupilot data"), 0 }, { "nmea", QObject::tr("Support for receiving NMEA data"), 0 }, { "mavlink", QObject::tr("Support for MAVLINK devices"), MAVLINK_VARIANT }, { NULL } };
  Option nav_options[] = { { "rotenc", QObject::tr("Rotary Encoder use in menus navigation") }, { "potscroll", QObject::tr("Pots use in menus navigation") }, { NULL } };
  Option extr_options[] = { { "frsky", QObject::tr("Support for frsky telemetry mod"), FRSKY_VARIANT }, { "jeti", QObject::tr("Support for jeti telemetry mod"), 0 }, { "ardupilot", QObject::tr("Support for receiving ardupilot data"), 0 }, { "nmea", QObject::tr("Support for receiving NMEA data"), 0 }, { "mavlink", QObject::tr("Support for MAVLINK devices"), MAVLINK_VARIANT }, { NULL } };
  Option fai_options[] = { { "faichoice", QObject::tr("Possibility to enable FAI MODE at field") }, { "faimode", QObject::tr("FAI MODE always enabled") }, { NULL } };
  /* 9x board */
  open9x = new Open9xFirmware("opentx-stock", QObject::tr("openTx for 9X board"), new Open9xInterface(BOARD_STOCK), geturl(BOARD_STOCK), getstamp(BOARD_STOCK), getrnurl(BOARD_STOCK), false);
  open9x->addOptions(ext_options);
  open9x->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  open9x->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  open9x->addOption("nosplash", QObject::tr("No splash screen"));
  open9x->addOption("nofp", QObject::tr("No flight modes"));
  open9x->addOption("nocurves", QObject::tr("Disable curves menus"));
  open9x->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  open9x->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  open9x->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED open9x->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  open9x->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  open9x->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  open9x->addOption("symlimits", QObject::tr("Symetrical Limits"));
  open9x->addOptions(nav_options);
  open9x->addOption("sp22", QObject::tr("SmartieParts 2.2 Backlight support"));
  open9x->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving some of them"));
  open9x->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving some of them"));
  open9x->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  open9x->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  open9x->addOption("battgraph", QObject::tr("Battery graph"));
  open9x->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  open9x->addOption("pgbar", QObject::tr("EEprom write progress bar"));
  open9x->addOption("imperial", QObject::tr("Imperial units"));
  open9x->addOption("nowshh", QObject::tr("No Winged Shadow How High support"));
  open9x->addOption("novario", QObject::tr("No vario support"));
  open9x->addOption("nogps", QObject::tr("No GPS support"));
  open9x->addOption("nogauges", QObject::tr("No gauges in the custom telemetry screen"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);

  /* 9x board with M128 chip */
  open9x = new Open9xFirmware("opentx-stock128", QObject::tr("openTx for M128 / 9X board"), new Open9xInterface(BOARD_M128), geturl(BOARD_M128), getstamp(BOARD_M128),getrnurl(BOARD_M128), false);
  open9x->addOptions(ext_options);
  open9x->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  open9x->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  open9x->addOption("nosplash", QObject::tr("No splash screen"));
  open9x->addOption("nofp", QObject::tr("No flight modes"));
  open9x->addOption("nocurves", QObject::tr("Disable curves menus"));
  open9x->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  open9x->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  open9x->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED open9x->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  open9x->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  open9x->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  open9x->addOption("symlimits", QObject::tr("Symetrical Limits"));
  open9x->addOptions(nav_options);
  open9x->addOption("sp22", QObject::tr("SmartieParts 2.2 Backlight support"));
  open9x->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving some of them"));
  open9x->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving some of them"));
  open9x->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  open9x->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  open9x->addOption("battgraph", QObject::tr("Battery graph"));
  open9x->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  open9x->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  open9x->addOption("imperial", QObject::tr("Imperial units"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);

  /* 9XR board */
  open9x = new Open9xFirmware("opentx-9xr", QObject::tr("openTx for 9XR"), new Open9xInterface(BOARD_STOCK), geturl(BOARD_STOCK), getstamp(BOARD_STOCK),getrnurl(BOARD_STOCK), false);
  open9x->addOptions(extr_options);
  open9x->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  open9x->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  open9x->addOption("nosplash", QObject::tr("No splash screen"));
  open9x->addOption("nofp", QObject::tr("No flight modes"));
  open9x->addOption("nocurves", QObject::tr("Disable curves menus"));
  open9x->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  open9x->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  open9x->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED open9x->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  open9x->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  open9x->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  open9x->addOption("symlimits", QObject::tr("Symetrical Limits"));
  open9x->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  open9x->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving some of them"));
  open9x->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving some of them"));
  open9x->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  open9x->addOption("battgraph", QObject::tr("Battery graph"));
  open9x->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  open9x->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  open9x->addOption("imperial", QObject::tr("Imperial units"));
  open9x->addOption("nowshh", QObject::tr("No Winged Shadow How High support"));
  open9x->addOption("novario", QObject::tr("No vario support"));
  open9x->addOption("nogps", QObject::tr("No GPS support"));
  open9x->addOption("nogauges", QObject::tr("No gauges in the custom telemetry screen"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);

  /* 9XR board with M128 chip */
  open9x = new Open9xFirmware("opentx-9xr128", QObject::tr("openTx for 9XR with M128 chip"), new Open9xInterface(BOARD_M128), geturl(BOARD_M128), getstamp(BOARD_M128),getrnurl(BOARD_M128), false);
  open9x->addOptions(extr_options);
  open9x->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  open9x->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  open9x->addOption("nosplash", QObject::tr("No splash screen"));
  open9x->addOption("nofp", QObject::tr("No flight modes"));
  open9x->addOption("nocurves", QObject::tr("Disable curves menus"));
  open9x->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  open9x->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  open9x->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED open9x->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  open9x->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  open9x->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  open9x->addOption("symlimits", QObject::tr("Symetrical Limits"));
  open9x->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  open9x->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving some of them"));
  open9x->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving some of them"));
  open9x->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  open9x->addOption("battgraph", QObject::tr("Battery graph"));
  open9x->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  open9x->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  open9x->addOption("imperial", QObject::tr("Imperial units"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);

  /* Gruvin9x board */
  open9x = new Open9xFirmware("opentx-v4", QObject::tr("openTx for Gruvin9x board / 9X"), new Open9xInterface(BOARD_GRUVIN9X), geturl(BOARD_GRUVIN9X), getstamp(BOARD_GRUVIN9X),getrnurl(BOARD_GRUVIN9X), false);
  open9x->setVariantBase(FRSKY_VARIANT);
  open9x->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  open9x->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  open9x->addOption("nofp", QObject::tr("No flight modes"));
  open9x->addOption("nocurves", QObject::tr("Disable curves menus"));
  open9x->addOption("sdcard", QObject::tr("Support for SD memory card"));
  open9x->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  open9x->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  Option dsm2_options[] = { { "DSM2", QObject::tr("Support for DSM2 modules"), 0 }, { "DSM2PPM", QObject::tr("Support for DSM2 modules using ppm instead of true serial"), 0 }, { NULL } };
  open9x->addOptions(dsm2_options);
  open9x->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  open9x->addOption("symlimits", QObject::tr("Symetrical Limits"));
  open9x->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  open9x->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving some of them"));
  open9x->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving some of them"));
  open9x->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  open9x->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  open9x->addOption("battgraph", QObject::tr("Battery graph"));
  open9x->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  open9x->addOption("imperial", QObject::tr("Imperial units"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);

#ifndef __APPLE__
  /* SKY9X board */
  open9x = new Open9xFirmware("opentx-sky9x", QObject::tr("openTx for Sky9x board / 9X"), new Open9xInterface(BOARD_SKY9X), geturl(BOARD_SKY9X), getstamp(BOARD_SKY9X),getrnurl(BOARD_SKY9X), true);
  open9x->setVariantBase(FRSKY_VARIANT);
  open9x->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  open9x->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  open9x->addOption("nofp", QObject::tr("No flight modes"));
  open9x->addOption("nocurves", QObject::tr("Disable curves menus"));
  open9x->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  open9x->addOption("symlimits", QObject::tr("Symetrical Limits"));
  open9x->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  open9x->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving some of them"));
  open9x->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving some of them"));
  open9x->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  open9x->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  open9x->addOption("battgraph", QObject::tr("Battery graph"));
  open9x->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOption("tsticks", QObject::tr("Use FrSky Taranis sticks in a 9X/9XR"));
  open9x->addOption("bluetooth", QObject::tr("Bluetooth interface"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);
#endif
  
  /* Taranis board */
  open9x = new Open9xFirmware("opentx-taranis", QObject::tr("openTx for FrSky Taranis"), new Open9xInterface(BOARD_TARANIS), geturl(BOARD_TARANIS), getstamp(BOARD_TARANIS),getrnurl(BOARD_TARANIS), true);
  open9x->addOption("noheli", QObject::tr("Disable HELI menu and cyclic mix support"));
  open9x->addOption("notemplates", QObject::tr("Disable TEMPLATES menu"));
  open9x->addOption("nogvars", QObject::tr("Disable Global variables"));
  open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
  open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  open9x->addOptions(fai_options);
  firmwares.push_back(open9x);

  QSettings settings("companion9x", "companion9x");
  int rev4a = settings.value("rev4asupport",0).toInt();
  if (rev4a) {
    open9x = new Open9xFirmware("opentx-taranisrev4a", QObject::tr("openTx for FrSky Taranis Rev4a"), new Open9xInterface(BOARD_TARANIS_REV4a), geturl(BOARD_TARANIS_REV4a), getstamp(BOARD_TARANIS_REV4a),getrnurl(BOARD_TARANIS), true);
    open9x->addOption("noheli", QObject::tr("Disable HELI menu and cyclic mix support"));
    open9x->addOption("notemplates", QObject::tr("Disable TEMPLATES menu"));
    open9x->addOption("nogvars", QObject::tr("Disable Global variables"));
    open9x->addOption("ppmus", QObject::tr("PPM values displayed in us"));
    open9x->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
    open9x->addOptions(fai_options);
    firmwares.push_back(open9x);
  }
}
