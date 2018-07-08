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

#ifndef _PULSES_ARM_H_
#define _PULSES_ARM_H_

#if NUM_MODULES == 2
  #define MODULES_INIT(...)            __VA_ARGS__, __VA_ARGS__
#else
  #define MODULES_INIT(...)            __VA_ARGS__
#endif

#if defined(PCBX12S) && PCBREV < 13
  #define pulse_duration_t             uint32_t
  #define trainer_pulse_duration_t     uint16_t
#else
  #define pulse_duration_t             uint16_t
  #define trainer_pulse_duration_t     uint16_t
#endif

extern uint8_t s_current_protocol[NUM_MODULES];
extern uint8_t s_pulses_paused;
extern uint16_t failsafeCounter[NUM_MODULES];

template<class T> struct PpmPulsesData {
  T pulses[20];
  T * ptr;
};

#if defined(PXX_FREQUENCY_HIGH)
#define EXTMODULE_USART_PXX_BAUDRATE  420000
#define INTMODULE_USART_PXX_BAUDRATE  450000
#define PXX_PERIOD                    4/*ms*/
#else
#define EXTMODULE_USART_PXX_BAUDRATE  115200
#define INTMODULE_USART_PXX_BAUDRATE  115200
#define PXX_PERIOD                    9/*ms*/
#endif

#define PXX_PERIOD_HALF_US            (PXX_PERIOD * 2000)

#if defined(PPM_PIN_SERIAL)
PACK(struct PxxSerialPulsesData {
  uint8_t  pulses[64];
  uint8_t  * ptr;
  uint16_t pcmValue;
  uint16_t pcmCrc;
  uint32_t pcmOnesCount;
  uint16_t serialByte;
  uint16_t serialBitCount;
});
#endif

#if defined(PPM_PIN_SERIAL)
PACK(struct Dsm2SerialPulsesData {
  uint8_t  pulses[64];
  uint8_t * ptr;
  uint8_t  serialByte ;
  uint8_t  serialBitCount;
  uint16_t _alignment;
});
#else
#define MAX_PULSES_TRANSITIONS 300
PACK(struct Dsm2TimerPulsesData {
  pulse_duration_t pulses[MAX_PULSES_TRANSITIONS];
  pulse_duration_t * ptr;
  uint16_t rest;
  uint8_t index;
});
#endif

#if defined(INTMODULE_USART) || defined(EXTMODULE_USART)
PACK(struct PxxUartPulsesData {
  uint8_t  pulses[64];
  uint8_t  * ptr;
  uint16_t pcmCrc;
  uint16_t _alignment;
});
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

#if !defined(INTMODULE_USART) || !defined(EXTMODULE_USART)
/* PXX uses 20 bytes (as of Rev 1.1 document) with 8 changes per byte + stop bit ~= 162 max pulses */
/* DSM2 uses 2 header + 12 channel bytes, with max 10 changes (8n2) per byte + 16 bits trailer ~= 156 max pulses */
/* Multimodule uses 3 bytes header + 22 channel bytes with max 11 changes per byte (8e2) + 16 bits trailer ~= 291 max pulses */
/* Multimodule reuses some of the DSM2 function and structs since the protocols are similar enough */
/* sbus is 1 byte header, 22 channel bytes (11bit * 16ch) + 1 byte flags */

#if defined(PXX_FREQUENCY_HIGH)
#error "Pulses array needs to be increased (PXX_FREQUENCY=HIGH)"
#endif

PACK(struct PxxTimerPulsesData {
  pulse_duration_t pulses[200];
  pulse_duration_t * ptr;
  uint16_t rest;
  uint16_t pcmCrc;
  uint32_t pcmOnesCount;
});
#endif

#define CROSSFIRE_FRAME_MAXLEN         64
#define CROSSFIRE_CHANNELS_COUNT       16
PACK(struct CrossfirePulsesData {
  uint8_t pulses[CROSSFIRE_FRAME_MAXLEN];
});

union ModulePulsesData {
#if defined(INTMODULE_USART) || defined(EXTMODULE_USART)
  PxxUartPulsesData pxx_uart;
#endif
#if defined(PPM_PIN_SERIAL)
  PxxSerialPulsesData pxx;
#elif !defined(INTMODULE_USART) || !defined(EXTMODULE_USART)
  PxxTimerPulsesData pxx;
#endif

#if defined(PPM_PIN_SERIAL)
  Dsm2SerialPulsesData dsm2;
#else
  Dsm2TimerPulsesData dsm2;
#endif

  PpmPulsesData<pulse_duration_t> ppm;
  CrossfirePulsesData crossfire;
} __ALIGNED;

/* The __ALIGNED keyword is required to align the struct inside the modulePulsesData below,
 * which is also defined to be __DMA  (which includes __ALIGNED) aligned.
 * Arrays in C/C++ are always defined to be *contiguously*. The first byte of the second element is therefore always
 * sizeof(ModulePulsesData). __ALIGNED is required for sizeof(ModulePulsesData) to be a multiple of the alignment.
 */

/* TODO: internal pulsedata only needs 200 bytes vs 300 bytes for external, both use 300 byte since we have a common struct */
extern ModulePulsesData modulePulsesData[NUM_MODULES];

union TrainerPulsesData {
  PpmPulsesData<trainer_pulse_duration_t> ppm;
};

extern TrainerPulsesData trainerPulsesData;
extern const uint16_t CRCTable[];

void setupPulses(uint8_t port);
void setupPulsesDSM2(uint8_t port);
void setupPulsesMultimodule(uint8_t port);
void setupPulsesSbus(uint8_t port);
void setupPulsesPXX(uint8_t port);
void setupPulsesPPMModule(uint8_t port);
void setupPulsesPPMTrainer();
void sendByteDsm2(uint8_t b);
void putDsm2Flush();
void putDsm2SerialBit(uint8_t bit);
void sendByteSbus(uint8_t byte);

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

inline bool pulsesStarted() { return s_current_protocol[0] != 255; }
inline void pausePulses() { s_pulses_paused = true; }
inline void resumePulses() { s_pulses_paused = false; }

#define SEND_FAILSAFE_NOW(idx) failsafeCounter[idx] = 1

inline void SEND_FAILSAFE_1S()
{
  for (int i=0; i<NUM_MODULES; i++) {
    failsafeCounter[i] = 100;
  }
}

// Assign failsafe values using the current channel outputs
// for channels not set previously to HOLD or NOPULSE
void setCustomFailsafe(uint8_t moduleIndex);

#if defined(PCBXLITE) && !defined(MODULE_R9M_FULLSIZE)
#define LEN_R9M_REGION                 "\007"
#define TR_R9M_REGION                  "FCC\0   ""LBT(EU)"
#define LEN_R9M_FCC_POWER_VALUES       "\010"
#define LEN_R9M_LBT_POWER_VALUES       "\015"
#define TR_R9M_FCC_POWER_VALUES        "(100 mW)"
#define TR_R9M_LBT_POWER_VALUES        "25 mW 8ch\0   ""25 mW 16ch\0  ""100mW no tele"

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

#define BIND_TELEM_ALLOWED(idx)      (!(IS_TELEMETRY_INTERNAL_MODULE() && moduleIdx == EXTERNAL_MODULE) && (!IS_MODULE_R9M_LBT(idx) || g_model.moduleData[idx].pxx.power < R9M_LBT_POWER_100))
#define BIND_CH9TO16_ALLOWED(idx)    (!IS_MODULE_R9M_LBT(idx) || g_model.moduleData[idx].pxx.power != R9M_LBT_POWER_25)

#else

#define LEN_R9M_REGION                 "\007"
#define TR_R9M_REGION                  "FCC\0   ""LBT(EU)"
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

#define BIND_TELEM_ALLOWED(idx)      (!(IS_TELEMETRY_INTERNAL_MODULE() && moduleIdx == EXTERNAL_MODULE) && (!IS_MODULE_R9M_LBT(idx) || g_model.moduleData[idx].pxx.power < R9M_LBT_POWER_200))
#define BIND_CH9TO16_ALLOWED(idx)    (!IS_MODULE_R9M_LBT(idx) || g_model.moduleData[idx].pxx.power != R9M_LBT_POWER_25)
#endif
#endif // _PULSES_ARM_H_
