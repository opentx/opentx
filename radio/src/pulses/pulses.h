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

#ifndef _PULSES_H_
#define _PULSES_H_

#include "definitions.h"
#include "dataconstants.h"
#include "pulses_common.h"
#include "pulses/pxx1.h"
#include "pulses/pxx2.h"

#if NUM_MODULES > 1
  #define IS_RANGECHECK_ENABLE()             (moduleSettings[0].mode == MODULE_MODE_RANGECHECK || moduleSettings[1].mode == MODULE_MODE_RANGECHECK)
#else
  #define IS_RANGECHECK_ENABLE()             (moduleSettings[0].mode == MODULE_MODE_RANGECHECK)
#endif

#if defined(PCBSKY9X) && defined(DSM2)
  #define DSM2_BIND_TIMEOUT      255         // 255*11ms
  extern uint8_t dsm2BindTimer;
#endif

#if defined(PXX)
  #define IS_PXX_PROTOCOL(protocol)          (protocol==PROTO_PXX)
#else
  #define IS_PXX_PROTOCOL(protocol)          (0)
#endif

#if defined(DSM2)
  #define IS_DSM2_PROTOCOL(protocol)         (protocol>=PROTOCOL_CHANNELS_DSM2_LP45 && protocol<=PROTOCOL_CHANNELS_DSM2_DSMX)
#else
  #define IS_DSM2_PROTOCOL(protocol)         (0)
#endif

#if defined(DSM2_SERIAL)
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (IS_DSM2_PROTOCOL(protocol))
#else
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(MULTIMODULE)
  #define IS_MULTIMODULE_PROTOCOL(protocol)  (protocol==PROTOCOL_CHANNELS_MULTIMODULE)
  #if !defined(DSM2)
     #error You need to enable DSM2 = PPM for MULTIMODULE support
  #endif
#else
  #define IS_MULTIMODULE_PROTOCOL(protocol)  (0)
#endif

#define IS_SBUS_PROTOCOL(protocol)         (protocol == PROTOCOL_CHANNELS_SBUS)

extern uint8_t s_pulses_paused;

enum ModuleSettingsMode
{
  MODULE_MODE_NORMAL,
  MODULE_MODE_RANGECHECK,
  MODULE_MODE_BIND,
  MODULE_MODE_REGISTER,
  MODULE_MODE_SPECTRUM_ANALYSER,
  MODULE_MODE_SHARE
};

PACK(struct ModuleSettings {
  uint8_t protocol:4;
  uint8_t paused:1;
  uint8_t mode:3;
  uint16_t counter;
});

extern ModuleSettings moduleSettings[NUM_MODULES];

template<class T> struct PpmPulsesData {
  T pulses[20];
  T * ptr;
};

#if defined(PPM_PIN_SERIAL)
PACK(struct Dsm2SerialPulsesData {
  uint8_t  pulses[64];
  uint8_t * ptr;
  uint8_t  serialByte ;
  uint8_t  serialBitCount;
  uint16_t _alignment;
});
typedef Dsm2SerialPulsesData Dsm2PulsesData;
#else
#define MAX_PULSES_TRANSITIONS 300
PACK(struct Dsm2TimerPulsesData {
  pulse_duration_t pulses[MAX_PULSES_TRANSITIONS];
  pulse_duration_t * ptr;
  uint16_t rest;
  uint8_t index;
});
typedef Dsm2TimerPulsesData Dsm2PulsesData;
#endif

#define PPM_PERIOD_HALF_US(module)   ((g_model.moduleData[module].ppm.frameLength * 5 + 225) * 200) /*half us*/
#define PPM_PERIOD(module)           (PPM_PERIOD_HALF_US(module) / 2000) /*ms*/
#define DSM2_BAUDRATE                125000
#define DSM2_PERIOD                  22 /*ms*/
#define SBUS_BAUDRATE                100000
#define SBUS_PERIOD_HALF_US          ((g_model.moduleData[EXTERNAL_MODULE].sbus.refreshRate * 5 + 225) * 200) /*half us*/
#define SBUS_PERIOD                  (SBUS_PERIOD_HALF_US / 2000) /*ms*/
#define MULTIMODULE_BAUDRATE         100000
#define MULTIMODULE_PERIOD           7 /*ms*/

#define CROSSFIRE_FRAME_MAXLEN         64
PACK(struct CrossfirePulsesData {
  uint8_t pulses[CROSSFIRE_FRAME_MAXLEN];
  uint8_t length;
});

union InternalModulePulsesData {
#if defined(PXX1)
  #if defined(INTMODULE_USART)
    UartPxx1Pulses pxx_uart;
  #else
    PwmPxx1Pulses pxx;
  #endif
#endif

#if defined(PXX2)
  Pxx2Pulses pxx2;
#endif

#if defined(TARANIS_INTERNAL_PPM)
  PpmPulsesData<pulse_duration_t> ppm;
#endif
} __ALIGNED(4);

union ExternalModulePulsesData {
#if defined(PXX1)
  #if defined(INTMODULE_USART) || defined(EXTMODULE_USART)
    UartPxx1Pulses pxx_uart;
  #endif
  #if defined(PPM_PIN_SERIAL)
    SerialPxx1Pulses pxx;
  #elif !defined(INTMODULE_USART) || !defined(EXTMODULE_USART)
    PwmPxx1Pulses pxx;
  #endif
#endif

#if defined(PXX2)
  Pxx2Pulses pxx2;
#endif

#if defined(DSM2) || defined(MULTIMODULE) || defined(SBUS)
  Dsm2PulsesData dsm2;
#endif

  PpmPulsesData<pulse_duration_t> ppm;

  CrossfirePulsesData crossfire;
} __ALIGNED(4);

/* The __ALIGNED keyword is required to align the struct inside the modulePulsesData below,
 * which is also defined to be __DMA  (which includes __ALIGNED) aligned.
 * Arrays in C/C++ are always defined to be *contiguously*. The first byte of the second element is therefore always
 * sizeof(ModulePulsesData). __ALIGNED is required for sizeof(ModulePulsesData) to be a multiple of the alignment.
 */


extern InternalModulePulsesData intmodulePulsesData;
extern ExternalModulePulsesData extmodulePulsesData;

union TrainerPulsesData {
  PpmPulsesData<trainer_pulse_duration_t> ppm;
};

extern TrainerPulsesData trainerPulsesData;

bool setupPulses(uint8_t module);
void setupPulsesDSM2();
void setupPulsesCrossfire();
void setupPulsesMultimodule();
void setupPulsesSbus();
void setupPulsesPPMInternalModule();
void setupPulsesPPMExternalModule();
void setupPulsesPPMTrainer();
void sendByteDsm2(uint8_t b);
void putDsm2Flush();
void putDsm2SerialBit(uint8_t bit);

#if defined(HUBSAN)
void Hubsan_Init();
#endif

inline void startPulses()
{
  s_pulses_paused = false;

#if defined(PCBTARANIS) || defined(PCBHORUS)
  setupPulses(INTERNAL_MODULE);
  setupPulses(EXTERNAL_MODULE);
#else
  setupPulses(EXTERNAL_MODULE);
#endif

#if defined(HUBSAN)
  Hubsan_Init();
#endif
}

enum ChannelsProtocols {
  PROTOCOL_CHANNELS_UNINITIALIZED,
  PROTOCOL_CHANNELS_NONE,
  PROTOCOL_CHANNELS_PPM,
#if defined(PXX) || defined(DSM2)
  PROTOCOL_CHANNELS_PXX1,
#endif
#if defined(DSM2)
  PROTOCOL_CHANNELS_DSM2_LP45,
  PROTOCOL_CHANNELS_DSM2_DSM2,
  PROTOCOL_CHANNELS_DSM2_DSMX,
#endif
  PROTOCOL_CHANNELS_CROSSFIRE,
  PROTOCOL_CHANNELS_MULTIMODULE,
  PROTOCOL_CHANNELS_SBUS,
  PROTOCOL_CHANNELS_PXX2
};

inline bool pulsesStarted() { return moduleSettings[0].protocol != PROTOCOL_CHANNELS_UNINITIALIZED; }
inline void pausePulses() { s_pulses_paused = true; }
inline void resumePulses() { s_pulses_paused = false; }

#define SEND_FAILSAFE_NOW(idx) moduleSettings[idx].counter = 1

inline void SEND_FAILSAFE_1S()
{
  for (int i=0; i<NUM_MODULES; i++) {
    moduleSettings[i].counter = 100;
  }
}

// Assign failsafe values using the current channel outputs
// for channels not set previously to HOLD or NOPULSE
void setCustomFailsafe(uint8_t moduleIndex);

#if defined(PCBXLITE) && !defined(MODULE_R9M_FULLSIZE)
#define LEN_R9M_REGION                 "\006"
#define TR_R9M_REGION                  "FCC\0  ""EU\0   ""868MHz""915MHz"
#define LEN_R9M_FCC_POWER_VALUES       "\010"
#define LEN_R9M_LBT_POWER_VALUES       "\015"
#define TR_R9M_FCC_POWER_VALUES        "(100 mW)"
#define TR_R9M_LBT_POWER_VALUES        "25 mW 8ch\0   ""25 mW 16ch\0  ""100mW no tele"
#define LEN_R9MFLEX_FREQ               "\006"
#define TR_R9MFLEX_FREQ                "868Mhz""915Mhz"

enum R9MFCCPowerValues {
  R9M_FCC_POWER_100 = 0,
  R9M_FCC_POWER_MAX = R9M_FCC_POWER_100
};

enum R9MLBTPowerValues {
  R9M_LBT_POWER_25 = 0,
  R9M_LBT_POWER_25_16,
  R9M_LBT_POWER_100,
  R9M_LBT_POWER_MAX = R9M_LBT_POWER_100
};

#define BIND_TELEM_ALLOWED(idx)      (!(IS_TELEMETRY_INTERNAL_MODULE() && moduleIdx == EXTERNAL_MODULE) && (!isModuleR9M_LBT(idx) || g_model.moduleData[idx].pxx.power < R9M_LBT_POWER_100))
#define BIND_CH9TO16_ALLOWED(idx)    (!isModuleR9M_LBT(idx) || g_model.moduleData[idx].pxx.power != R9M_LBT_POWER_25)

#else

#define LEN_R9M_REGION                 "\004"
#define TR_R9M_REGION                  "FCC\0""EU\0 ""FLEX"
#define LEN_R9MFLEX_FREQ               "\006"
#define TR_R9MFLEX_FREQ                "868Mhz""915Mhz"
#define LEN_R9M_FCC_POWER_VALUES       "\006"
#define LEN_R9M_LBT_POWER_VALUES       "\013"
#define TR_R9M_FCC_POWER_VALUES        "10 mW\0" "100 mW" "500 mW" "1 W\0"
#define TR_R9M_LBT_POWER_VALUES        "25 mW 8ch\0 ""25 mW 16ch\0" "200 mW 16ch" "500 mW 16ch"

enum R9MFCCPowerValues {
  R9M_FCC_POWER_10 = 0,
  R9M_FCC_POWER_100,
  R9M_FCC_POWER_500,
  R9M_FCC_POWER_1000,
  R9M_FCC_POWER_MAX = R9M_FCC_POWER_1000
};

enum R9MLBTPowerValues {
  R9M_LBT_POWER_25 = 0,
  R9M_LBT_POWER_25_16,
  R9M_LBT_POWER_200,
  R9M_LBT_POWER_500,
  R9M_LBT_POWER_MAX = R9M_LBT_POWER_500
};

#define BIND_TELEM_ALLOWED(idx)      (!(IS_TELEMETRY_INTERNAL_MODULE() && moduleIdx == EXTERNAL_MODULE) && (!isModuleR9M_LBT(idx) || g_model.moduleData[idx].pxx.power < R9M_LBT_POWER_200))
#define BIND_CH9TO16_ALLOWED(idx)    (!isModuleR9M_LBT(idx) || g_model.moduleData[idx].pxx.power != R9M_LBT_POWER_25)
#endif

#endif // _PULSES_H_
