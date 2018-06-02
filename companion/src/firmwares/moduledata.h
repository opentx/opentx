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

#ifndef MODULEDATA_H
#define MODULEDATA_H

#include "constants.h"

#include <QtCore>

class RadioDataConversionState;

enum PulsesProtocol {
  PULSES_OFF,
  PULSES_PPM,
  PULSES_SILV_A,
  PULSES_SILV_B,
  PULSES_SILV_C,
  PULSES_CTP1009,
  PULSES_LP45,
  PULSES_DSM2,
  PULSES_DSMX,
  PULSES_PPM16,
  PULSES_PPMSIM,
  PULSES_PXX_XJT_X16,
  PULSES_PXX_XJT_D8,
  PULSES_PXX_XJT_LR12,
  PULSES_PXX_DJT,
  PULSES_CROSSFIRE,
  PULSES_MULTIMODULE,
  PULSES_PXX_R9M,
  PULSES_SBUS,
  PULSES_PROTOCOL_LAST
};

enum MultiModuleRFProtocols {
  MM_RF_PROTO_FLYSKY=0,
  MM_RF_PROTO_FIRST=MM_RF_PROTO_FLYSKY,
  MM_RF_PROTO_HUBSAN,
  MM_RF_PROTO_FRSKY,
  MM_RF_PROTO_HISKY,
  MM_RF_PROTO_V2X2,
  MM_RF_PROTO_DSM2,
  MM_RF_PROTO_DEVO,
  MM_RF_PROTO_YD717,
  MM_RF_PROTO_KN,
  MM_RF_PROTO_SYMAX,
  MM_RF_PROTO_SLT,
  MM_RF_PROTO_CX10,
  MM_RF_PROTO_CG023,
  MM_RF_PROTO_BAYANG,
  MM_RF_PROTO_ESky,
  MM_RF_PROTO_MT99XX,
  MM_RF_PROTO_MJXQ,
  MM_RF_PROTO_SHENQI,
  MM_RF_PROTO_FY326,
  MM_RF_PROTO_SFHSS,
  MM_RF_PROTO_J6PRO,
  MM_RF_PROTO_FQ777,
  MM_RF_PROTO_ASSAN,
  MM_RF_PROTO_HONTAI,
  MM_RF_PROTO_OLRS,
  MM_RF_PROTO_FS_AFHDS2A,
  MM_RF_PROTO_Q2X2,
  MM_RF_PROTO_WK_2X01,
  MM_RF_PROTO_Q303,
  MM_RF_PROTO_GW008,
  MM_RF_PROTO_DM002,
  MM_RF_PROTO_CABELL,
  MM_RF_PROTO_ESKY150,
  MM_RF_PROTO_H83D,
  MM_RF_PROTO_CORONA,
  MM_RF_PROTO_CFLIE,
  MM_RF_PROTO_LAST= MM_RF_PROTO_CFLIE
};

enum TrainerProtocol {
  TRAINER_MASTER_JACK,
  TRAINER_SLAVE_JACK,
  TRAINER_MASTER_SBUS_MODULE,
  TRAINER_MASTER_CPPM_MODULE,
  TRAINER_MASTER_SBUS_BATT_COMPARTMENT
};

enum R9MSubTypes {
  R9M_FCC,
  R9M_LBT
};

class ModuleData {
  Q_DECLARE_TR_FUNCTIONS(ModuleData)

  public:
    ModuleData() { clear(); }
    unsigned int modelId;
    int          protocol;
    unsigned int subType;
    bool         invertedSerial;
    unsigned int channelsStart;
    int          channelsCount; // 0=8 channels
    unsigned int failsafeMode;
    int          failsafeChannels[CPN_MAX_CHNOUT];


    struct {
      int delay;
      bool pulsePol;           // false = positive
      bool outputType;         // false = open drain, true = push pull
      int frameLength;
    } ppm;

    struct {
      unsigned int rfProtocol;
      bool autoBindMode;
      bool lowPowerMode;
      bool customProto;
      int optionValue;
    } multi;

    struct {
      int power;                   // 0 10 mW, 1 100 mW, 2 500 mW, 3 1W
      bool receiver_telem_off;     // false = receiver telem enabled
      bool receiver_channel_9_16;  // false = pwm out 1-8, true 9-16
      bool external_antenna;       // false = internal antenna, true = external antenna
      bool sport_out;
    } pxx;


    void clear() { memset(this, 0, sizeof(ModuleData)); }
    QString polarityToString() const { return ppm.pulsePol ? tr("Positive") : tr("Negative"); } // TODO ModelPrinter
    void convert(RadioDataConversionState & cstate);
};

#endif // MODULEDATA_H
