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

#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include "boards.h"
#include "constants.h"
#include "customfunctiondata.h"
#include "rawsource.h"

#include <QtCore>

class Firmware;
class ModelData;
class GeneralSettings;
class RadioDataConversionState;

enum UartModes {
  UART_MODE_NONE,
  UART_MODE_TELEMETRY_MIRROR,
  UART_MODE_TELEMETRY,
  UART_MODE_SBUS_TRAINER,
  UART_MODE_DEBUG
};

class TrainerMix {
  public:
    TrainerMix() { clear(); }
    unsigned int src; // 0-7 = ch1-8
    RawSwitch swtch;
    int weight;
    unsigned int mode;   // off, add-mode, subst-mode
    void clear() { memset(this, 0, sizeof(TrainerMix)); }
};

class TrainerData {
  public:
    TrainerData() { clear(); }
    int         calib[4];
    TrainerMix  mix[4];
    void clear() { memset(this, 0, sizeof(TrainerData)); }
};

class GeneralSettings {
  Q_DECLARE_TR_FUNCTIONS(GeneralSettings)

  public:

    enum BeeperMode {
      BEEPER_QUIET = -2,
      BEEPER_ALARMS_ONLY = -1,
      BEEPER_NOKEYS = 0,
      BEEPER_ALL = 1
    };

    GeneralSettings();
    void convert(RadioDataConversionState & cstate);

    void setDefaultControlTypes(Board::Type board);
    int getDefaultStick(unsigned int channel) const;
    RawSource getDefaultSource(unsigned int channel) const;
    int getDefaultChannel(unsigned int stick) const;

    unsigned int version;
    unsigned int variant;
    int   calibMid[CPN_MAX_ANALOGS];
    int   calibSpanNeg[CPN_MAX_ANALOGS];
    int   calibSpanPos[CPN_MAX_ANALOGS];
    unsigned int  currModelIndex;
    char currModelFilename[16+1];
    unsigned int   contrast;
    unsigned int   vBatWarn;
    int    txVoltageCalibration;
    int    txCurrentCalibration;
    int    vBatMin;
    int    vBatMax;
    int   backlightMode;
    TrainerData trainer;
    unsigned int   view;    // main screen view // TODO enum
    bool      disableThrottleWarning;
    bool      fai;
    bool      disableMemoryWarning;
    BeeperMode beeperMode;
    bool      disableAlarmWarning;
    bool      disableRssiPoweroffAlarm;
    unsigned int       usbMode;
    BeeperMode hapticMode;
    unsigned int   stickMode; // TODO enum
    int       timezone;
    bool      adjustRTC;
    bool      optrexDisplay;
    unsigned int    inactivityTimer;
    bool      minuteBeep;
    bool      preBeep;
    bool      flashBeep;
    unsigned int  splashMode;
    int splashDuration;
    unsigned int  backlightDelay;
    unsigned int   templateSetup;  //RETA order according to chout_ar array
    int    PPM_Multiplier;
    int    hapticLength;
    unsigned int   reNavigation;
    unsigned int stickReverse;
    unsigned int   speakerPitch;
    int   hapticStrength;
    unsigned int   speakerMode;
    char      ownerName[10+1];
    int    beeperLength;
    unsigned int    gpsFormat;
    int     speakerVolume;
    unsigned int   backlightBright;
    unsigned int   backlightOffBright;
    int switchesDelay;
    int    temperatureCalib;
    int    temperatureWarn;
    unsigned int mAhWarn;
    unsigned int mAhUsed;
    unsigned int globalTimer;
    char bluetoothName[10+1];
    unsigned int bluetoothBaudrate;
    unsigned int bluetoothMode;
    unsigned int sticksGain;
    unsigned int rotarySteps;
    unsigned int countryCode;
    bool jitterFilter;
    unsigned int imperial;
    char ttsLanguage[2+1];
    int beepVolume;
    int wavVolume;
    int varioVolume;
    int varioPitch;
    int varioRange;
    int varioRepeat;
    int backgroundVolume;
    unsigned int mavbaud;
    unsigned int switchUnlockStates;
    unsigned int hw_uartMode;  // UartModes
    unsigned int backlightColor;
    CustomFunctionData customFn[CPN_MAX_SPECIAL_FUNCTIONS];
    char switchName[CPN_MAX_SWITCHES][3+1];
    unsigned int switchConfig[CPN_MAX_SWITCHES];
    char stickName[CPN_MAX_STICKS][3+1];
    char potName[CPN_MAX_KNOBS][3+1];
    unsigned int potConfig[CPN_MAX_KNOBS];
    char sliderName[CPN_MAX_SLIDERS][3+1];
    unsigned int sliderConfig[CPN_MAX_SLIDERS];

    char themeName[8+1];
    typedef uint8_t ThemeOptionData[8+1];
    ThemeOptionData themeOptionValue[5];

    bool switchPositionAllowedTaranis(int index) const;
    bool switchSourceAllowedTaranis(int index) const;
    bool isPotAvailable(int index) const;
    bool isSliderAvailable(int index) const;
};


#endif // GENERALSETTINGS_H
