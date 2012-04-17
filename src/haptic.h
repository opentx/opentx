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

#ifndef haptic_h
#define haptic_h

#define HAPTIC_QUEUE_LENGTH  3

class hapticQueue
{
  public:

    hapticQueue();

    // only difference between these two functions is that one does the
    // interupt queue (Now) and the other queues for playing ASAP.
    void playNow(uint8_t tLen, uint8_t tPause, uint8_t tRepeat=0);

    void playASAP(uint8_t tLen, uint8_t tPause, uint8_t tRepeat=0);

    inline bool busy() { return (buzzTimeLeft > 0); }

    void event(uint8_t e);

    // heartbeat is responsibile for issueing the haptic buzzs and general square waves
    // it is essentially the life of the class.
    void heartbeat();

    // bool freeslots(uint8_t slots);

    inline bool empty() {
      return (t_queueRidx == t_queueWidx);
    }

  protected:
    inline uint8_t getHapticLength(uint8_t tLen);

  private:
    uint8_t t_queueRidx;
    uint8_t t_queueWidx;

    uint8_t buzzTimeLeft;
    uint8_t buzzPause;

    uint8_t hapticTick;

    // queue arrays
    uint8_t queueHapticLength[HAPTIC_QUEUE_LENGTH];
    uint8_t queueHapticPause[HAPTIC_QUEUE_LENGTH];
    uint8_t queueHapticRepeat[HAPTIC_QUEUE_LENGTH];
};

//wrapper function - dirty but results in a space saving!!!
extern hapticQueue haptic;

void hapticDefevent(uint8_t e);

#define IS_HAPTIC_BUSY()     haptic.busy()

#define HAPTIC_HEARTBEAT()   haptic.heartbeat()

#ifdef AUDIO
inline void hapticAudioEvent(uint8_t e)
{
  if (g_eeGeneral.hapticMode>=-1 && e<=AU_ERROR)
    hapticDefevent(1);
  else if (e == AU_TIMER_30)
    hapticDefevent(3);
  else if (e == AU_TIMER_20)
    hapticDefevent(2);   
  else if (e == AU_TIMER_10)
    hapticDefevent(1);   
  else if (g_eeGeneral.hapticMode==0 && e>=AU_WARNING1)
    hapticDefevent(1);     
  else if (g_eeGeneral.hapticMode>0)
    hapticDefevent(0);
}
#else
inline void hapticBeepEvent(uint8_t e)
{
  if (g_eeGeneral.hapticMode>=-1 && e>=3)
    hapticDefevent(1);
  else if (e == AU_TIMER_30)
    hapticDefevent(3);
  else if (e == AU_TIMER_20)
    hapticDefevent(2);   
  else if (e == AU_TIMER_10)
    hapticDefevent(1);     
  else if (g_eeGeneral.hapticMode==0 && e>0)
    hapticDefevent(1);
  else if (g_eeGeneral.hapticMode>0)
    hapticDefevent(0);
}
#endif

#endif // haptic_h
