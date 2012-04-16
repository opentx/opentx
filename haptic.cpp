/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"

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
#endif

#if defined(PCBARM)
  if (buzzTimeLeft > 0) {
    buzzTimeLeft--; //time gets counted down
    hapticOn((g_eeGeneral.hapticStrength * 2) * 10);
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
        t_queueRidx = (t_queueRidx + 1) % HAPTIC_QUEUE_LENGTH;
      }
    }
  }
#else
  if (buzzTimeLeft > 0) {
    buzzTimeLeft--; // time gets counted down

    if (hapticTick-- > 0) {
      HAPTIC_ON; // haptic output 'high'
    }
    else {
      HAPTIC_OFF; // haptic output 'high'
      hapticTick = g_eeGeneral.hapticStrength;
    }
  }
  else {
    HAPTIC_OFF; // haptic output 'high'
    if (buzzPause > 0) {
      buzzPause--;
    }
    else if (t_queueRidx != t_queueWidx) {
      buzzTimeLeft = queueHapticLength[t_queueRidx];
      buzzPause = queueHapticPause[t_queueRidx];
      if (!queueHapticRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) % HAPTIC_QUEUE_LENGTH;
      }
    }
  }
#endif
}

inline uint8_t hapticQueue::getHapticLength(uint8_t tLen)
{
  return ((g_eeGeneral.hapticLength * 2) + tLen) * 2;
}

void hapticQueue::playNow(uint8_t tLen, uint8_t tPause, uint8_t tRepeat)
{
  buzzTimeLeft = getHapticLength(tLen);
  buzzPause = tPause;
  t_queueWidx = t_queueRidx;

  if (tRepeat) {
    playASAP(tLen, tPause, tRepeat-1);
  }
}

void hapticQueue::playASAP(uint8_t tLen, uint8_t tPause, uint8_t tRepeat)
{
  uint8_t next_queueWidx = (t_queueWidx + 1) % HAPTIC_QUEUE_LENGTH;
  if (next_queueWidx != t_queueRidx) {
    queueHapticLength[t_queueWidx] = getHapticLength(tLen);
    queueHapticPause[t_queueWidx] = tPause;
    queueHapticRepeat[t_queueWidx] = tRepeat-1;
    t_queueWidx = next_queueWidx;
  }
}

void hapticQueue::event(uint8_t e)
{
    switch (e) {
      case 0: // very little buzz for keys / trims
        playNow(5, 0, 0);
        break;
      case 1: // one buzz
        playASAP(10,2,1);
        break;
      case 2: // two buzz
        playASAP(10,2,2);
        break;
      case 3: // three buzz
        playASAP(10,2,3);
        break;
      default:
        break;
  }
}

void hapticDefevent(uint8_t e)
{
  haptic.event(e);
}
