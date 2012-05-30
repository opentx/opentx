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
    if (toneChanged) {
      toneChanged = 0;
      set_frequency(toneFreq * 61 / 2);
      if(toneRateLimit == 0){ //only start if within rate limit
      	if(toneFreq == 0){
      		tone_stop();
	} else {		
      		tone_start(0);
	} 
      }	
      toneRateLimit++; //count up the rate limit
      if(toneRateLimit == TONE1_RATE_LIMIT){ //reset the rate limit
	toneRateLimit = 0;	
      }      
    }
    else if (toneFreqIncr && (toneTimeLeft&1) == 0) {
      toneFreq += toneFreqIncr;
      set_frequency(toneFreq * 61 / 2);
    }
    toneTimeLeft--; //time gets counted down
  }
  else {
    if (tonePause > 0) {
      tone_stop();
      toneRateLimit = 0; //reset rate limit
      tonePause--; //time gets counted down
    }
    else if (t_queueRidx != t_queueWidx) {
      toneChanged = 1;
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
        if (tone2Changed) {
          tone2Changed = 0;
          set_frequency(tone2Freq * 61 / 2);
          if(tone2RateLimit == 0){ //only start if within rate limit
          	tone_start(0);
          }
        }
        tone2TimeLeft--; //time gets counted down
        tone2RateLimit++; //count up the rate limit
        if(tone2RateLimit == TONE2_RATE_LIMIT){ //reset the rate limit
        	tone2RateLimit = 0;	
        }
      }
      else {
        tone_stop();
        tone2RateLimit = 0; //reset rate limit
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

void audioQueue::pause(uint8_t tLen)
{
  play(0, tLen, 5); // a pause
}	

void audioQueue::play(uint8_t tFreq, uint8_t tLen, uint8_t tPause,
    uint8_t tFlags, int8_t tFreqIncr)
{
  if (tFlags & PLAY_SOUND_VARIO) {
    tone2Changed = 1;
    tone2Freq = tFreq;
    tone2TimeLeft = tLen;
    tone2Pause = tPause;
  }
  else {
    if (tFreq > 0) { //we dont add pitch if zero as this is a pause only event
      tFreq += g_eeGeneral.speakerPitch + BEEP_OFFSET; // add pitch compensator
    }
    tLen = getToneLength(tLen);
    if (tFlags & PLAY_NOW || (!busy() && empty())) {
      toneChanged = 1;
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
#ifdef HAPTIC
  haptic.event(e); //do this before audio to help sync timings
#endif

  if (g_eeGeneral.flashBeep && (e <= AU_ERROR || e >= AU_WARNING1)) {
    if (g_LightOffCounter < FLASH_DURATION)
      g_LightOffCounter = FLASH_DURATION;
  }

  if (g_eeGeneral.beeperMode>0 || (g_eeGeneral.beeperMode==0 && e>=AU_TRIM_MOVE) || (g_eeGeneral.beeperMode>=-1 && e<=AU_ERROR)) {
    if (e < AU_FRSKY_FIRST || empty()) {
      switch (e) {
        // inactivity timer alert
        case AU_INACTIVITY:
          play(70, 20, 4, 2|PLAY_NOW);
          break;
        // low battery in tx
        case AU_TX_BATTERY_LOW:
          if (empty()) {
            play(60, 40, 6, 2, 1);
            play(80, 40, 6, 2, -1);
          }
          break;
        // error
        case AU_ERROR:
          play(BEEP_DEFAULT_FREQ, 80, 2, PLAY_NOW);
          break;
        // keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_UP:
          play(BEEP_KEY_UP_FREQ, 20, 2, PLAY_NOW);
          break;
        // keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_DOWN:
          play(BEEP_KEY_DOWN_FREQ, 20, 2, PLAY_NOW);
          break;
        // menu display (also used by a few generic beeps)
        case AU_MENUS:
          play(BEEP_DEFAULT_FREQ, 20, 4, PLAY_NOW);
          break;
        // trim move
        case AU_TRIM_MOVE:
          play(f, 12, 2, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_MIDDLE:
          play(f, 20, 4, PLAY_NOW);
          break;
        // warning one
        case AU_WARNING1:
          play(BEEP_DEFAULT_FREQ, 20, 2, PLAY_NOW);
          break;
        // warning two
        case AU_WARNING2:
          play(BEEP_DEFAULT_FREQ, 40, 2, PLAY_NOW);
          break;
        // warning three
        case AU_WARNING3:
          play(BEEP_DEFAULT_FREQ, 60, 2, PLAY_NOW);
          break;
        // startup tune
        case AU_TADA:
          play(50, 20, 10);
          play(90, 20, 10);
          play(110, 10, 8, 2);
          break;
        // pot/stick center
        case AU_POT_STICK_MIDDLE:
          play(BEEP_DEFAULT_FREQ + 50, 20, 2, PLAY_NOW);
          break;
        // mix warning 1
        case AU_MIX_WARNING_1:
          play(BEEP_DEFAULT_FREQ + 50, 12, 0, PLAY_NOW);
          break;
        // mix warning 2
        case AU_MIX_WARNING_2:
          play(BEEP_DEFAULT_FREQ + 52, 12, 0, PLAY_NOW);
          break;
        // mix warning 3
        case AU_MIX_WARNING_3:
          play(BEEP_DEFAULT_FREQ + 54, 12, 0, PLAY_NOW);
          break;
        // time 30 seconds left
        case AU_TIMER_30:
          play(BEEP_DEFAULT_FREQ + 50, 30, 6, 2|PLAY_NOW);
          break;
        // time 20 seconds left
        case AU_TIMER_20:
          play(BEEP_DEFAULT_FREQ + 50, 30, 6, 1|PLAY_NOW);
          break;
        // time 10 seconds left
        case AU_TIMER_10:
          play(BEEP_DEFAULT_FREQ + 50, 30, 6, PLAY_NOW);
          break;
        // time <3 seconds left
        case AU_TIMER_LT3:
          play(BEEP_DEFAULT_FREQ + 50, 30, 6, PLAY_NOW);
          break;
        case AU_FRSKY_WARN1:
          play(BEEP_DEFAULT_FREQ+20,30,10,2);
          pause(200);
          break;
        case AU_FRSKY_WARN2:
          play(BEEP_DEFAULT_FREQ+30,30,10,2);
          pause(200);
          break;
        case AU_FRSKY_CHEEP:
          play(BEEP_DEFAULT_FREQ+30,20,4,2,2);
          pause(200);
          break;
        case AU_FRSKY_RING:
          play(BEEP_DEFAULT_FREQ+25,10,4,10);
          play(BEEP_DEFAULT_FREQ+25,10,20,1);
          play(BEEP_DEFAULT_FREQ+25,10,4,10);
          pause(200);
          break;
        case AU_FRSKY_SCIFI:
          play(80,20,6,2,-1);
          play(60,20,6,2,1);
          play(70,20,2,0);
          pause(200);
          break;
        case AU_FRSKY_ROBOT:
          play(70,10,2,1);
          play(50,30,4,1);
          play(80,30,4,1);
          pause(200);
          break;
        case AU_FRSKY_CHIRP:
          play(BEEP_DEFAULT_FREQ+40,10,2,2);
          play(BEEP_DEFAULT_FREQ+54,10,2,3);
          pause(200);
          break;
        case AU_FRSKY_TADA:
          play(50,10,10);
          play(90,10,10);
          play(110,6,8,2);
          pause(200);
          break;
        case AU_FRSKY_CRICKET:
          play(80,10,20,3);
          play(80,10,40,1);
          play(80,10,20,3);
          pause(200);
          break;
        case AU_FRSKY_SIREN:
          play(10,40,10,2,1);
          pause(200);
          break;
        case AU_FRSKY_ALARMC:
          play(50,8,20,2);
          play(70,16,40,1);
          play(50,16,20,2);
          play(70,8,40,1);
          pause(200);
          break;
        case AU_FRSKY_RATATA:
          play(BEEP_DEFAULT_FREQ+50,10,20,10);
          break;
        case AU_FRSKY_TICK:
          play(BEEP_DEFAULT_FREQ+50,10,100,2);
          pause(200);
          break;
        default:
          break;
      }
    }
  }
}

void audioDefevent(uint8_t e)
{
  audio.event(e, BEEP_DEFAULT_FREQ);
}
