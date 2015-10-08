/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
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

#ifndef pulses_arm_h
#define pulses_arm_h

#if NUM_MODULES == 2
  #define MODULES_INIT(...) __VA_ARGS__, __VA_ARGS__
#else
  #define MODULES_INIT(...) __VA_ARGS__
#endif

extern uint8_t s_current_protocol[NUM_MODULES];
extern uint8_t s_pulses_paused;
extern uint16_t failsafeCounter[NUM_MODULES];

#if defined(PPM_PIN_HW_SERIAL)
PACK(struct PpmPulsesData {
  uint16_t pulses[20];
  uint32_t index;
});
PACK(struct PxxPulsesData {
  uint8_t  pulses[64];
  uint8_t  *ptr;
  uint16_t pcmValue;
  uint16_t pcmCrc;
  uint32_t pcmOnesCount;
  uint16_t serialByte;
  uint16_t serialBitCount;
});
PACK(struct Dsm2PulsesData {
  uint8_t  pulses[64];
  uint8_t *ptr;
  uint8_t  serialByte ;
  uint8_t  serialBitCount;
});
#else
PACK(struct PpmPulsesData {
  uint16_t pulses[20];
  uint16_t *ptr;
});
PACK(struct PxxPulsesData {
  uint16_t pulses[400];
  uint16_t *ptr;
  uint16_t pcmValue;
  uint16_t pcmCrc;
  uint32_t pcmOnesCount;
});
PACK(struct Dsm2PulsesData {
  uint16_t pulses[400];
  uint16_t *ptr;
  uint16_t value;
  uint16_t index;
});
#endif

union ModulePulsesData {
  PxxPulsesData pxx;
  Dsm2PulsesData dsm2;
  PpmPulsesData ppm;
};

union TrainerPulsesData {
  PpmPulsesData ppm;
};

extern ModulePulsesData modulePulsesData[NUM_MODULES];
extern TrainerPulsesData trainerPulsesData;

void setupPulses(unsigned int port);
void setupPulsesDSM2(unsigned int port);
void setupPulsesPXX(unsigned int port);
void setupPulsesPPM(unsigned int port);

#if defined(HUBSAN)
void Hubsan_Init();
#endif

inline void startPulses()
{
  s_pulses_paused = false;

#if defined(PCBTARANIS)
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
  for (int i=0; i<NUM_MODULES; i++)
    failsafeCounter[i] = 100;
}

#endif
