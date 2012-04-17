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

audioQueue::audioQueue()
{
  toneTimeLeft = 0;
  tonePause = 0;

  t_queueRidx = 0;
  t_queueWidx = 0;
}

// heartbeat is responsibile for issueing the audio tones and general square waves
// it is essentially the life of the class.
// it is called every 10ms
void audioQueue::heartbeat()
{
#if defined(SIMU)
  return;
#endif

#if defined(PCBARM)
  if (toneTimeLeft) {

    if (queueTone(toneFreq * 61 / 2, toneTimeLeft * 10,
        toneFreqIncr * 61 / 2)) {
      toneTimeLeft = 0; //time gets counted down
    }
  }
  else {
    if (tonePause) {
      if (queueTone(0, tonePause * 10, 0)) {
        tonePause = 0; //time gets counted down
      }
    }
    else {
      if (t_queueRidx != t_queueWidx) {
        toneFreq = queueToneFreq[t_queueRidx];
        toneTimeLeft = queueToneLength[t_queueRidx];
        toneFreqIncr = queueToneFreqIncr[t_queueRidx];
        tonePause = queueTonePause[t_queueRidx];
        if (!queueToneRepeat[t_queueRidx]--) {
          t_queueRidx = (t_queueRidx + 1) % AUDIO_QUEUE_LENGTH;
        }
      }
    }
  }
#else
  if (toneTimeLeft > 0) {
#if defined(PCBV4)
    if (toneFreq) {
      OCR0A = (5000 / toneFreq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
      SPEAKER_ON;
    }
#endif
    toneTimeLeft--; //time gets counted down
    toneFreq += toneFreqIncr;
  }
  else {
    
    if (tonePause > 0) {
      SPEAKER_OFF;
      tonePause--;
    }
    else if (t_queueRidx != t_queueWidx) {
      toneFreq = queueToneFreq[t_queueRidx];
      toneTimeLeft = queueToneLength[t_queueRidx];
      toneFreqIncr = queueToneFreqIncr[t_queueRidx];
      tonePause = queueTonePause[t_queueRidx];
      if((toneFreq==0) || (toneTimeLeft==0)){
        SPEAKER_OFF;
      }
      if (!queueToneRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) % AUDIO_QUEUE_LENGTH;
      }
    } else {
      SPEAKER_OFF;
    }
  }
#endif
}

inline uint8_t audioQueue::getToneLength(uint8_t tLen)
{
  uint8_t result = tLen; // default
  if (g_eeGeneral.beeperLength < 0) {
    result /= (1-g_eeGeneral.beeperLength);
  }
  if (g_eeGeneral.beeperLength > 0) {
    result *= (1+g_eeGeneral.beeperLength);
  }
  return result;
}

bool s_beeper;


void audioQueue::playNow(uint8_t tFreq, uint8_t tLen, uint8_t tPause,
    uint8_t tRepeat, int8_t tFreqIncr)
{
  toneFreq = ((s_beeper && tFreq) ? tFreq + g_eeGeneral.speakerPitch + BEEP_OFFSET : 0); // add pitch compensator
  toneTimeLeft = getToneLength(tLen);
  tonePause = tPause;
  toneFreqIncr = tFreqIncr;
  t_queueWidx = t_queueRidx;

  if (tRepeat) {
    playASAP(tFreq, tLen, tPause, tRepeat-1, tFreqIncr);
  }
}

void audioQueue::playASAP(uint8_t tFreq, uint8_t tLen, uint8_t tPause,
    uint8_t tRepeat, int8_t tFreqIncr)
{
  uint8_t next_queueWidx = (t_queueWidx + 1) % AUDIO_QUEUE_LENGTH;
  if (next_queueWidx != t_queueRidx) {
    queueToneFreq[t_queueWidx] = ((s_beeper && tFreq) ? tFreq + g_eeGeneral.speakerPitch + BEEP_OFFSET : 0); // add pitch compensator
    queueToneLength[t_queueWidx] = getToneLength(tLen);
    queueTonePause[t_queueWidx] = tPause;
    queueToneRepeat[t_queueWidx] = tRepeat -1;
    queueToneFreqIncr[t_queueWidx] = tFreqIncr;
    t_queueWidx = next_queueWidx;
  }
}

void audioQueue::playVario(uint8_t tFreq, uint8_t tLen)
{
  uint8_t next_queueWidx = (t_queueWidx + 1) % AUDIO_QUEUE_LENGTH;
  if (next_queueWidx != t_queueRidx) {
    queueToneFreq[t_queueWidx] = tFreq;
    queueToneLength[t_queueWidx] = tLen;
    queueTonePause[t_queueWidx] = 0;
    queueToneRepeat[t_queueWidx] = 0;
    queueToneFreqIncr[t_queueWidx] = 0;
    t_queueWidx = next_queueWidx;
  }
}

void audioQueue::event(uint8_t e, uint8_t f)
{
  s_beeper = (g_eeGeneral.beeperMode>0 || (g_eeGeneral.beeperMode==0 && e>=AU_WARNING1) || (g_eeGeneral.beeperMode>=-1 && e<=AU_ERROR));
  if (g_eeGeneral.flashBeep && (e <= AU_ERROR || e >= AU_WARNING1)) g_LightOffCounter = FLASH_DURATION; // we got an event do we need to flash the display ?
  if (e < AU_FRSKY_FIRST || empty()) {
    switch (e) {
      // inactivity timer alert
      case AU_INACTIVITY:
        playNow(70, 10, 2, 2);
        break;
      // low battery in tx
      case AU_TX_BATTERY_LOW:
        if (empty()) {
          playASAP(60, 20, 3, 2, 1);
          playASAP(80, 20, 3, 2, -1);
        }
        break;
      // error
      case AU_ERROR:
        playNow(BEEP_DEFAULT_FREQ, 40, 1, 0);
        break;
      // keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
      case AU_KEYPAD_UP:
        playNow(BEEP_KEY_UP_FREQ, 10, 1, 0);
        break;
      // keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
      case AU_KEYPAD_DOWN:
        playNow(BEEP_KEY_DOWN_FREQ, 10, 1, 0);
        break;
      // menu display (also used by a few generic beeps)
      case AU_MENUS:
        playNow(BEEP_DEFAULT_FREQ, 10, 2, 0);
        break;
      // trim move
      case AU_TRIM_MOVE:
        playNow(f, 6, 1);
        break;
      // trim center
      case AU_TRIM_MIDDLE:
        playNow(BEEP_DEFAULT_FREQ, 10, 2, 0);
        break;
      // warning one
      case AU_WARNING1:
        playNow(BEEP_DEFAULT_FREQ, 10, 1, 0);
        break;
      // warning two
      case AU_WARNING2:
        playNow(BEEP_DEFAULT_FREQ, 20, 1, 0);
        break;
      // warning three
      case AU_WARNING3:
        playNow(BEEP_DEFAULT_FREQ, 30, 1, 0);
        break;
      // startup tune
      case AU_TADA:
        playASAP(50, 10, 5);
        playASAP(90, 10, 5);
        playASAP(110, 5, 4, 2);
        break;
      // pot/stick center
      case AU_POT_STICK_MIDDLE:
        playNow(BEEP_DEFAULT_FREQ + 50, 10, 1, 0);
        break;
      // mix warning 1
      case AU_MIX_WARNING_1:
        playNow(BEEP_DEFAULT_FREQ + 50, 6, 0, 0);
        break;
      // mix warning 2
      case AU_MIX_WARNING_2:
        playNow(BEEP_DEFAULT_FREQ + 52, 6, 0, 0);
        break;
      // mix warning 3
      case AU_MIX_WARNING_3:
        playNow(BEEP_DEFAULT_FREQ + 54, 6, 0, 0);
        break;
      // time 30 seconds left
      case AU_TIMER_30:
        playNow(BEEP_DEFAULT_FREQ + 50, 15, 3, 3);
        break;
      // time 20 seconds left
      case AU_TIMER_20:
        playNow(BEEP_DEFAULT_FREQ + 50, 15, 3, 2);
        break;
      // time 10 seconds left
      case AU_TIMER_10:
        playNow(BEEP_DEFAULT_FREQ + 50, 15, 3, 1);
        break;
      // time <3 seconds left
      case AU_TIMER_LT3:
        playNow(BEEP_DEFAULT_FREQ, 20, 25, 1);
        break;
      case AU_FRSKY_WARN1:
        playASAP(BEEP_DEFAULT_FREQ+20,15,5,2);
        break;
      case AU_FRSKY_WARN2:
        playASAP(BEEP_DEFAULT_FREQ+30,15,5,2);
        break;
      case AU_FRSKY_CHEEP:
        playASAP(BEEP_DEFAULT_FREQ+30,10,2,2,2);
        break;
      case AU_FRSKY_RING:
        playASAP(BEEP_DEFAULT_FREQ+25,5,2,10);
        playASAP(BEEP_DEFAULT_FREQ+25,5,10,1);
        playASAP(BEEP_DEFAULT_FREQ+25,5,2,10);
        break;
      case AU_FRSKY_SCIFI:
        playASAP(80,10,3,2,-1);
        playASAP(60,10,3,2,1);
        playASAP(70,10,1,0);
        break;
      case AU_FRSKY_ROBOT:
        playASAP(70,5,1,1);
        playASAP(50,15,2,1);
        playASAP(80,15,2,1);
        break;
      case AU_FRSKY_CHIRP:
        playASAP(BEEP_DEFAULT_FREQ+40,5,1,2);
        playASAP(BEEP_DEFAULT_FREQ+54,5,1,3);
        break;
      case AU_FRSKY_TADA:
        playASAP(50,5,5);
        playASAP(90,5,5);
        playASAP(110,3,4,2);
        break;
      case AU_FRSKY_CRICKET:
        playASAP(80,5,10,3);
        playASAP(80,5,20,1);
        playASAP(80,5,10,3);
        break;
      case AU_FRSKY_SIREN:
        playASAP(10,20,5,2,1);
        break;
      case AU_FRSKY_ALARMC:
        playASAP(50,4,10,2);
        playASAP(70,8,20,1);
        playASAP(50,8,10,2);
        playASAP(70,4,20,1);
        break;
      case AU_FRSKY_RATATA:
        playASAP(BEEP_DEFAULT_FREQ+50,5,10,10);
        break;
      case AU_FRSKY_TICK:
        playASAP(BEEP_DEFAULT_FREQ+50,5,50,2);
        break;
      default:
        break;
    }
  }
}

void audioDefevent(uint8_t e)
{
  audio.event(e, BEEP_DEFAULT_FREQ);
#ifdef HAPTIC
  hapticAudioEvent(e);
#endif
}
