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

#include "../open9x.h"

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

  if (toneTimeLeft > 0) {
#if defined(PCBV4)
    if (toneFreq) {
      OCR0A = (5000 / toneFreq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
      SPEAKER_ON;
    }
#endif
    toneTimeLeft--; //time gets counted down
    toneFreq += toneFreqIncr;
    // TODO tone2TimeLeft = 0?
  }
  else {
    if (tonePause > 0) {
      SPEAKER_OFF;
      tonePause--; //time gets counted down
    }
    else if (t_queueRidx != t_queueWidx) {
      toneFreq = queueToneFreq[t_queueRidx];
      toneTimeLeft = queueToneLength[t_queueRidx];
      toneFreqIncr = queueToneFreqIncr[t_queueRidx];
      tonePause = queueTonePause[t_queueRidx];
      if (!queueToneRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) % AUDIO_QUEUE_LENGTH;
      }
    }
    else {
      if (tone2TimeLeft > 0) {
#if defined(PCBV4)
        if (tone2Freq) {
          OCR0A = (5000 / tone2Freq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
          SPEAKER_ON;
        }
#else
        toneFreq = tone2Freq;
#endif
        tone2TimeLeft--; //time gets counted down
      } 
      else {
        SPEAKER_OFF;
        if (tone2Pause > 0) {
          tone2Pause--; //time gets counted down
        }
      }
    }
  }
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

void audioQueue::play(uint8_t tFreq, uint8_t tLen, uint8_t tPause,
    uint8_t tFlags, int8_t tFreqIncr)
{
  if (tFlags & PLAY_SOUND_VARIO) {
    tone2Freq = tFreq;
    tone2TimeLeft = tLen;
    tone2Pause = tPause;
  }
  else {
    tFreq += g_eeGeneral.speakerPitch + BEEP_OFFSET; // add pitch compensator
    tLen = getToneLength(tLen);
    if (tFlags & PLAY_NOW || (!busy() && empty())) {
      toneFreq = tFreq;
      toneTimeLeft = tLen;
      tonePause = tPause;
      toneFreqIncr = tFreqIncr;
      t_queueWidx = t_queueRidx;
    }
    else {
      tFlags++;
    }
  
    tFlags &= 0x0f;
    if (tFlags) {
      uint8_t next_queueWidx = (t_queueWidx + 1) % AUDIO_QUEUE_LENGTH;
      if (next_queueWidx != t_queueRidx) {
        queueToneFreq[t_queueWidx] = tFreq;
        queueToneLength[t_queueWidx] = tLen;
        queueTonePause[t_queueWidx] = tPause;
        queueToneRepeat[t_queueWidx] = tFlags - 1;
        queueToneFreqIncr[t_queueWidx] = tFreqIncr;
        t_queueWidx = next_queueWidx;
      }
    }
  }
}

void audioQueue::event(uint8_t e, uint8_t f)
{
  if (g_eeGeneral.flashBeep && (e <= AU_ERROR || e >= AU_WARNING1))
    g_LightOffCounter = FLASH_DURATION;

  if (g_eeGeneral.beeperMode>0 || (g_eeGeneral.beeperMode==0 && e>=AU_WARNING1) || (g_eeGeneral.beeperMode>=-1 && e<=AU_ERROR)) {
    if (e < AU_FRSKY_FIRST || empty()) {
      switch (e) {
        // inactivity timer alert
        case AU_INACTIVITY:
          play(70, 10, 2, 2|PLAY_NOW);
          break;
        // low battery in tx
        case AU_TX_BATTERY_LOW:
          if (empty()) {
            play(60, 20, 3, 2, 1);
            play(80, 20, 3, 2, -1);
          }
          break;
        // error
        case AU_ERROR:
          play(BEEP_DEFAULT_FREQ, 40, 1, PLAY_NOW);
          break;
        // keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_UP:
          play(BEEP_KEY_UP_FREQ, 10, 1, PLAY_NOW);
          break;
        // keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_DOWN:
          play(BEEP_KEY_DOWN_FREQ, 10, 1, PLAY_NOW);
          break;
        // menu display (also used by a few generic beeps)
        case AU_MENUS:
          play(BEEP_DEFAULT_FREQ, 10, 2, PLAY_NOW);
          break;
        // trim move
        case AU_TRIM_MOVE:
          play(f, 6, 1, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_MIDDLE:
          play(BEEP_DEFAULT_FREQ, 10, 2, PLAY_NOW);
          break;
        // warning one
        case AU_WARNING1:
          play(BEEP_DEFAULT_FREQ, 10, 1, PLAY_NOW);
          break;
        // warning two
        case AU_WARNING2:
          play(BEEP_DEFAULT_FREQ, 20, 1, PLAY_NOW);
          break;
        // warning three
        case AU_WARNING3:
          play(BEEP_DEFAULT_FREQ, 30, 1, PLAY_NOW);
          break;
        // startup tune
        case AU_TADA:
          play(50, 10, 5);
          play(90, 10, 5);
          play(110, 5, 4, 2);
          break;
        // pot/stick center
        case AU_POT_STICK_MIDDLE:
          play(BEEP_DEFAULT_FREQ + 50, 10, 1, PLAY_NOW);
          break;
        // mix warning 1
        case AU_MIX_WARNING_1:
          play(BEEP_DEFAULT_FREQ + 50, 6, 0, PLAY_NOW);
          break;
        // mix warning 2
        case AU_MIX_WARNING_2:
          play(BEEP_DEFAULT_FREQ + 52, 6, 0, PLAY_NOW);
          break;
        // mix warning 3
        case AU_MIX_WARNING_3:
          play(BEEP_DEFAULT_FREQ + 54, 6, 0, PLAY_NOW);
          break;
        // time 30 seconds left
        case AU_TIMER_30:
          play(BEEP_DEFAULT_FREQ + 50, 15, 3, 2|PLAY_NOW);
          break;
        // time 20 seconds left
        case AU_TIMER_20:
          play(BEEP_DEFAULT_FREQ + 50, 15, 3, 1|PLAY_NOW);
          break;
        // time 10 seconds left
        case AU_TIMER_10:
        // time <3 seconds left
        case AU_TIMER_LT3:
          play(BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_NOW);
          break;
        case AU_FRSKY_WARN1:
          play(BEEP_DEFAULT_FREQ+20,15,5,2);
          break;
        case AU_FRSKY_WARN2:
          play(BEEP_DEFAULT_FREQ+30,15,5,2);
          break;
        case AU_FRSKY_CHEEP:
          play(BEEP_DEFAULT_FREQ+30,10,2,2,2);
          break;
        case AU_FRSKY_RING:
          play(BEEP_DEFAULT_FREQ+25,5,2,10);
          play(BEEP_DEFAULT_FREQ+25,5,10,1);
          play(BEEP_DEFAULT_FREQ+25,5,2,10);
          break;
        case AU_FRSKY_SCIFI:
          play(80,10,3,2,-1);
          play(60,10,3,2,1);
          play(70,10,1,0);
          break;
        case AU_FRSKY_ROBOT:
          play(70,5,1,1);
          play(50,15,2,1);
          play(80,15,2,1);
          break;
        case AU_FRSKY_CHIRP:
          play(BEEP_DEFAULT_FREQ+40,5,1,2);
          play(BEEP_DEFAULT_FREQ+54,5,1,3);
          break;
        case AU_FRSKY_TADA:
          play(50,5,5);
          play(90,5,5);
          play(110,3,4,2);
          break;
        case AU_FRSKY_CRICKET:
          play(80,5,10,3);
          play(80,5,20,1);
          play(80,5,10,3);
          break;
        case AU_FRSKY_SIREN:
          play(10,20,5,2,1);
          break;
        case AU_FRSKY_ALARMC:
          play(50,4,10,2);
          play(70,8,20,1);
          play(50,8,10,2);
          play(70,4,20,1);
          break;
        case AU_FRSKY_RATATA:
          play(BEEP_DEFAULT_FREQ+50,5,10,10);
          break;
        case AU_FRSKY_TICK:
          play(BEEP_DEFAULT_FREQ+50,5,50,2);
          break;
        default:
          break;
      }
    }
  }
}

void audioDefevent(uint8_t e)
{
#ifdef HAPTIC
  haptic.event(e); //do this before audio to help sync timings
#endif	
  audio.event(e);
}
