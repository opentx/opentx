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

#ifndef _EEPROMINTERFACE_H_
#define _EEPROMINTERFACE_H_

#include "radiodata.h"
#include "../../radio/src/definitions.h"
#include "simulatorinterface.h"
#include <QStringList>
#include <QList>
#include <iostream>
#include <QDebug>

#define EESIZE_STOCK           2048
#define EESIZE_M128            4096
#define EESIZE_GRUVIN9X        4096
#define EESIZE_TARANIS         (32*1024)
#define EESIZE_SKY9X           (128*4096)
#define EESIZE_9XRPRO          (128*4096)
#define EESIZE_MAX             EESIZE_9XRPRO

#define FSIZE_STOCK            (64*1024)
#define FSIZE_M128             (128*1024)
#define FSIZE_GRUVIN9X         (256*1024)
#define FSIZE_TARANIS          (512*1024)
#define FSIZE_SKY9X            (256*1024)
#define FSIZE_9XRPRO           (512*1024)
#define FSIZE_HORUS            (2048*1024)
#define FSIZE_MAX              FSIZE_HORUS

#define IS_9X(board)           (board==BOARD_STOCK || board==BOARD_M128)
#define IS_STOCK(board)        (board==BOARD_STOCK)
#define IS_2560(board)         (board==BOARD_GRUVIN9X || board==BOARD_MEGA2560)
#define IS_SKY9X(board)        (board==BOARD_SKY9X || board==BOARD_9XRPRO || board==BOARD_AR9X)
#define IS_9XRPRO(board)       (board==BOARD_9XRPRO)
#define IS_TARANIS_X7(board)   (board==BOARD_TARANIS_X7)
#define IS_TARANIS_PLUS(board) (board==BOARD_TARANIS_X9DP || board==BOARD_TARANIS_X9E)
#define IS_TARANIS_X9E(board)  (board==BOARD_TARANIS_X9E)
#define IS_TARANIS(board)      (board==BOARD_TARANIS_X9D  || board==BOARD_TARANIS_X9DP || board==BOARD_TARANIS_X9E || board==BOARD_TARANIS_X7)
#define IS_HORUS(board)        (board==BOARD_HORUS)
#define IS_HORUS_OR_TARANIS(board) (IS_HORUS(board) || IS_TARANIS(board))
#define IS_FLAMENCO(board)     (board==BOARD_FLAMENCO)
#define IS_STM32(board)        (IS_TARANIS(board) || IS_HORUS(board) || IS_FLAMENCO(board))
#define IS_ARM(board)          (IS_STM32(board) || IS_SKY9X(board))
#define HAS_LARGE_LCD(board)   (IS_HORUS(board) || (IS_TARANIS(board) && board != BOARD_TARANIS_X7))

QString RotaryEncoderString(int index);

const uint8_t modn12x3[4][4]= {
  {1, 2, 3, 4},
  {1, 3, 2, 4},
  {4, 2, 3, 1},
  {4, 3, 2, 1} };

enum Capability {
  Models,
  ModelName,
  FlightModes,
  FlightModesName,
  FlightModesHaveFades,
  Imperial,
  Mixes,
  Timers,
  TimersName,
  VoicesAsNumbers,
  VoicesMaxLength,
  MultiLangVoice,
  ModelImage,
  Pots,
  Sliders,
  Switches,
  FactoryInstalledSwitches,
  SwitchesPositions,
  NumTrimSwitches,
  CustomFunctions,
  SafetyChannelCustomFunction,
  LogicalSwitches,
  CustomAndSwitches,
  HasNegAndSwitches,
  LogicalSwitchesExt,
  RotaryEncoders,
  Outputs,
  ChannelsName,
  ExtraInputs,
  ExtendedTrims,
  NumCurves,
  NumCurvePoints,
  OffsetWeight,
  Simulation,
  SoundMod,
  SoundPitch,
  MaxVolume,
  Haptic,
  HasBeeper,
  ModelTrainerEnable,
  HasExpoNames,
  HasNoExpo,
  HasMixerNames,
  HasCvNames,
  HasPxxCountry,
  HasPPMStart,
  HasGeneralUnits,
  HasFAIMode,
  OptrexDisplay,
  PPMExtCtrl,
  PPMFrameLength,
  Telemetry,
  TelemetryUnits,
  TelemetryBars,
  Heli,
  Gvars,
  GvarsInCS,
  GvarsAreNamed,
  GvarsFlightModes,
  GvarsName,
  NoTelemetryProtocol,
  TelemetryCustomScreens,
  TelemetryCustomScreensFieldsPerLine,
  TelemetryMaxMultiplier,
  HasVario,
  HasVarioSink,
  HasFailsafe,
  HasSoundMixer,
  NumModules,
  PPMCenter,
  PPMUnitMicroseconds,
  SYMLimits,
  HastxCurrentCalibration,
  HasVolume,
  HasBrightness,
  PerModelTimers,
  SlowScale,
  SlowRange,
  PermTimers,
  HasSDLogs,
  CSFunc,
  LcdWidth,
  LcdHeight,
  LcdDepth,
  GetThrSwitch,
  HasDisplayText,
  HasTopLcd,
  GlobalFunctions,
  VirtualInputs,
  TrainerInputs,
  RtcTime,
  SportTelemetry,
  LuaScripts,
  LuaInputsPerScript,
  LuaOutputsPerScript,
  LimitsPer1000,
  EnhancedCurves,
  HasFasOffset,
  HasMahPersistent,
  MultiposPots,
  MultiposPotsPositions,
  SimulatorVariant,
  MavlinkTelemetry,
  HasInputDiff,
  HasMixerExpo,
  MixersMonitor,
  HasBatMeterRange,
  DangerousFunctions,
};

class SimulatorInterface;
class EEPROMInterface
{
  public:

    EEPROMInterface(BoardEnum board):
      board(board)
    {
    }

    virtual ~EEPROMInterface() {}

    inline BoardEnum getBoard() { return board; }

    virtual unsigned long load(RadioData &radioData, const uint8_t * eeprom, int size) = 0;

    virtual unsigned long loadBackup(RadioData & radioData, const uint8_t * eeprom, int esize, int index) = 0;

    virtual int save(uint8_t * eeprom, const RadioData & radioData, uint8_t version=0, uint32_t variant=0) = 0;

    virtual int getSize(const ModelData &) = 0;

    virtual int getSize(const GeneralSettings &) = 0;

  protected:

    BoardEnum board;

  private:

    EEPROMInterface();

};

int getEEpromSize(BoardEnum board);

extern std::list<QString> EEPROMWarnings;

/* EEPROM string conversion functions */
void setEEPROMString(char *dst, const char *src, int size);
void getEEPROMString(char *dst, const char *src, int size);

float ValToTim(int value);
int TimToVal(float value);

QString getSignedStr(int value);
QString getGVarString(int16_t val, bool sign=false);

inline int applyStickMode(int stick, unsigned int mode)
{
  if (mode == 0 || mode > 4) {
    std::cerr << "Incorrect stick mode" << mode;
    return stick;
  }

  const unsigned int stickModes[]= {
      1, 2, 3, 4,
      1, 3, 2, 4,
      4, 2, 3, 1,
      4, 3, 2, 1 };

  if (stick >= 1 && stick <= 4)
    return stickModes[(mode-1)*4 + stick - 1];
  else
    return stick;
}

void registerEEpromInterfaces();
void unregisterEEpromInterfaces();
void registerOpenTxFirmwares();
void unregisterOpenTxFirmwares();

enum EepromLoadErrors {
  ALL_OK,
  UNKNOWN_ERROR,
  UNSUPPORTED_NEWER_VERSION,
  WRONG_SIZE,
  WRONG_FILE_SYSTEM,
  NOT_OPENTX,
  NOT_TH9X,
  NOT_GRUVIN9X,
  NOT_ERSKY9X,
  NOT_ER9X,
  UNKNOWN_BOARD,
  WRONG_BOARD,
  BACKUP_NOT_SUPPORTED,

  HAS_WARNINGS,
  OLD_VERSION,
  WARNING_WRONG_FIRMWARE,

  NUM_ERRORS
};

void ShowEepromErrors(QWidget *parent, const QString &title, const QString &mainMessage, unsigned long errorsFound);
void ShowEepromWarnings(QWidget *parent, const QString &title, unsigned long errorsFound);


struct Option {
  const char * name;
  QString tooltip;
  uint32_t variant;
};

class Firmware {

  public:
    Firmware(const QString & id, const QString & name, BoardEnum board):
      id(id),
      name(name),
      board(board),
      variantBase(0),
      base(NULL),
      eepromInterface(NULL)
    {
    }

    Firmware(Firmware * base, const QString & id, const QString & name, BoardEnum board):
      id(id),
      name(name),
      board(board),
      variantBase(0),
      base(base),
      eepromInterface(NULL)
    {
    }

    virtual ~Firmware()
    {
    }

    inline const Firmware * getFirmwareBase() const
    {
      return base ? base : this;
    }
    
    virtual Firmware * getFirmwareVariant(const QString & id) { return NULL; }

    unsigned int getVariantNumber();

    virtual void addLanguage(const char *lang);

    virtual void addTTSLanguage(const char *lang);

    virtual void addOption(const char *option, QString tooltip="", uint32_t variant=0);

    virtual void addOptions(Option options[]);

    virtual QString getStampUrl() = 0;

    virtual QString getReleaseNotesUrl() = 0;

    virtual QString getFirmwareUrl() = 0;

    BoardEnum getBoard() const
    {
      return board;
    }
    
    void setEEpromInterface(EEPROMInterface * eeprom)
    {
      eepromInterface = eeprom;
    }
    
    EEPROMInterface * getEEpromInterface()
    {
      return eepromInterface;
    }

    QString getName() const
    {
      return name;
    }

    QString getId() const
    {
      return id;
    }

    virtual int getCapability(Capability) = 0;

    struct Switch {
      GeneralSettings::SwitchConfig type;
      const char * name;
    };

    virtual Switch getSwitch(unsigned int index) = 0;

    virtual QString getAnalogInputName(unsigned int index) = 0;

    virtual QTime getMaxTimerStart() = 0;

    virtual int isAvailable(PulsesProtocol proto, int port=0) = 0;

    const int getFlashSize();

  public:
    QList<const char *> languages;
    QList<const char *> ttslanguages;
    QList< QList<Option> > opts;

  protected:
    QString id;
    QString name;
    BoardEnum board;
    unsigned int variantBase;
    Firmware * base;
    EEPROMInterface * eepromInterface;

  private:
    Firmware();

};

extern QList<Firmware *> firmwares;
extern Firmware * default_firmware_variant;
extern Firmware * current_firmware_variant;

Firmware * getFirmware(const QString & id);

inline Firmware * getCurrentFirmware()
{
  return current_firmware_variant;
}

SimulatorInterface * getCurrentSimulator();

inline EEPROMInterface * getCurrentEEpromInterface()
{
  return getCurrentFirmware()->getEEpromInterface();
}

inline BoardEnum getCurrentBoard()
{
  return getCurrentFirmware()->getBoard();
}

inline int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}

#define CHECK_IN_ARRAY(T, index) ((unsigned int)index < (unsigned int)(sizeof(T)/sizeof(T[0])) ? T[(unsigned int)index] : "???")

extern QList<EEPROMInterface *> eepromInterfaces;

bool loadFile(RadioData & radioData, const QString & filename);


#endif // _EEPROMINTERFACE_H_
