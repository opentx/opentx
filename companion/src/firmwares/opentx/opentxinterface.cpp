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
#include "opentxinterface.h"
#include "opentxeeprom.h"
#include "open9xGruvin9xeeprom.h"
#include "open9xSky9xeeprom.h"
#include "file.h"
#include "appdata.h"

#define OPENTX_FIRMWARE_DOWNLOADS        "http://downloads-21.open-tx.org/firmware"
#define OPENTX_NIGHT_FIRMWARE_DOWNLOADS  "http://downloads-21.open-tx.org/nightly/firmware"

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

OpenTxEepromInterface::OpenTxEepromInterface(BoardEnum board):
  EEPROMInterface(board),
  efile(new RleFile())
{
}

OpenTxEepromInterface::~OpenTxEepromInterface()
{
  delete efile;
}

const char * OpenTxEepromInterface::getName()
{
  switch (board) {
    case BOARD_STOCK:
      return "OpenTX for 9X board";
    case BOARD_M128:
      return "OpenTX for M128 / 9X board";
    case BOARD_MEGA2560:
      return "OpenTX for MEGA2560 board";
    case BOARD_GRUVIN9X:
      return "OpenTX for Gruvin9x board / 9X";
    case BOARD_TARANIS:
      return "OpenTX for FrSky Taranis";
    case BOARD_TARANIS_PLUS:
      return "OpenTX for FrSky Taranis Plus";
    case BOARD_TARANIS_X9E:
      return "OpenTX for FrSky Taranis X9E";
    case BOARD_SKY9X:
      return "OpenTX for Sky9x board / 9X";
    case BOARD_9XRPRO:
      return "OpenTX for 9XR-PRO";
    case BOARD_AR9X:
      return "OpenTX for ar9x board / 9X";
    default:
      return "OpenTX for an unknown board";
  }
}

const int OpenTxEepromInterface::getEEpromSize()
{
  switch (board) {
    case BOARD_STOCK:
      return EESIZE_STOCK;
    case BOARD_M128:
      return EESIZE_M128;
    case BOARD_MEGA2560:
    case BOARD_GRUVIN9X:
      return EESIZE_GRUVIN9X;
    case BOARD_SKY9X:
      return EESIZE_SKY9X;
    case BOARD_9XRPRO:
    case BOARD_AR9X:
      return EESIZE_9XRPRO;
    case BOARD_TARANIS:
    case BOARD_TARANIS_PLUS:
    case BOARD_TARANIS_X9E:
      return EESIZE_TARANIS;
    default:
      return 0;
  }
}

const int OpenTxEepromInterface::getMaxModels()
{
  if (IS_ARM(board))
    return 60;
  else if (board == BOARD_M128)
    return 30;
  else if (IS_2560(board))
    return 30;
  else
    return 16;
}

template <class T>
bool OpenTxEepromInterface::loadModel(ModelData &model, uint8_t *data, int index, unsigned int stickMode)
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
bool OpenTxEepromInterface::loadModelVariant(unsigned int index, ModelData &model, uint8_t *data, unsigned int version, unsigned int variant)
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
    QByteArray backupData((char *)data, index);
    QByteArray modelData;
    if (IS_SKY9X(board))
      modelData = backupData;
    else
      importRlc(modelData, backupData);
    if (modelData.size()) {
      open9xModel.Import(modelData);
      // open9xModel.Dump();
      model.used = true;
    }
    else {
      model.clear();
    }
  }

  return true;
}

bool OpenTxEepromInterface::loadModel(uint8_t version, ModelData &model, uint8_t *data, int index, unsigned int variant, unsigned int stickMode)
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
    else if (IS_SKY9X(board)) {
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
    else if (IS_SKY9X(board)) {
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
    else if (IS_SKY9X(board)) {
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
    else if (IS_SKY9X(board)) {
      return loadModel<Open9xArmModelData_v211>(model, data, index, 0 /*no more stick mode messed*/);
    }
    else {
      return loadModel<Open9xModelData_v211>(model, data, index, 0 /*no more stick mode messed*/);
    }
  }
  else if (version == 212) {
    if (IS_SKY9X(board)) {
      return loadModel<Open9xArmModelData_v212>(model, data, index);
    }
    else {
      return loadModelVariant<OpenTxModelData>(index, model, data, version, variant);
    }
  }
  else if (version >= 213) {
    return loadModelVariant<OpenTxModelData>(index, model, data, version, variant);
  }

  std::cout << " ko\n";
  return false;
}

template <class T>
bool OpenTxEepromInterface::loadGeneral(GeneralSettings &settings, unsigned int version)
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
bool OpenTxEepromInterface::saveGeneral(GeneralSettings &settings, BoardEnum board, uint32_t version, uint32_t variant)
{
  T open9xSettings(settings, board, version, variant);
  // open9xSettings.Dump();
  QByteArray eeprom;
  open9xSettings.Export(eeprom);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (const uint8_t*)eeprom.constData(), eeprom.size());
  return (sz == eeprom.size());
}

template <class T>
bool OpenTxEepromInterface::saveModel(unsigned int index, ModelData &model, unsigned int version, unsigned int variant)
{
  T open9xModel(model, board, version, variant);
  // open9xModel.Dump();
  QByteArray eeprom;
  open9xModel.Export(eeprom);
  int sz = efile->writeRlc2(FILE_MODEL(index), FILE_TYP_MODEL, (const uint8_t*)eeprom.constData(), eeprom.size());
  return (sz == eeprom.size());
}

unsigned long OpenTxEepromInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  std::bitset<NUM_ERRORS> errors;
  errors.set(UNKNOWN_ERROR);
  return errors.to_ulong();
}

unsigned long OpenTxEepromInterface::load(RadioData &radioData, const uint8_t *eeprom, int size)
{
  std::cout << "trying " << getName() << " import...";

  std::bitset<NUM_ERRORS> errors;

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
        errors.set(WRONG_SIZE);
        return errors.to_ulong();
      }
      else {
        errors.set(HAS_WARNINGS);
        errors.set(WARNING_WRONG_FIRMWARE);
        size=2048;
      }
    } else {
      std::cout << " wrong size (" << size << "/" << getEEpromSize() << ")\n";
      errors.set(WRONG_SIZE);
      return errors.to_ulong();
    }
  }

  if (!efile->EeFsOpen((uint8_t *)eeprom, size, board)) {
    std::cout << " wrong file system\n";
    errors.set(WRONG_FILE_SYSTEM);
    return errors.to_ulong();
  }

  efile->openRd(FILE_GENERAL);

  uint8_t version;
  if (efile->readRlc2(&version, 1) != 1) {
    std::cout << " no\n";
    errors.set(UNKNOWN_ERROR);
    return errors.to_ulong();
  }

  std::cout << " version " << (unsigned int)version;

  EepromLoadErrors version_error = checkVersion(version);
  if (version_error == OLD_VERSION) {
    errors.set(version_error);
    errors.set(HAS_WARNINGS);
  } else if (version_error == NOT_OPENTX) {
    std::cout << " not open9x\n";
    errors.set(version_error);
    return errors.to_ulong();
  }

  if (!loadGeneral<OpenTxGeneralData>(radioData.generalSettings, version)) {
    std::cout << " ko\n";
    errors.set(UNKNOWN_ERROR);
    return errors.to_ulong();
  }

  std::cout << " variant " << radioData.generalSettings.variant;
  for (int i=0; i<getMaxModels(); i++) {
    if (!loadModel(version, radioData.models[i], NULL, i, radioData.generalSettings.variant, radioData.generalSettings.stickMode+1)) {
      std::cout << " ko\n";
      errors.set(UNKNOWN_ERROR);
      return errors.to_ulong();
    }
  }
  std::cout << " ok\n";
  errors.set(NO_ERROR);
  return errors.to_ulong();
}

int OpenTxEepromInterface::save(uint8_t *eeprom, RadioData &radioData, uint32_t variant, uint8_t version)
{
  EEPROMWarnings.clear();

  if (!version) {
    switch(board) {
      case BOARD_TARANIS:
      case BOARD_TARANIS_PLUS:
      case BOARD_TARANIS_X9E:
      case BOARD_SKY9X:
      case BOARD_AR9X:
      case BOARD_9XRPRO:
        version = 218;
        break;
      case BOARD_GRUVIN9X:
      case BOARD_MEGA2560:
        version = 217;
        break;
      case BOARD_M128:
        version = 217;
        break;
      case BOARD_STOCK:
        version = 216;
        break;
    }
  }

  int size = getEEpromSize();

  efile->EeFsCreate(eeprom, size, board, version);

  if (board == BOARD_M128) {
    variant |= M128_VARIANT;
  }
  else if (board == BOARD_TARANIS_X9E) {
    variant |= TARANIS_X9E_VARIANT;
  }

  int result = saveGeneral<OpenTxGeneralData>(radioData.generalSettings, board, version, variant);
  if (!result) {
    return 0;
  }

  for (int i=0; i<getMaxModels(); i++) {
    if (!radioData.models[i].isEmpty()) {
      result = saveModel<OpenTxModelData>(i, radioData.models[i], version, variant);
      if (!result) {
        return 0;
      }
    }
  }

  if (!EEPROMWarnings.empty()) {
    QString msg;
    int noErrorsToDisplay = std::min((int)EEPROMWarnings.size(),10);
    for (int n = 0; n < noErrorsToDisplay; n++) {
      msg += "-" + EEPROMWarnings.front() + "\n";
      EEPROMWarnings.pop_front();
    }
    if (!EEPROMWarnings.empty()) {
      msg = QObject::tr("(displaying only first 10 warnings)") + "\n" + msg;
    }
    EEPROMWarnings.clear();
    QMessageBox::warning(NULL,
        QObject::tr("Warning"),
        QObject::tr("EEPROM saved with these warnings:") + "\n" + msg);
  }

  return size;
}

int OpenTxEepromInterface::getSize(const ModelData & model)
{
  if (IS_SKY9X(board))
    return 0;

  if (model.isEmpty())
    return 0;

  QByteArray tmp(EESIZE_MAX, 0);
  efile->EeFsCreate((uint8_t *)tmp.data(), EESIZE_MAX, board, 255/*version max*/);

  OpenTxModelData open9xModel((ModelData &)model, board, 255/*version max*/, GetCurrentFirmware()->getVariantNumber());

  QByteArray eeprom;
  open9xModel.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_MODEL, (const uint8_t*)eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

int OpenTxEepromInterface::getSize(const GeneralSettings & settings)
{
  if (IS_SKY9X(board))
    return 0;

  QByteArray tmp(EESIZE_MAX, 0);
  efile->EeFsCreate((uint8_t *)tmp.data(), EESIZE_MAX, board, 255);

  OpenTxGeneralData open9xGeneral((GeneralSettings &)settings, board, 255, GetCurrentFirmware()->getVariantNumber());
  // open9xGeneral.Dump();

  QByteArray eeprom;
  open9xGeneral.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_GENERAL, (const uint8_t*)eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

Firmware * OpenTxFirmware::getFirmwareVariant(const QString & id)
{
  if (id == getId()) {
    return this;
  }
  else if (id.contains(getId()+"-") || (!id.contains("-") && id.contains(getId()))) {
    Firmware * result = new OpenTxFirmware(id, this);
    // TODO result.variant = firmware->getVariant(id);
    return result;
  }
  else {
    return NULL;
  }
}

int OpenTxFirmware::getCapability(const Capability capability)
{
  switch (capability) {
    case Imperial:
      if (IS_ARM(board))
        return 0;
      else
        return id.contains("imperial") ? 1 : 0;
    case ModelImage:
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
    case FlightModes:
      if (IS_ARM(board))
        return 9;
      else if (IS_2560(board))
        return 6;
      else
        return 5;
    case FlightModesHaveFades:
      return 1;
    case Heli:
      if (IS_TARANIS(board))
        return id.contains("noheli") ? 0 : 1;
      else
        return id.contains("heli") ? 1 : 0;
    case Gvars:
      if (IS_TARANIS(board))
        return id.contains("nogvars") ? 0 : 9;
      else if (id.contains("gvars"))
        return IS_ARM(board) ? 9 : 5;
      else
        return 0;
    case FlightModesName:
      return (IS_TARANIS(board) ? 10 : 6);
    case GvarsName:
      return (IS_9X(board) ? 0 : 6);
    case HasChNames:
      return (IS_TARANIS(board) ? 1 : 0);
    case GvarsInCS:
    case HasFAIMode:
      return 1;
    case GvarsAreNamed:
    case GvarsFlightModes:
      return ((IS_ARM(board) || IS_2560(board)) ? 1 : 0);
    case Mixes:
      return (IS_TARANIS(board) ? 64 : (IS_ARM(board) ? 60 : 32));
    case OffsetWeight:
      return (IS_ARM(board) ? 500 : 245);
    case Timers:
      return ((IS_ARM(board) && id.contains("timer3")) ? 3 : 2);
    case TimersName:
      return (IS_TARANIS(board) ? 8 : (IS_ARM(board) ? 3 : 0));
    case PermTimers:
      if (IS_2560(board) || IS_ARM(board))
        return 1;
      else
        return 0;
    case Pots:
      if (IS_TARANIS_X9E(board))
        return 4;
      else if (IS_TARANIS(board))
        return 3;   //Taranis has only 2 pots but still has a placeholder in settings for 3 pots
      else
        return 3;
    case Sliders:
      if (IS_TARANIS_X9E(board))
        return 4;
      else if (IS_TARANIS(board))
        return 2;
      else
        return 0;
    case Switches:
      if (IS_TARANIS_X9E(board))
        return 18;
      else if (IS_TARANIS(board))
        return 8;
      else
        return 7;
    case SwitchesPositions:
      if (IS_TARANIS_X9E(board))
        return 18*3;
      else if (IS_TARANIS(board))
        return 8*3;
      else
        return 9;
    case CustomFunctions:
      if (IS_ARM(board))
        return 64;
      else if (IS_2560(board) || board==BOARD_M128)
        return 24;
      else
        return 16;
    case SafetyChannelCustomFunction:
      return id.contains("nooverridech") ? 0 : 1;
    case LogicalSwitches:
      if (IS_ARM(board))
        return 64;
      else
        return 12;
    case CustomAndSwitches:
      if (IS_ARM(board))
        return getCapability(LogicalSwitches);
      else
        return 15/*4bits*/-9/*sw positions*/;
    case LogicalSwitchesExt:
      return (IS_ARM(board) ? true : false);
    case RotaryEncoders:
      if (board == BOARD_GRUVIN9X)
        return 2;
      else if (IS_SKY9X(board))
        return 1;
      else
        return 0;
    case Outputs:
      return (IS_ARM(board) ? 32 : 16);
    case NumCurvePoints:
      return (IS_ARM(board) ? 512 : 104);
    case VoicesAsNumbers:
      return (IS_ARM(board) ? 0 : 1);
    case VoicesMaxLength:
      return (IS_ARM(board) ? (IS_TARANIS(board) ? 8 :  6) : 0);
    case MultiLangVoice:
      return (IS_ARM(board) ? 1 : 0);
    case SoundPitch:
      return 1;
    case Haptic:
      return (IS_2560(board) || IS_SKY9X(board) || IS_TARANIS_PLUS(board) || id.contains("haptic"));
    case ModelTrainerEnable:
      if (IS_TARANIS(board))
        return 1;
      else
        return 0;
    case MaxVolume:
      return (IS_ARM(board) ? 23 : 7);
    case HasSoundMixer:
      return (IS_ARM(board) ? 1 : 0);
    case ExtraInputs:
      return 1;
    case ExtendedTrims:
      return 500;
    case Simulation:
      return 1;
    case NumCurves:
      return (IS_TARANIS(board) ? 32 : (IS_ARM(board) ? 16 : 8));
    case HasMixerNames:
      return (IS_ARM(board) ? (IS_TARANIS(board) ? 8 : 6) : false);
    case HasExpoNames:
      return (IS_ARM(board) ? (IS_TARANIS(board) ? 8 : 6) : false);
    case HasNoExpo:
      return (IS_TARANIS(board) ? false : true);
    case ChannelsName:
      return (IS_TARANIS(board) ? 6 : 0);
    case HasCvNames:
      return (IS_TARANIS(board) ? 1 : 0);
    case Telemetry:
      if (IS_2560(board) || IS_ARM(board) || id.contains("frsky") || id.contains("telemetrez"))
        return TM_HASTELEMETRY|TM_HASOFFSET|TM_HASWSHH;
      else
        return 0;
    case TelemetryBars:
      return 1;
    case TelemetryCustomScreens:
      return IS_ARM(board) ? 4 : 2;
    case TelemetryCustomScreensFieldsPerLine:
      return IS_TARANIS(board) ? 3 : 2;
    case NoTelemetryProtocol:
      return IS_TARANIS(board) ? 1 : 0;
    case TelemetryUnits:
      return 0;
    case TelemetryMaxMultiplier:
      return (IS_ARM(board) ? 32 : 8);
    case PPMCenter:
      return (IS_TARANIS(board) ? 500 : (id.contains("ppmca") ? 125 : 0));
    case PPMUnitMicroseconds:
      return id.contains("ppmus") ? 1 : 0;
    case SYMLimits:
      return 1;
    case OptrexDisplay:
      return (board==BOARD_SKY9X ? true : false);
    case HasVario:
      return 1;
    case HasVarioSink:
      return ((IS_2560(board) || IS_ARM(board)) ? true : false);
    case HasFailsafe:
      return (IS_ARM(board) ? 32 : 0);
    case NumModules:
      return (IS_ARM(board) ? 2 : 1);
    case HasPPMStart:
      return (IS_ARM(board) ? true : false);
    case HastxCurrentCalibration:
      return (IS_SKY9X(board) ? true : false);
    case HasVolume:
      return (IS_ARM(board) ? true : false);
    case HasBrightness:
      return (IS_ARM(board) ? true : false);
    case PerModelTimers:
      return 1;
    case SlowScale:
      return (IS_ARM(board) ? 10 : 2);
    case SlowRange:
      return (IS_ARM(board) ? 250 : 15);
    case CSFunc:
      return 18;
    case HasSDLogs:
      return ((IS_2560(board) || IS_ARM(board)) ? true : false);
    case LCDWidth:
      return (IS_TARANIS(board) ? 212 : 128) ;
    case GetThrSwitch:
      return (IS_TARANIS(board) ? SWITCH_SF1 : SWITCH_THR) ;
    case HasDisplayText:
      return IS_ARM(board) ? 1 : 0;
    case HasTopLcd:
      return IS_TARANIS_X9E(board) ? 1 : 0;
    case GlobalFunctions:
      return IS_ARM(board) ? 64 : 0;
    case VirtualInputs:
      return IS_TARANIS(board) ? 32 : 0;
    case TrainerInputs:
      return IS_ARM(board) ? 16 : 8;
    case RtcTime:
      return IS_ARM(board) || IS_2560(board) ? 1 : 0;
    case LuaScripts:
      return IS_TARANIS(board) && id.contains("lua") ? 7 : 0;
    case LuaInputsPerScript:
      return IS_TARANIS(board) ? 10 : 0;
    case LuaOutputsPerScript:
      return IS_TARANIS(board) ? 6 : 0;
    case LimitsPer1000:
    case EnhancedCurves:
      return IS_TARANIS(board);
    case HasFasOffset:
      return (IS_STOCK(board) ? false : true);
    case HasMahPersistent:
      return (IS_ARM(board) ? true : false);
    case MultiposPots:
      return IS_TARANIS(board) ? 3 : 0;
    case MultiposPotsPositions:
      return IS_TARANIS(board) ? 6 : 0;
    case SimulatorVariant:
      if (board == BOARD_STOCK)
        return SIMU_STOCK_VARIANTS;
      else if (board == BOARD_M128)
        return SIMU_M128_VARIANTS;
      else if (board == BOARD_TARANIS_X9E)
        return TARANIS_X9E_VARIANT;
      else
        return 0;
    case MavlinkTelemetry:
      return id.contains("mavlink") ? 1 : 0;
    case SportTelemetry:
      return IS_ARM(board) ? 1 : 0;
    case HasInputDiff:
    case HasMixerExpo:
      return (IS_TARANIS(board) ? true : false);
    case MixersMonitor:
      return id.contains("mixersmon") ? 1 : 0;
    case HasBatMeterRange:
      return (IS_TARANIS(board) ? true : false);
    case DangerousFunctions:
      return id.contains("danger") ? 1 : 0;
    default:
      return 0;
  }
}

bool OpenTxFirmware::isTelemetrySourceAvailable(int source)
{
  if (IS_TARANIS(board) && (source == TELEMETRY_SOURCE_RSSI_TX))
    return false;

  if (source == TELEMETRY_SOURCE_DTE)
    return false;

  return true;
}

int OpenTxEepromInterface::isAvailable(PulsesProtocol proto, int port)
{
  if (IS_TARANIS(board)) {
    switch (port) {
      case 0:
        switch (proto) {
          case PULSES_OFF:
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_D8:
          case PULSES_PXX_XJT_LR12:
            return 1;
          default:
            return 0;
        }
        break;
      case 1:
        switch (proto) {
          case PULSES_OFF:
          case PULSES_PPM:
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_D8:
          case PULSES_PXX_XJT_LR12:
          //case PULSES_PXX_DJT:     // Unavailable for now
          case PULSES_LP45:
          case PULSES_DSM2:
          case PULSES_DSMX:
            return 1;
          default:
            return 0;
        }
        break;
      case -1:
        switch (proto) {
          case PULSES_PPM:
            return 1;
          default:
            return 0;
        }
        break;
      default:
        return 0;
    }
  }
  else if (IS_SKY9X(board)) {
    switch (port) {
      case 0:
        switch (proto) {
          case PULSES_PPM:
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_D8:
          case PULSES_PXX_XJT_LR12:
          case PULSES_LP45:
          case PULSES_DSM2:
          case PULSES_DSMX:
            return 1;
          default:
            return 0;
        }
        break;
      case 1:
        switch (proto) {
          case PULSES_PPM:
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
      case PULSES_PPM:
      case PULSES_DSMX:
      case PULSES_LP45:
      case PULSES_DSM2:
      case PULSES_PXX_DJT:
      case PULSES_PPM16:
      case PULSES_PPMSIM:
        return 1;
      default:
        return 0;
    }
  }
}

template<typename T, size_t SIZE>
size_t getSizeA(T (&)[SIZE]) {
    return SIZE;
}

EepromLoadErrors OpenTxEepromInterface::checkVersion(unsigned int version)
{
  switch(version) {
    case 201:
      // first version
    case 202:
      // channel order is now always RUD - ELE - THR - AIL
      // changes in timers
      // ppmFrameLength added
      // thrTraceSrc added
    case 203:
      // mixers changed (for the trims use for change the offset of a mix)
      // telemetry offset raised to -127 +127
      // function switches now have a param on 4 bits
    case 204:
      // telemetry changes (bars)
    case 205:
      // mixer changes (differential, negative curves)...
    // case 206:
    case 207:
      // V4: Rotary Encoders position in FlightModes
    case 208:
      // Trim value in 16bits
      // FrSky A1/A2 offset on 12bits
      // ARM: More Mixers / Expos / CSW / FSW / CHNOUT
    case 209:
      // Add TrmR, TrmE, TrmT, TrmA as Mix sources
      // Trims are now OFF / ON / Rud / Ele / Thr / Ail
    case 210:
      // Add names in Mixes / Expos
      // Add a new telemetry screen
      // Add support for Play Track <filename>
    case 211:
      // Curves big change
    case 212:
      // Big changes in mixers / limitse
    case 213:
      // GVARS / Variants introduction
    case 214:
      // Massive EEPROM change!
    case 215:
      // M128 revert because too much RAM used!
    case 216:
      // A lot of things (first github release)
    case 217:
      // 3 logical switches removed on M128 / gruvin9x boards
      return OLD_VERSION;
    case 218:
      break;
    default:
      return NOT_OPENTX;
  }

  return NO_ERROR;
}

bool OpenTxEepromInterface::checkVariant(unsigned int version, unsigned int variant)
{
  if (board == BOARD_M128 && !(variant & M128_VARIANT)) {
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
    std::cout << " wrong variant (" << variant << ")";
    return false;
  }
  else if (board == BOARD_TARANIS_X9E) {
    if (variant != TARANIS_X9E_VARIANT) {
      std::cout << " wrong variant (" << variant << ")";
      return false;
    }
  }
  else if (IS_TARANIS(board)) {
    if (variant != 0) {
      std::cout << " wrong variant (" << variant << ")";
      return false;
    }
  }

  return true;
}

unsigned long OpenTxEepromInterface::loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index)
{
  std::bitset<NUM_ERRORS> errors;

  std::cout << "trying " << getName() << " backup import...";

  if (esize < 8 || memcmp(eeprom, "o9x", 3) != 0) {
    std::cout << " no\n";
    errors.set(WRONG_SIZE);
    return errors.to_ulong();
  }

  BoardEnum backupBoard = (BoardEnum)-1;
  switch (eeprom[3]) {
    case 0x33:
      backupBoard = BOARD_TARANIS;
      break;
    case 0x32:
      backupBoard = BOARD_SKY9X;
      break;
    case 0x31:
      backupBoard = BOARD_GRUVIN9X;
      break;
    default:
      std::cout << " unknown board\n";
      errors.set(UNKNOWN_BOARD);
      return errors.to_ulong();
  }

  if (backupBoard != board) {
    std::cout << " not right board\n";
    errors.set(WRONG_BOARD);
    return errors.to_ulong();
  }

  uint8_t version = eeprom[4];
  uint8_t bcktype = eeprom[5];
  uint16_t size = ((uint16_t)eeprom[7] << 8) + eeprom[6];
  uint16_t variant = ((uint16_t)eeprom[9] << 8) + eeprom[8];

  std::cout << " version " << (unsigned int)version << " ";

  EepromLoadErrors version_error = checkVersion(version);
  if (version_error == OLD_VERSION) {
    errors.set(version_error);
    errors.set(HAS_WARNINGS);
  } else if (version_error == NOT_OPENTX) {
    std::cout << " not open9x\n";
    errors.set(version_error);
    return errors.to_ulong();
  }

  if (size > esize-8) {
    std::cout << " wrong size\n";
    errors.set(WRONG_SIZE);
    return errors.to_ulong();
  }

  if (bcktype=='M') {
    if (!loadModel(version, radioData.models[index], &eeprom[8], size, variant)) {
      std::cout << " ko\n";
      errors.set(UNKNOWN_ERROR);
      return errors.to_ulong();
    }
  }
  else {
    std::cout << " backup type not supported\n";
    errors.set(BACKUP_NOT_SUPPORTED);
    return errors.to_ulong();
  }

  std::cout << " ok\n";
  errors.set(NO_ERROR);
  return errors.to_ulong();
}

QString OpenTxFirmware::getFirmwareBaseUrl()
{
#if defined(ALLOW_NIGHTLY_BUILDS)
  return (g.useFirmwareNightlyBuilds() ? OPENTX_NIGHT_FIRMWARE_DOWNLOADS : OPENTX_FIRMWARE_DOWNLOADS);
#else
  return OPENTX_FIRMWARE_DOWNLOADS;
#endif
}

QString OpenTxFirmware::getFirmwareUrl()
{
  QString url = getFirmwareBaseUrl();
  switch (board) {
    case BOARD_STOCK:
    case BOARD_M128:
    case BOARD_GRUVIN9X:
    case BOARD_MEGA2560:
      url.append(QString("/getfw.php?fw=%1.hex").arg(id));
      break;
    case BOARD_SKY9X:
    case BOARD_AR9X:
    case BOARD_9XRPRO:
    case BOARD_TARANIS:
    case BOARD_TARANIS_PLUS:
    case BOARD_TARANIS_X9E:
      url.append(QString("/getfw.php?fw=%1.bin").arg(id));
      break;
    default:
      url.clear();
      break;
  }
  return url;
}

QString OpenTxFirmware::getReleaseNotesUrl()
{
  QString url = getFirmwareBaseUrl();
  url.append("/releasenotes.txt");
  return url;
}

QString OpenTxFirmware::getStampUrl()
{
  QString url = getFirmwareBaseUrl();
  url.append("/stamp-opentx.txt");
  return url;
}

void addOpenTxCommonOptions(OpenTxFirmware * firmware)
{
  firmware->addOption("ppmus", QObject::tr("Channel values displayed in us"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  firmware->addOption("nooverridech", QObject::tr("No OverrideCH functions available"));
  Option fai_options[] = { { "faichoice", QObject::tr("Possibility to enable FAI MODE (no telemetry) at field") }, { "faimode", QObject::tr("FAI MODE (no telemetry) always enabled") }, { NULL } };
  firmware->addOptions(fai_options);
}

void addOpenTxTaranisOptions(OpenTxFirmware * firmware)
{
  firmware->addOption("timer3", QObject::tr("Support for a third timer"));
  firmware->addOption("noheli", QObject::tr("Disable HELI menu and cyclic mix support"));
  firmware->addOption("nogvars", QObject::tr("Disable Global variables"));
  firmware->addOption("lua", QObject::tr("Support for Lua model scripts"));
  Option usb_options[] = { { "massstorage", QObject::tr("Instead of Joystick emulation, USB connection is Mass Storage (as in the Bootloader)") }, { "cli", QObject::tr("Instead of Joystick emulation, USB connection is Command Line Interface") }, { NULL } };
  firmware->addOptions(usb_options);
  firmware->addOption("mixersmon", QObject::tr("Adds mixers output view to the CHANNELS MONITOR screen, pressing [ENT] switches between the views"));
  firmware->addOption("eu", QObject::tr("Removes D8 and LR12 FrSky protocols that are not legal for use in the EU on radios sold after Jan 1st, 2015"));
  firmware->addOption("internalppm", QObject::tr("Support for PPM internal module hack"));
}

void addOpenTxLcdOptions(OpenTxFirmware * firmware)
{
  Option lcd_options[] = {
    { "ST7565P", QObject::tr("ST7565P LCD or compatible") },
    { "ST7565R", QObject::tr("ST7565R LCD or compatible") },
    { "ERC12864FSF", QObject::tr("ERC12864FSF LCD") },
    { "ST7920", QObject::tr("ST7920 LCD") },
    { NULL }
  };
  firmware->addOptions(lcd_options);
}

void registerOpenTxFirmwares()
{
  OpenTxFirmware * firmware;

  Option ext_options[] = { { "frsky", QObject::tr("Support for frsky telemetry mod"), FRSKY_VARIANT }, { "telemetrez", QObject::tr("Support for telemetry easy board"), FRSKY_VARIANT }, { "jeti", QObject::tr("Support for jeti telemetry mod"), 0 }, { "ardupilot", QObject::tr("Support for receiving ardupilot data"), 0 }, { "nmea", QObject::tr("Support for receiving NMEA data"), 0 }, { "mavlink", QObject::tr("Support for MAVLINK devices"), MAVLINK_VARIANT }, { NULL } };
  Option extr_options[] = { { "frsky", QObject::tr("Support for frsky telemetry mod"), FRSKY_VARIANT }, { "jeti", QObject::tr("Support for jeti telemetry mod"), 0 }, { "ardupilot", QObject::tr("Support for receiving ardupilot data"), 0 }, { "nmea", QObject::tr("Support for receiving NMEA data"), 0 }, { "mavlink", QObject::tr("Support for MAVLINK devices"), MAVLINK_VARIANT }, { NULL } };
  Option nav_options[] = { { "rotenc", QObject::tr("Rotary Encoder use in menus navigation") }, { "potscroll", QObject::tr("Pots use in menus navigation") }, { NULL } };
  Option dsm2_options[] = { { "DSM2", QObject::tr("Support for DSM2 modules"), 0 }, { "DSM2PPM", QObject::tr("Support for DSM2 modules using ppm instead of true serial"), 0 }, { NULL } };

  /* 9x board */
  firmware = new OpenTxFirmware("opentx-9x", QObject::tr("9X board"), BOARD_STOCK);
  firmware->addOptions(ext_options);
  firmware->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nosplash", QObject::tr("No splash screen"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  firmware->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED firmware->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  firmware->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOptions(nav_options);
  firmware->addOption("sp22", QObject::tr("SmartieParts 2.2 Backlight support"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  firmware->addOption("pgbar", QObject::tr("EEprom write progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  firmware->addOption("nowshh", QObject::tr("No Winged Shadow How High support"));
  firmware->addOption("novario", QObject::tr("No vario support"));
  firmware->addOption("nogps", QObject::tr("No GPS support"));
  firmware->addOption("nogauges", QObject::tr("No gauges in the custom telemetry screen"));
  firmware->addOption("fasoffset", QObject::tr("Allow compensating for offset errors in FrSky FAS current sensors"));
  firmware->addOption("stickrev", QObject::tr("Add support for reversing stick inputs (e.g. needed for FrSky gimbals)"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* 9x board with M128 chip */
  firmware = new OpenTxFirmware("opentx-9x128", QObject::tr("M128 / 9X board"), BOARD_M128);
  firmware->addOptions(ext_options);
  firmware->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nosplash", QObject::tr("No splash screen"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  firmware->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED firmware->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  firmware->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOptions(nav_options);
  firmware->addOption("sp22", QObject::tr("SmartieParts 2.2 Backlight support"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  firmware->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* 9XR board */
  firmware = new OpenTxFirmware("opentx-9xr", QObject::tr("9XR"), BOARD_STOCK);
  firmware->addOptions(extr_options);
  firmware->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nosplash", QObject::tr("No splash screen"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  firmware->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED firmware->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  firmware->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  firmware->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  firmware->addOption("nowshh", QObject::tr("No Winged Shadow How High support"));
  firmware->addOption("novario", QObject::tr("No vario support"));
  firmware->addOption("nogps", QObject::tr("No GPS support"));
  firmware->addOption("nogauges", QObject::tr("No gauges in the custom telemetry screen"));
  firmware->addOption("stickrev", QObject::tr("Add support for reversing stick inputs (e.g. needed for FrSky gimbals)"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* 9XR board with M128 chip */
  firmware = new OpenTxFirmware("opentx-9xr128", QObject::tr("9XR with M128 chip"), BOARD_M128);
  firmware->addOptions(extr_options);
  firmware->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nosplash", QObject::tr("No splash screen"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  firmware->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  // NOT TESTED firmware->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  firmware->addOption("DSM2", QObject::tr("Support for DSM2 modules"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("thrtrace", QObject::tr("Enable the throttle trace in Statistics"));
  firmware->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* Gruvin9x board */
  firmware = new OpenTxFirmware("opentx-gruvin9x", QObject::tr("Gruvin9x board / 9X"), BOARD_GRUVIN9X);
  firmware->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("sdcard", QObject::tr("Support for SD memory card"));
  firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  firmware->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  firmware->addOptions(dsm2_options);
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* MEGA2560 board */
  firmware = new OpenTxFirmware("opentx-mega2560", QObject::tr("MEGA2560 board"), BOARD_MEGA2560);
  addOpenTxLcdOptions(firmware);
  firmware->addOption("PWR", QObject::tr("Power management by soft-off circuitry"));
  firmware->addOptions(ext_options);
  firmware->addOption("PXX", QObject::tr("Support of FrSky PXX protocol"));
  firmware->addOptions(dsm2_options);
  firmware->addOption("heli", QObject::tr("Enable heli menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("sdcard", QObject::tr("Support for SD memory card"));
//firmware->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
//firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  firmware->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("mixersmon", QObject::tr("Adds mixers output view to the CHANNELS MONITOR screen, pressing [ENT] switches between the views"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* Sky9x board */
  firmware = new OpenTxFirmware("opentx-sky9x", QObject::tr("Sky9x board / 9X"), BOARD_SKY9X);
  firmware->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("timer3", QObject::tr("Support for a third timer"));
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("bluetooth", QObject::tr("Bluetooth interface"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* ar9x board */
  firmware = new OpenTxFirmware("opentx-ar9x", QObject::tr("ar9x board / 9X"), BOARD_AR9X);
  firmware->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("timer3", QObject::tr("Support for a third timer"));
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("bluetooth", QObject::tr("Bluetooth interface"));
//  firmware->addOption("rtc", QObject::tr("Optional RTC added"));
//  firmware->addOption("volume", QObject::tr("i2c volume control added"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* 9XR-Pro */
  firmware = new OpenTxFirmware("opentx-9xrpro", QObject::tr("9XR-PRO"), BOARD_9XRPRO);
  firmware->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("templates", QObject::tr("Enable TEMPLATES menu"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("nocurves", QObject::tr("Disable curves menus"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("timer3", QObject::tr("Support for a third timer"));
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("bluetooth", QObject::tr("Bluetooth interface"));
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* Taranis board */
  firmware = new OpenTxFirmware("opentx-taranis", QObject::tr("FrSky Taranis"), BOARD_TARANIS);
  firmware->addOption("haptic", QObject::tr("Haptic module installed"));
  addOpenTxTaranisOptions(firmware);
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* Taranis Plus board */
  firmware = new OpenTxFirmware("opentx-taranisplus", QObject::tr("FrSky Taranis Plus"), BOARD_TARANIS_PLUS);
  addOpenTxTaranisOptions(firmware);
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  /* Taranis X9E board */
  firmware = new OpenTxFirmware("opentx-taranisx9e", QObject::tr("FrSky Taranis X9E"), BOARD_TARANIS_X9E);
  firmware->addOption("shutdownconfirm", QObject::tr("Confirmation before radio shutdown"));
  addOpenTxTaranisOptions(firmware);
  addOpenTxCommonOptions(firmware);
  firmwares.push_back(firmware);

  default_firmware_variant = GetFirmware("opentx-taranisplus-en");
  current_firmware_variant = default_firmware_variant;
}

