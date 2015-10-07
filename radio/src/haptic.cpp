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

#include "opentx.h"

hapticQueue::hapticQueue()
{
  buzzTimeLeft = 0;
  buzzPause = 0;

  t_queueRidx = 0;
  t_queueWidx = 0;

  hapticTick = 0;
}

void hapticQueue::heartbeat()
{
#if defined(SIMU)
  return;
#else
  if (buzzTimeLeft > 0) {
    buzzTimeLeft--; // time gets counted down
#if defined(PCBSKY9X) || (defined(PCBTARANIS) && defined(REVPLUS)) || defined(PCBFLAMENCO) || defined(PCBHORUS)
    // TODO define HAPTIC_PWM option
    hapticOn(HAPTIC_STRENGTH() * 20);
#else
    if (hapticTick-- > 0) {
      hapticOn();
    }
    else {
      hapticOff();
      hapticTick = HAPTIC_STRENGTH();
    }
#endif
  }
  else {
    hapticOff();
    if (buzzPause > 0) {
      buzzPause--;
    }
    else if (t_queueRidx != t_queueWidx) {
      buzzTimeLeft = queueHapticLength[t_queueRidx];
      buzzPause = queueHapticPause[t_queueRidx];
      if (!queueHapticRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) & (HAPTIC_QUEUE_LENGTH-1);
      }
    }
  }
#endif // defined(SIMU)
}

void hapticQueue::play(uint8_t tLen, uint8_t tPause, uint8_t tFlags)
{
  tLen = getHapticLength(tLen);

  if ((tFlags & PLAY_NOW) || (!busy() && empty())) {
    buzzTimeLeft = tLen;
    buzzPause = tPause;
    t_queueWidx = t_queueRidx;
  }
  else {
    tFlags += 1;
  }

  tFlags &= 0x0f;
  if (tFlags) {
    uint8_t next_queueWidx = (t_queueWidx + 1) & (HAPTIC_QUEUE_LENGTH-1);
    if (next_queueWidx != t_queueRidx) {
      queueHapticLength[t_queueWidx] = tLen;
      queueHapticPause[t_queueWidx] = tPause;
      queueHapticRepeat[t_queueWidx] = tFlags-1;
      t_queueWidx = next_queueWidx;
    }
  }
}

void hapticQueue::event(uint8_t e)
{
  if (g_eeGeneral.hapticMode>0 || (g_eeGeneral.hapticMode==0 && e>=AU_WARNING1) || (g_eeGeneral.hapticMode>=-1 && e<=AU_ERROR)) {
    if (e <= AU_ERROR)
      play(15, 3, PLAY_NOW);
    else if (e <= AU_TRIM_MOVE)
      play(5, 0, PLAY_NOW);
    else if (e <= AU_TIMER_LT10)
      play(15, 3, PLAY_NOW);
    else if (e < AU_FRSKY_FIRST)
      play(15, 3, (e-AU_TIMER_20+1)|PLAY_NOW);
    else if (e >= AU_FRSKY_LAST && empty()) {
      play(30, 10, 0);
      play(10, 50-10*(e-AU_FRSKY_LAST), (e-AU_FRSKY_LAST));
    }	
  }
}

hapticQueue haptic;
