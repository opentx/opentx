/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentxinterface.h"
#include "opentxeeprom.h"
#include "rlefile.h"
#include "appdata.h"
#include <bitset>
#include <QMessageBox>
#include <QTime>
#include <QUrl>
#include <companion/src/storage/storage.h>

using namespace Board;

#define OPENTX_FIRMWARE_DOWNLOADS        "http://downloads-22.open-tx.org/firmware"
#define OPENTX_NIGHT_FIRMWARE_DOWNLOADS  "http://downloads-22.open-tx.org/nightly/firmware"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)

template<typename T, size_t N>
inline
size_t SizeOfArray(T(&)[N])
{
  return N;
}

OpenTxEepromInterface::OpenTxEepromInterface(OpenTxFirmware * firmware):
  EEPROMInterface(firmware->getBoard()),
  efile(new RleFile()),
  firmware(firmware)
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
    case BOARD_TARANIS_X9D:
      return "OpenTX for FrSky Taranis X9D";
    case BOARD_TARANIS_X9DP:
      return "OpenTX for FrSky Taranis X9D+";
    case BOARD_TARANIS_X9E:
      return "OpenTX for FrSky Taranis X9E";
    case BOARD_TARANIS_X7:
      return "OpenTX for FrSky Taranis X7";
    case BOARD_SKY9X:
      return "OpenTX for Sky9x board / 9X";
    case BOARD_9XRPRO:
      return "OpenTX for 9XR-PRO";
    case BOARD_AR9X:
      return "OpenTX for ar9x board / 9X";
    case BOARD_FLAMENCO:
      return "OpenTX for Flamenco experimental";
    case BOARD_X12S:
      return "OpenTX for FrSky Horus";
    case BOARD_X10:
      return "OpenTX for FrSky X10";
    default:
      return "OpenTX for an unknown board";
  }
}

bool OpenTxEepromInterface::loadRadioSettingsFromRLE(GeneralSettings & settings, RleFile * rleFile, uint8_t version)
{
  QByteArray data(sizeof(settings), 0); // GeneralSettings should be always bigger than the EEPROM struct
  OpenTxGeneralData open9xSettings(settings, board, version);
  efile->openRd(FILE_GENERAL);
  int size = rleFile->readRlc2((uint8_t *)data.data(), data.size());
  if (size) {
    open9xSettings.Import(data);
    return checkVariant(settings.version, settings.variant);
  }
  else {
    std::cout << " error when loading general settings";
    return false;
  }
}

bool OpenTxEepromInterface::loadModelFromRLE(ModelData & model, RleFile * rleFile, unsigned int index, uint8_t version, uint32_t variant)
{
  QByteArray data(sizeof(model), 0);  // ModelData should be always bigger than the EEPROM struct
  rleFile->openRd(FILE_MODEL(index));
  int size = rleFile->readRlc2((uint8_t *)data.data(), data.size());
  if (size) {
    if (loadFromByteArray<ModelData, OpenTxModelData>(model, data, version, variant)) {
      model.used = true;
    }
  }
  else {
    model.clear();
  }
  return true;
}

template <class T, class M>
bool OpenTxEepromInterface::saveToByteArray(const T & src, QByteArray & data, uint8_t version)
{
  if (version == 0) {
    version = getLastDataVersion(getBoard());
  }
  QByteArray raw;
  T srcCopy(src); // work on a copy of radio data, because Export() will modify it!
  M manager(srcCopy, board, version, 0);
  // manager.Dump();
  manager.Export(raw);
  data.resize(8);
  *((uint32_t*)&data.data()[0]) = StorageFormat::getFourCC(board);
  data[4] = version;
  data[5] = 'M';
  *((uint16_t*)&data.data()[6]) = raw.size();
  data.append(raw);
  return true;
}

template <class T, class M>
bool OpenTxEepromInterface::loadFromByteArray(T & dest, const QByteArray & data, uint8_t version, uint32_t variant)
{
  M manager(dest, board, version, variant);
  if (manager.Import(data) != 0) {
    return false;
  }
  // manager.Dump(); // Dumps the structure so that it's easy to check with firmware datastructs.h
  return true;
}

template <class T, class M>
bool OpenTxEepromInterface::loadFromByteArray(T & dest, const QByteArray & data)
{
  uint32_t fourcc = *((uint32_t*)&data.data()[0]);
  if (StorageFormat::getFourCC(board) != fourcc) {
    if (IS_HORUS(board) && fourcc == 0x3178396F) {
      qDebug() << QString().sprintf("%s: Deprecated fourcc used %x vs %x", getName(), fourcc, StorageFormat::getFourCC(board));
    }
    else {
      qDebug() << QString().sprintf("%s: Wrong fourcc %x vs %x", getName(), fourcc, StorageFormat::getFourCC(board));
      return false;
    }
  }
  qDebug() << QString().sprintf("%s: OK", getName());
  uint8_t version = data[4];
  QByteArray raw = data.right(data.size() - 8);
  return loadFromByteArray<T, M>(dest, raw, version);
}

unsigned long OpenTxEepromInterface::load(RadioData &radioData, const uint8_t * eeprom, int size)
{
  std::cout << "trying " << getName() << " import...";

  std::bitset<NUM_ERRORS> errors;

  if (size != Boards::getEEpromSize(board)) {
    if (size == 4096) {
      int notnull = false;
      for (int i = 2048; i < 4096; i++) {
        if (eeprom[i] != 255) {
          notnull = true;
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
        size = 2048;
      }
    }
    else {
      std::cout << " wrong size (" << size << "/" << Boards::getEEpromSize(board) << ")\n";
      errors.set(WRONG_SIZE);
      return errors.to_ulong();
    }
  }

  if (!efile->EeFsOpen((uint8_t *) eeprom, size, board)) {
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

  std::cout << " version " << (unsigned int) version;

  EepromLoadErrors version_error = checkVersion(version);
  if (version_error == OLD_VERSION) {
    errors.set(version_error);
    errors.set(HAS_WARNINGS);
    ShowEepromWarnings(NULL, QObject::tr("Warning"), errors.to_ulong());
  }
  else if (version_error == NOT_OPENTX) {
    std::cout << " not open9x\n";
    errors.set(version_error);
    return errors.to_ulong();
  }

  if (!loadRadioSettingsFromRLE(radioData.generalSettings, efile, version)) {
    std::cout << " ko\n";
    errors.set(UNKNOWN_ERROR);
    return errors.to_ulong();
  }

  std::cout << " variant " << radioData.generalSettings.variant;
  if (getCurrentFirmware()->getCapability(Models) == 0) {
    radioData.models.resize(firmware->getCapability(Models));
  }
  for (int i = 0; i < firmware->getCapability(Models); i++) {
    if (i < (int)radioData.models.size() && !loadModelFromRLE(radioData.models[i], efile, i, version, radioData.generalSettings.variant)) {
      std::cout << " ko\n";
      errors.set(UNKNOWN_ERROR);
      if (getCurrentFirmware()->getCapability(Models) == 0) {
        radioData.models.resize(i);
      }
      return errors.to_ulong();
    }
  }
  std::cout << " ok\n";
  errors.set(ALL_OK);
  return errors.to_ulong();
}

uint8_t OpenTxEepromInterface::getLastDataVersion(Board::Type board)
{
  switch (board) {
    case BOARD_STOCK:
      return 216;
    case BOARD_GRUVIN9X:
    case BOARD_MEGA2560:
    case BOARD_M128:
      return 217;
    default:
      return 218;
  }
}

void OpenTxEepromInterface::showErrors(const QString & title, const QStringList & errors)
{
  QString msg;
  if (errors.empty()) {
    msg = QObject::tr("Unknown error");
  }
  else {
    int noErrorsToDisplay = std::min((int)errors.size(), 10);
    for (int n = 0; n < noErrorsToDisplay; n++) {
      msg += " -" + errors.at(n) + "\n";
    }
    if (noErrorsToDisplay < errors.size()) {
      msg += QObject::tr(" ... plus %1 errors").arg(errors.size() - noErrorsToDisplay) + "\n" + msg;
    }
  }

  QMessageBox::warning(NULL,
                       QObject::tr("Error"),
                       title + "\n" + msg);
}

int OpenTxEepromInterface::save(uint8_t * eeprom, const RadioData & radioData, uint8_t version, uint32_t variant)
{
  // TODO QMessageBox::warning should not be called here

  if (version == 0) {
    version = getLastDataVersion(board);
  }

  int size = Boards::getEEpromSize(board);

  efile->EeFsCreate(eeprom, size, board, version);

  if (board == BOARD_M128) {
    variant |= M128_VARIANT;
  }
  else if (IS_TARANIS_X9E(board)) {
    variant |= TARANIS_X9E_VARIANT;
  }
  else if (IS_TARANIS_X7(board)) {
    variant |= TARANIS_X7_VARIANT;
  }

  OpenTxGeneralData generator((GeneralSettings &)radioData.generalSettings, board, version, variant);
  // generator.Dump();
  QByteArray data;
  generator.Export(data);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (const uint8_t *)data.constData(), data.size());
  if (sz == 0 || generator.errors().count() > 0) {
    showErrors(QObject::tr("Cannot write radio settings"), generator.errors());
    return 0;
  }

  for (int i = 0; i < getCurrentFirmware()->getCapability(Models); i++) {
    if (!radioData.models[i].isEmpty()) {
      OpenTxModelData generator((ModelData &)radioData.models[i], board, version, variant);
      // generator.Dump();
      QByteArray data;
      generator.Export(data);
      int sz = efile->writeRlc2(FILE_MODEL(i), FILE_TYP_MODEL, (const uint8_t *)data.constData(), data.size());
      if (sz == 0 || generator.errors().count() > 0) {
        showErrors(QObject::tr("Cannot write model %1").arg(radioData.models[i].name), generator.errors());
        return 0;
      }
    }
  }

  return size;
}

int OpenTxEepromInterface::getSize(const ModelData &model)
{
  if (IS_SKY9X(board))
    return 0;

  if (model.isEmpty())
    return 0;

  QByteArray tmp(Boards::getEEpromSize(Board::BOARD_UNKNOWN), 0);
  efile->EeFsCreate((uint8_t *) tmp.data(), Boards::getEEpromSize(Board::BOARD_UNKNOWN), board, 255/*version max*/);

  OpenTxModelData open9xModel((ModelData &) model, board, 255/*version max*/, getCurrentFirmware()->getVariantNumber());

  QByteArray eeprom;
  open9xModel.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_MODEL, (const uint8_t *) eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

int OpenTxEepromInterface::getSize(const GeneralSettings &settings)
{
  if (IS_SKY9X(board))
    return 0;

  QByteArray tmp(Boards::getEEpromSize(Board::BOARD_UNKNOWN), 0);
  efile->EeFsCreate((uint8_t *) tmp.data(), Boards::getEEpromSize(Board::BOARD_UNKNOWN), board, 255);

  OpenTxGeneralData open9xGeneral((GeneralSettings &) settings, board, 255, getCurrentFirmware()->getVariantNumber());
  // open9xGeneral.Dump();

  QByteArray eeprom;
  open9xGeneral.Export(eeprom);
  int sz = efile->writeRlc2(0, FILE_TYP_GENERAL, (const uint8_t *) eeprom.constData(), eeprom.size());
  if (sz != eeprom.size()) {
    return -1;
  }
  return efile->size(0);
}

Firmware * OpenTxFirmware::getFirmwareVariant(const QString &id)
{
  if (id == getId()) {
    return this;
  }
  else if (id.contains(getId() + "-") || (!id.contains("-") && id.contains(getId()))) {
    Firmware * result = new OpenTxFirmware(id, this);
    // TODO result.variant = firmware->getVariant(id);
    return result;
  }
  else {
    return NULL;
  }
}

int OpenTxFirmware::getCapability(::Capability capability)
{
  switch (capability) {
    case Models:
      if (IS_HORUS(board))
        return 0;
      else if (IS_ARM(board))
        return 60;
      else if (board == BOARD_M128)
        return 30;
      else if (IS_2560(board))
        return 30;
      else
        return 16;
    case Imperial:
      if (IS_ARM(board))
        return 0;
      else
        return id.contains("imperial") ? 1 : 0;
    case ModelImage:
      return (board == BOARD_TARANIS_X9D || IS_TARANIS_PLUS(board) || IS_HORUS(board));
    case HasBeeper:
      return (!IS_ARM(board));
    case HasPxxCountry:
      return 1;
    case HasGeneralUnits:
      if (IS_ARM(board))
        return 1;
      else
        return 0;
    case HasNegAndSwitches:
      return IS_ARM(board);
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
      if (IS_HORUS_OR_TARANIS(board))
        return id.contains("noheli") ? 0 : 1;
      else
        return id.contains("heli") ? 1 : 0;
    case Gvars:
      if (IS_HORUS_OR_TARANIS(board))
        return id.contains("nogvars") ? 0 : 9;
      else if (id.contains("gvars"))
        return IS_ARM(board) ? 9 : 5;
      else
        return 0;
    case ModelName:
      return (IS_HORUS(board) ? 15 : (HAS_LARGE_LCD(board) ? 12 : 10));
    case FlightModesName:
      return (IS_HORUS_OR_TARANIS(board) ? 10 : 6);
    case GvarsName:
      return (IS_9X(board) ? 0 : 3);
    case GvarsInCS:
    case HasFAIMode:
      return 1;
    case GvarsAreNamed:
    case GvarsFlightModes:
      return ((IS_ARM(board) || IS_2560(board)) ? 1 : 0);
    case Mixes:
      return (IS_ARM(board) ? 64 : 32);
    case OffsetWeight:
      return (IS_ARM(board) ? 500 : 245);
    case Timers:
      return (IS_ARM(board) ? 3 : 2);
    case TimersName:
      if (HAS_LARGE_LCD(board))
        return 8;
      else if (IS_ARM(board))
        return 3;
      else
        return 0;
    case PermTimers:
      return (IS_2560(board) || IS_ARM(board));
    case CustomFunctions:
      if (IS_ARM(board))
        return 64;
      else if (IS_2560(board) || board == BOARD_M128)
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
        return 15/*4bits*/- 9/*sw positions*/;
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
      return (IS_ARM(board) ? (IS_TARANIS_X9(board) ? 8 : 6) : 0);
    case MultiLangVoice:
      return (IS_ARM(board) ? 1 : 0);
    case SoundPitch:
      return 1;
    case Haptic:
      return (IS_2560(board) || IS_SKY9X(board) || IS_TARANIS_PLUS(board) || IS_TARANIS_X7(board) || IS_TARANIS_X9E(board) || IS_HORUS(board) || id.contains("haptic"));
    case ModelTrainerEnable:
      if (IS_HORUS_OR_TARANIS(board))
        return 1;
      else
        return 0;
    case MaxVolume:
      return (IS_ARM(board) ? 23 : 7);
    case HasSoundMixer:
      return (IS_ARM(board) ? 1 : 0);
    case ExtraInputs:
      return 1;
    case TrimsRange:
      return 125;
    case ExtendedTrimsRange:
      return 500;
    case Simulation:
      return 1;
    case NumCurves:
      return (HAS_LARGE_LCD(board) ? 32 : (IS_ARM(board) ? 16 : 8));
    case HasMixerNames:
      return (IS_ARM(board) ? (IS_TARANIS_X9(board) ? 8 : 6) : false);
    case HasExpoNames:
      return (IS_ARM(board) ? (IS_TARANIS_X9(board) ? 8 : 6) : false);
    case HasNoExpo:
      return (IS_HORUS_OR_TARANIS(board) ? false : true);
    case ChannelsName:
      return (IS_ARM(board) ? (HAS_LARGE_LCD(board) ? 6 : 4) : 0);
    case HasCvNames:
      return (IS_HORUS_OR_TARANIS(board) ? 1 : 0);
    case Telemetry:
      if (IS_2560(board) || IS_ARM(board) || id.contains("frsky") || id.contains("telemetrez"))
        return TM_HASTELEMETRY | TM_HASOFFSET | TM_HASWSHH;
      else
        return 0;
    case TelemetryBars:
      return 1;
    case TelemetryCustomScreens:
      if (IS_HORUS(board))
        return 0;
      else
        return IS_ARM(board) ? 4 : 2;
    case TelemetryCustomScreensFieldsPerLine:
      return HAS_LARGE_LCD(board) ? 3 : 2;
    case NoTelemetryProtocol:
      return IS_HORUS_OR_TARANIS(board) ? 1 : 0;
    case TelemetryUnits:
      return 0;
    case TelemetryMaxMultiplier:
      return (IS_ARM(board) ? 32 : 8);
    case PPMCenter:
      return (IS_HORUS_OR_TARANIS(board) ? 500 : (id.contains("ppmca") ? 125 : 0));
    case PPMUnitMicroseconds:
      return id.contains("ppmus") ? 1 : 0;
    case SYMLimits:
      return 1;
    case OptrexDisplay:
      return (board == BOARD_SKY9X ? true : false);
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
    case LcdWidth:
      if (IS_HORUS(board))
        return 480;
      else if (IS_TARANIS_X7(board))
        return 128;
      else if (IS_TARANIS(board))
        return 212;
      else
        return 128;
    case LcdHeight:
      if (IS_HORUS(board))
        return 272;
      else
        return 64;
    case LcdDepth:
      if (IS_HORUS(board))
        return 16;
      else if (IS_TARANIS_X7(board))
        return 1;
      else if (IS_TARANIS(board))
        return 4;
      else
        return 1;
    case GetThrSwitch:
      return (IS_HORUS_OR_TARANIS(board) ? SWITCH_SF1 : SWITCH_THR);
    case HasDisplayText:
      return IS_ARM(board) ? 1 : 0;
    case HasTopLcd:
      return IS_TARANIS_X9E(board) ? 1 : 0;
    case GlobalFunctions:
      return IS_ARM(board) ? 64 : 0;
    case VirtualInputs:
      return IS_ARM(board) ? 32 : 0;
    case InputsLength:
      return HAS_LARGE_LCD(board) ? 4 : 3;
    case TrainerInputs:
      return IS_ARM(board) ? 16 : 8;
    case RtcTime:
      return IS_ARM(board) || IS_2560(board) ? 1 : 0;
    case LuaScripts:
      return IS_HORUS_OR_TARANIS(board) && id.contains("lua") ? 7 : 0;
    case LuaInputsPerScript:
      return IS_HORUS_OR_TARANIS(board) ? 10 : 0;
    case LuaOutputsPerScript:
      return IS_HORUS_OR_TARANIS(board) ? 6 : 0;
    case LimitsPer1000:
    case EnhancedCurves:
      return IS_ARM(board);
    case HasFasOffset:
      return (IS_STOCK(board) ? false : true);
    case HasMahPersistent:
      return (IS_ARM(board) ? true : false);
    case MultiposPots:
      return IS_HORUS_OR_TARANIS(board) ? 3 : 0;
    case MultiposPotsPositions:
      return IS_HORUS_OR_TARANIS(board) ? 6 : 0;
    case SimulatorVariant:
      if (board == BOARD_STOCK)
        return SIMU_STOCK_VARIANTS;
      else if (board == BOARD_M128)
        return SIMU_M128_VARIANTS;
      else if (IS_TARANIS_X9E(board))
        return TARANIS_X9E_VARIANT;
      else if (IS_TARANIS_X7(board))
        return TARANIS_X7_VARIANT;
      else
        return 0;
    case MavlinkTelemetry:
      return id.contains("mavlink") ? 1 : 0;
    case SportTelemetry:
      return IS_ARM(board) ? 1 : 0;
    case HasInputDiff:
    case HasMixerExpo:
      return (IS_HORUS_OR_TARANIS(board) ? true : false);
    case HasBatMeterRange:
      return (IS_HORUS_OR_TARANIS(board) ? true : id.contains("battgraph"));
    case DangerousFunctions:
      return id.contains("danger") ? 1 : 0;
    case HasModelCategories:
      return IS_HORUS(board);
    default:
      return 0;
  }
}

QString OpenTxFirmware::getAnalogInputName(unsigned int index)
{
  if ((int)index < getBoardCapability(board, Board::Sticks)) {
    const QString sticks[] = {
      QObject::tr("Rud"),
      QObject::tr("Ele"),
      QObject::tr("Thr"),
      QObject::tr("Ail")
    };
    return sticks[index];
  }

  index -= getBoardCapability(board, Board::Sticks);

  if (IS_9X(board) || IS_2560(board) || IS_SKY9X(board)) {
    const QString pots[] = {
      QObject::tr("P1"),
      QObject::tr("P2"),
      QObject::tr("P3")
    };
    return CHECK_IN_ARRAY(pots, index);
  }
  else if (IS_TARANIS_X9E(board)) {
    const QString pots[] = {
      QObject::tr("F1"),
      QObject::tr("F2"),
      QObject::tr("F3"),
      QObject::tr("F4"),
      QObject::tr("S1"),
      QObject::tr("S2"),
      QObject::tr("LS"),
      QObject::tr("RS")
    };
    return CHECK_IN_ARRAY(pots, index);
  }
  else if (IS_TARANIS(board)) {
    const QString pots[] = {
      QObject::tr("S1"),
      QObject::tr("S2"),
      QObject::tr("S3"),
      QObject::tr("LS"),
      QObject::tr("RS")
    };
    return CHECK_IN_ARRAY(pots, index);
  }
  else if (IS_HORUS(board)) {
    const QString pots[] = {
      QObject::tr("S1"),
      QObject::tr("6P"),
      QObject::tr("S2"),
      QObject::tr("L1"),
      QObject::tr("L2"),
      QObject::tr("LS"),
      QObject::tr("RS"),
      QObject::tr("JSx"),
      QObject::tr("JSy")
    };
    return CHECK_IN_ARRAY(pots, index);
  }
  else {
    return "???";
  }
}

QTime OpenTxFirmware::getMaxTimerStart()
{
  if (IS_HORUS_OR_TARANIS(board))
    return QTime(23, 59, 59);
  else if (IS_ARM(board))
    return QTime(8, 59, 59);
  else
    return QTime(0, 59, 59);
}

int OpenTxFirmware::isAvailable(PulsesProtocol proto, int port)
{
  if (IS_HORUS_OR_TARANIS(board)) {
    switch (port) {
      case 0:
        switch (proto) {
          case PULSES_OFF:
          case PULSES_PXX_XJT_X16:
          case PULSES_PXX_XJT_LR12:
            return 1;
          case PULSES_PXX_XJT_D8:
            return id.contains("eu") ? 0 : 1;
          case PULSES_PPM:
            return id.contains("internalppm") ? 1 : 0;
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
          case PULSES_CROSSFIRE:
            return 1;
          case PULSES_MULTIMODULE:
            return id.contains("multimodule") ? 1 : 0;
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
          case PULSES_MULTIMODULE:
            return id.contains("multimodule") ? 1 : 0;
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
        // case PULSES_PXX_DJT:     // Unavailable for now
      case PULSES_PPM16:
      case PULSES_PPMSIM:
        return 1;
      default:
        return 0;
    }
  }
}

template<typename T, size_t SIZE>
size_t getSizeA(T (&)[SIZE])
{
  return SIZE;
}

EepromLoadErrors OpenTxEepromInterface::checkVersion(unsigned int version)
{
  switch (version) {
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

  return ALL_OK;
}

bool OpenTxEepromInterface::checkVariant(unsigned int version, unsigned int variant)
{
  if (board == BOARD_M128 && !(variant & M128_VARIANT)) {
    if (version == 212) {
      uint8_t tmp[1000];
      for (int i = 1; i < 31; i++) {
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
  else if (IS_TARANIS_X9E(board)) {
    if (variant != TARANIS_X9E_VARIANT) {
      std::cout << " wrong variant (" << variant << ")";
      return false;
    }
  }
  else if (IS_TARANIS_X7(board)) {
    if (variant != TARANIS_X7_VARIANT) {
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

bool
OpenTxEepromInterface::loadModelFromBackup(ModelData & model, const uint8_t * data, unsigned int size, uint8_t version, uint32_t variant)
{
  QByteArray backupData((char *) data, size);
  QByteArray modelData;
  if (IS_SKY9X(board)) {
    modelData = QByteArray((char *) data, size);
  }
  else {
    importRlc(modelData, backupData);
  }
  if (modelData.size()) {
    if (loadFromByteArray<ModelData, OpenTxModelData>(model, modelData, version, variant)) {
      model.used = true;
      return true;
    }
  }
  model.clear();
  return false;
}

unsigned long OpenTxEepromInterface::loadBackup(RadioData &radioData, const uint8_t * eeprom, int esize, int index)
{
  std::bitset<NUM_ERRORS> errors;

  std::cout << "trying " << getName() << " backup import...";

  if (esize < 8 || memcmp(eeprom, "o9x", 3) != 0) {
    std::cout << " no\n";
    errors.set(WRONG_SIZE);
    return errors.to_ulong();
  }

  Type backupBoard = (Type) -1;
  switch (eeprom[3]) {
    case 0x33:
      backupBoard = BOARD_TARANIS_X9D;
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
  uint16_t size = ((uint16_t) eeprom[7] << 8) + eeprom[6];
  uint16_t variant = ((uint16_t) eeprom[9] << 8) + eeprom[8];

  std::cout << " version " << (unsigned int) version << " ";

  EepromLoadErrors version_error = checkVersion(version);
  if (version_error == OLD_VERSION) {
    errors.set(version_error);
    errors.set(HAS_WARNINGS);
  }
  else if (version_error == NOT_OPENTX) {
    std::cout << " not open9x\n";
    errors.set(version_error);
    return errors.to_ulong();
  }

  if (size > esize - 8) {
    std::cout << " wrong size\n";
    errors.set(WRONG_SIZE);
    return errors.to_ulong();
  }

  if (bcktype == 'M') {
    if (!loadModelFromBackup(radioData.models[index], &eeprom[8], size, version, variant)) {
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
  errors.set(ALL_OK);
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
  QByteArray data = QUrl::toPercentEncoding(id);

  if (IS_ARM(board))
    url.append(QString("/getfw.php?fw=%1.bin").arg((QString) data));
  else
    url.append(QString("/getfw.php?fw=%1.hex").arg((QString) data));
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
  firmware->addOption("nooverridech", QObject::tr("No OverrideCH functions available"));
  Option fai_options[] = {{"faichoice", QObject::tr("Possibility to enable FAI MODE (no telemetry) at field")},
                          {"faimode",   QObject::tr("FAI MODE (no telemetry) always enabled")},
                          {NULL}};
  firmware->addOptions(fai_options);
}

void addOpenTxArmOptions(OpenTxFirmware * firmware)
{
  firmware->addOption("multimodule", QObject::tr("Support for the DIY-Multiprotocol-TX-Module"));
  firmware->addOption("eu", QObject::tr("Removes D8 FrSky protocol support which is not legal for use in the EU on radios sold after Jan 1st, 2015"));
}

void addOpenTxFrskyOptions(OpenTxFirmware * firmware)
{
  addOpenTxCommonOptions(firmware);
  addOpenTxArmOptions(firmware);
  firmware->addOption("noheli", QObject::tr("Disable HELI menu and cyclic mix support"));
  firmware->addOption("nogvars", QObject::tr("Disable Global variables"));
  firmware->addOption("lua", QObject::tr("Support for Lua model scripts"));
  firmware->addOption("luac", QObject::tr("Enable Lua compiler"));
  firmware->addOption("bindopt", QObject::tr("Enable bindings options"));
  Option usb_options[] = {{"massstorage", QObject::tr("Instead of Joystick emulation, USB connection is Mass Storage (as in the Bootloader)")},
                          {"cli",         QObject::tr("Instead of Joystick emulation, USB connection is Command Line Interface")},
                          {NULL}};
  firmware->addOptions(usb_options);
}

void addOpenTxTaranisOptions(OpenTxFirmware * firmware)
{
  addOpenTxFrskyOptions(firmware);
  firmware->addOption("internalppm", QObject::tr("Support for PPM internal module hack"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
}

void addOpenTxLcdOptions(OpenTxFirmware * firmware)
{
  Option lcd_options[] = {
    {"ST7565P",     QObject::tr("ST7565P LCD or compatible")},
    {"ST7565R",     QObject::tr("ST7565R LCD or compatible")},
    {"ERC12864FSF", QObject::tr("ERC12864FSF LCD")},
    {"ST7920",      QObject::tr("ST7920 LCD")},
    {"KS108",       QObject::tr("KS108 LCD")},
    {NULL}
  };
  firmware->addOptions(lcd_options);
}

void addOpenTxVoiceOptions(OpenTxFirmware * firmware)
{
  Option voice_options[] = {
    {"WTV20",     	QObject::tr("WTV20 voice module")},
    {"JQ6500", 		QObject::tr("JQ6500 voice module")},
    {NULL}
  };
  firmware->addOptions(voice_options);
}

QList<OpenTxEepromInterface *> opentxEEpromInterfaces;

void registerOpenTxFirmware(OpenTxFirmware * firmware)
{
  OpenTxEepromInterface * eepromInterface = new OpenTxEepromInterface(firmware);
  firmware->setEEpromInterface(eepromInterface);
  opentxEEpromInterfaces.push_back(eepromInterface);
  eepromInterfaces.push_back(eepromInterface);
  Firmware::addRegisteredFirmware(firmware);
}

void registerOpenTxFirmwares()
{
  OpenTxFirmware * firmware;

  Option ext_options[] = {{"frsky",      QObject::tr("Support for frsky telemetry mod"),  FRSKY_VARIANT},
                          {"telemetrez", QObject::tr("Support for telemetry easy board"), FRSKY_VARIANT},
                          {"jeti",       QObject::tr("Support for jeti telemetry mod"),       0},
                          {"ardupilot",  QObject::tr("Support for receiving ardupilot data"), 0},
                          {"nmea",       QObject::tr("Support for receiving NMEA data"),      0},
                          {"mavlink",    QObject::tr("Support for MAVLINK devices"),      MAVLINK_VARIANT},
                          {NULL}};
  Option extr_options[] = {{"frsky",     QObject::tr("Support for frsky telemetry mod"), FRSKY_VARIANT},
                           {"jeti",      QObject::tr("Support for jeti telemetry mod"),       0},
                           {"ardupilot", QObject::tr("Support for receiving ardupilot data"), 0},
                           {"nmea",      QObject::tr("Support for receiving NMEA data"),      0},
                           {"mavlink",   QObject::tr("Support for MAVLINK devices"),     MAVLINK_VARIANT},
                           {NULL}};
  Option nav_options[] = {{"rotenc",    QObject::tr("Rotary Encoder use in menus navigation")},
                          {"potscroll", QObject::tr("Pots use in menus navigation")},
                          {NULL}};
  Option dsm2_options[] = {{"DSM2",    QObject::tr("Support for DSM2 modules"),                                  0},
                           {"DSM2PPM", QObject::tr("Support for DSM2 modules using ppm instead of true serial"), 0},
                           {NULL}};

  /* FrSky Taranis X9D+ board */
  firmware = new OpenTxFirmware("opentx-x9d+", QObject::tr("FrSky Taranis X9D+"), BOARD_TARANIS_X9DP);
  addOpenTxTaranisOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky Taranis X9D board */
  firmware = new OpenTxFirmware("opentx-x9d", QObject::tr("FrSky Taranis X9D"), BOARD_TARANIS_X9D);
  firmware->addOption("haptic", QObject::tr("Haptic module installed"));
  addOpenTxTaranisOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky Taranis X9E board */
  firmware = new OpenTxFirmware("opentx-x9e", QObject::tr("FrSky Taranis X9E"), BOARD_TARANIS_X9E);
  firmware->addOption("shutdownconfirm", QObject::tr("Confirmation before radio shutdown"));
  firmware->addOption("horussticks", QObject::tr("Horus gimbals installed (Hall sensors)"));
  addOpenTxTaranisOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* FrSky X7 board */
  firmware = new OpenTxFirmware("opentx-x7", QObject::tr("FrSky Taranis X7"), BOARD_TARANIS_X7);
  // No mixersmon for now
  addOpenTxFrskyOptions(firmware);
  firmware->addOption("internalppm", QObject::tr("Support for PPM internal module hack"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  registerOpenTxFirmware(firmware);

  /* FrSky Horus board */
  firmware = new OpenTxFirmware("opentx-x12s", QObject::tr("FrSky Horus"), BOARD_X12S);
  addOpenTxFrskyOptions(firmware);
  firmware->addOption("pcbdev", QObject::tr("Use ONLY with first DEV pcb version"));
  registerOpenTxFirmware(firmware);


  /* FrSky X10 board */
/* Disabled for now
  firmware = new OpenTxFirmware("opentx-x10", QObject::tr("FrSky X10"), BOARD_X10);
  addOpenTxFrskyOptions(firmware);
  registerOpenTxFirmware(firmware);
*/
  /* 9XR-Pro */
  firmware = new OpenTxFirmware("opentx-9xrpro", QObject::tr("Turnigy 9XR-PRO"), BOARD_9XRPRO);
  firmware->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("bluetooth", QObject::tr("Bluetooth interface"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxArmOptions(firmware);
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* 9XR board with M128 chip */
  firmware = new OpenTxFirmware("opentx-9xr128", QObject::tr("Turnigy 9XR with m128 chip"), BOARD_M128);
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
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* 9XR board */
  firmware = new OpenTxFirmware("opentx-9xr", QObject::tr("Turnigy 9XR"), BOARD_STOCK);
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
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* 9x board */
  firmware = new OpenTxFirmware("opentx-9x", QObject::tr("9X with stock board"), BOARD_STOCK);
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
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* 9x board with M128 chip */
  firmware = new OpenTxFirmware("opentx-9x128", QObject::tr("9X with stock board and m128 chip"), BOARD_M128);
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
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* ar9x board */
  firmware = new OpenTxFirmware("opentx-ar9x", QObject::tr("9X with AR9X board"), BOARD_AR9X);
  firmware->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("bluetooth", QObject::tr("Bluetooth interface"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
//  firmware->addOption("rtc", QObject::tr("Optional RTC added"));
//  firmware->addOption("volume", QObject::tr("i2c volume control added"));
  addOpenTxArmOptions(firmware);
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* Sky9x board */
  firmware = new OpenTxFirmware("opentx-sky9x", QObject::tr("9X with Sky9x board"), BOARD_SKY9X);
  firmware->addOption("heli", QObject::tr("Enable HELI menu and cyclic mix support"));
  firmware->addOption("nofp", QObject::tr("No flight modes"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("potscroll", QObject::tr("Pots use in menus navigation"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("bluetooth", QObject::tr("Bluetooth interface"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxArmOptions(firmware);
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* Gruvin9x board */
  firmware = new OpenTxFirmware("opentx-gruvin9x", QObject::tr("9X with Gruvin9x board"), BOARD_GRUVIN9X);
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
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  /* MEGA2560 board */
  firmware = new OpenTxFirmware("opentx-mega2560", QObject::tr("DIY MEGA2560 radio"), BOARD_MEGA2560);
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
  firmware->addOption("audio", QObject::tr("Support for radio modified with regular speaker"));
  //firmware->addOption("voice", QObject::tr("Used if you have modified your radio with voice mode"));
  addOpenTxVoiceOptions(firmware);
  firmware->addOption("haptic", QObject::tr("Used if you have modified your radio with haptic mode"));
  firmware->addOption("ppmca", QObject::tr("PPM center adjustment in limits"));
  firmware->addOption("gvars", QObject::tr("Global variables"), GVARS_VARIANT);
  firmware->addOption("symlimits", QObject::tr("Symetrical Limits"));
  firmware->addOption("autosource", QObject::tr("In model setup menus automatically set source by moving the control"));
  firmware->addOption("autoswitch", QObject::tr("In model setup menus automatically set switch by moving the control"));
  firmware->addOption("dblkeys", QObject::tr("Enable resetting values by pressing up and down at the same time"));
  firmware->addOption("nographics", QObject::tr("No graphical check boxes and sliders"));
  firmware->addOption("battgraph", QObject::tr("Battery graph"));
  firmware->addOption("nobold", QObject::tr("Don't use bold font for highlighting active items"));
  firmware->addOption("pgbar", QObject::tr("EEprom write Progress bar"));
  firmware->addOption("imperial", QObject::tr("Imperial units"));
  firmware->addOption("sqt5font", QObject::tr("Use alternative SQT5 font"));
  addOpenTxCommonOptions(firmware);
  registerOpenTxFirmware(firmware);

  Firmware::setDefaultVariant(Firmware::getFirmwareForId("opentx-x9d+"));
  Firmware::setCurrentVariant(Firmware::getDefaultVariant());
}

void unregisterOpenTxFirmwares()
{
  foreach (Firmware * f, Firmware::getRegisteredFirmwares()) {
    delete f;
  }
  unregisterEEpromInterfaces();
}

template <class T, class M>
OpenTxEepromInterface * loadFromByteArray(T & dest, const QByteArray & data)
{
  foreach(OpenTxEepromInterface * eepromInterface, opentxEEpromInterfaces) {
    if (eepromInterface->loadFromByteArray<T, M>(dest, data)) {
      return eepromInterface;
    }
  }
  return NULL;
}

template <class T, class M>
bool saveToByteArray(const T & dest, QByteArray & data)
{
  Board::Type board = getCurrentBoard();
  foreach(OpenTxEepromInterface * eepromInterface, opentxEEpromInterfaces) {
    if (eepromInterface->getBoard() == board) {
      return eepromInterface->saveToByteArray<T, M>(dest, data);
    }
  }
  return false;
}

OpenTxEepromInterface * loadModelFromByteArray(ModelData & model, const QByteArray & data)
{
  return loadFromByteArray<ModelData, OpenTxModelData>(model, data);
}

OpenTxEepromInterface * loadRadioSettingsFromByteArray(GeneralSettings & settings, const QByteArray & data)
{
  return loadFromByteArray<GeneralSettings, OpenTxGeneralData>(settings, data);
}

bool writeModelToByteArray(const ModelData & model, QByteArray & data)
{
  return saveToByteArray<ModelData, OpenTxModelData>(model, data);
}

bool writeRadioSettingsToByteArray(const GeneralSettings & settings, QByteArray & data)
{
  return saveToByteArray<GeneralSettings, OpenTxGeneralData>(settings, data);
}
