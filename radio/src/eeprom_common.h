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

#if defined(SIMU)
  #define WRITE_DELAY_10MS 200
#elif defined(PCBTARANIS)
  #define WRITE_DELAY_10MS 500
#elif defined(PCBSKY9X) && !defined(REV0)
  #define WRITE_DELAY_10MS 500
#elif defined(PCBGRUVIN9X) && !defined(REV0)
  #define WRITE_DELAY_10MS 500
#else
  #define WRITE_DELAY_10MS 200
#endif

#define TIME_TO_WRITE() (s_eeDirtyMsk && (tmr10ms_t)(get_tmr10ms() - s_eeDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)

extern uint8_t   s_eeDirtyMsk;
extern tmr10ms_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk);
void eeCheck(bool immediately);
void eeReadAll();
bool eeModelExists(uint8_t id);
void eeLoadModel(uint8_t id);
bool eeConvert();
void eeErase(bool warn);
void ConvertModel(int id, int version);
uint8_t eeFindEmptyModel(uint8_t id, bool down);
void selectModel(uint8_t sub);

#if defined(CPUARM)
  extern ModelHeader modelHeaders[MAX_MODELS];
  void eeLoadModelHeader(uint8_t id, ModelHeader *header);
  void eeLoadModelHeaders();
#else
  #define eeLoadModelHeaders()
#endif
