/*
 * Author - Rob Thomson & Bertrand Songis
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
  aqinit();
}

// TODO should not be needed
void audioQueue::aqinit()
{
  //make sure haptic off by default
  HAPTIC_OFF;

  toneTimeLeft = 0;
  tonePause = 0;

  t_queueRidx = 0;
  t_queueRidx = 0;

#ifdef HAPTIC
  toneHaptic = 0;
  hapticTick = 0;
#endif
}

bool audioQueue::busy()
{
  return (toneTimeLeft > 0);
}

#if 0
bool audioQueue::freeslots(uint8_t slots)
{
  return AUDIO_QUEUE_LENGTH - ((t_queueWidx + AUDIO_QUEUE_LENGTH - t_queueRidx) % AUDIO_QUEUE_LENGTH) >= slots;
}
#endif

// heartbeat is responsibile for issueing the audio tones and general square waves
// it is essentially the life of the class.
// it is called every 10ms
void audioQueue::heartbeat()
{
  if (toneTimeLeft > 0) {
#if defined(PCBV4)
    OCR0A = (5000 / toneFreq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
    SPEAKER_ON;
#endif
    toneTimeLeft--; //time gets counted down
    toneFreq += toneFreqIncr;

#if defined(HAPTIC)
    if (toneHaptic){
      if (hapticTick-- > 0) {
        HAPTIC_ON; // haptic output 'high'
      }
      else {
        HAPTIC_OFF; // haptic output 'low'
        hapticTick = g_eeGeneral.hapticStrength;
      }
    }
#endif
  }
  else {
    SPEAKER_OFF;
    HAPTIC_OFF;

    if (tonePause-- <= 0) {
      if (t_queueRidx != t_queueWidx) {
        toneFreq = queueToneFreq[t_queueRidx];
        toneTimeLeft = queueToneLength[t_queueRidx];
        toneFreqIncr = queueToneFreqIncr[t_queueRidx];
        tonePause = queueTonePause[t_queueRidx];
#if defined(HAPTIC)
        toneHaptic = queueToneHaptic[t_queueRidx];
#endif
        if (!queueToneRepeat[t_queueRidx]--) {
          t_queueRidx = (t_queueRidx + 1) % AUDIO_QUEUE_LENGTH;
        }
      }
    }
  }
}

inline uint8_t audioQueue::getToneLength(uint8_t tLen)
{
  uint8_t result = tLen; // default
  if (g_eeGeneral.beeperVal == 2) {
    result /= 3;
  }
  else if (g_eeGeneral.beeperVal == 3) {
    result /= 2;
  }
  else if (g_eeGeneral.beeperVal == 5) {
    //long
    result *= 2;
  }
  else if (g_eeGeneral.beeperVal == 6) {
    //xlong
    result *= 3;
  }
  return result;
}

void audioQueue::playNow(uint8_t tFreq, uint8_t tLen, uint8_t tPause,
    uint8_t tRepeat, uint8_t tHaptic, int8_t tFreqIncr)
{
  if (g_eeGeneral.beeperVal) {
    toneFreq = (tFreq ? tFreq + g_eeGeneral.speakerPitch + BEEP_OFFSET : 0); // add pitch compensator
    toneTimeLeft = getToneLength(tLen);
    tonePause = tPause;
#if defined(HAPTIC)
    toneHaptic = tHaptic;
#endif
    toneFreqIncr = tFreqIncr;
    t_queueWidx = t_queueRidx;

    if (tRepeat) {
      playASAP(tFreq, tLen, tPause, tRepeat-1, tHaptic, tFreqIncr);
    }
  }
}

void audioQueue::playASAP(uint8_t tFreq, uint8_t tLen, uint8_t tPause,
    uint8_t tRepeat, uint8_t tHaptic, int8_t tFreqIncr)
{
  if (g_eeGeneral.beeperVal) {
    uint8_t next_queueWidx = (t_queueWidx + 1) % AUDIO_QUEUE_LENGTH;
    if (next_queueWidx != t_queueRidx) {
      queueToneFreq[t_queueWidx] = (tFreq ? tFreq + g_eeGeneral.speakerPitch + BEEP_OFFSET : 0); // add pitch compensator
      queueToneLength[t_queueWidx] = getToneLength(tLen);
      queueTonePause[t_queueWidx] = tPause;
#if defined(HAPTIC)
      queueToneHaptic[t_queueWidx] = tHaptic;
#endif
      queueToneRepeat[t_queueWidx] = tRepeat;
      queueToneFreqIncr[t_queueWidx] = tFreqIncr;
      t_queueWidx = next_queueWidx;
    }
  }
}

#ifdef FRSKY
void audioQueue::frskyevent(uint8_t e)
{
  if (empty()) {
    // example playASAP(tStart,tLen,tPause,tRepeat,tHaptic,tEnd);
    switch(e) {
      case AU_FRSKY_WARN1:
        playASAP(BEEP_DEFAULT_FREQ+20,15,5,2,1);
        break;
      case AU_FRSKY_WARN2:
        playASAP(BEEP_DEFAULT_FREQ+30,15,5,2,1);
        break;
      case AU_FRSKY_CHEEP:
        playASAP(BEEP_DEFAULT_FREQ+30,10,2,2,1,2);
        break;
      case AU_FRSKY_RING:
        playASAP(BEEP_DEFAULT_FREQ+25,5,2,10,1);
        playASAP(BEEP_DEFAULT_FREQ+25,5,10,1,1);
        playASAP(BEEP_DEFAULT_FREQ+25,5,2,10,1);
        break;
      case AU_FRSKY_SCIFI:
        playASAP(80,10,3,2,0,-1);
        playASAP(60,10,3,2,0,1);
        playASAP(70,10,1,0,2);
        break;
      case AU_FRSKY_ROBOT:
        playASAP(70,5,1,1,1);
        playASAP(50,15,2,1,1);
        playASAP(80,15,2,1,1);
        break;
      case AU_FRSKY_CHIRP:
        playASAP(BEEP_DEFAULT_FREQ+40,5,1,2,1);
        playASAP(BEEP_DEFAULT_FREQ+54,5,1,3,1);
        break;
      case AU_FRSKY_TADA:
        playASAP(50,5,5);
        playASAP(90,5,5);
        playASAP(110,3,4,2);
        break;
      case AU_FRSKY_CRICKET:
        playASAP(80,5,10,3,1);
        playASAP(80,5,20,1,1);
        playASAP(80,5,10,3,1);
        break;
      case AU_FRSKY_SIREN:
        playASAP(10,20,5,2,1,1);
        break;
      case AU_FRSKY_ALARMC:
        playASAP(50,4,10,2,1);
        playASAP(70,8,20,1,1);
        playASAP(50,8,10,2,1);
        playASAP(70,4,20,1,1);
        break;
      case AU_FRSKY_RATATA:
        playASAP(BEEP_DEFAULT_FREQ+50,5,10,10,1);
        break;
      case AU_FRSKY_TICK:
        playASAP(BEEP_DEFAULT_FREQ+50,5,50,2,1);
        break;
      case AU_FRSKY_HAPTIC1:
        playASAP(0,20,10,1,1);
        break;
      case AU_FRSKY_HAPTIC2:
        playASAP(0,20,10,2,1);
        break;
      case AU_FRSKY_HAPTIC3:
        playASAP(0,20,10,3,1);
        break;
      default:
        break;
    }
  }
}
#endif

void audioQueue::event(uint8_t e, uint8_t f)
{
  uint8_t beepVal = g_eeGeneral.beeperVal;

  switch (e) {
    //startup tune
    // case 0:
    case AU_TADA:
      if (empty()) {
        playASAP(50, 10, 5);
        playASAP(90, 10, 5);
        playASAP(110, 5, 4, 2);
      }
      break;

      //warning one
      // case 1:
    case AU_WARNING1:
      playNow(BEEP_DEFAULT_FREQ, 10, 1, 0, 1);
      break;

      //warning two
      //case 2:
    case AU_WARNING2:
      playNow(BEEP_DEFAULT_FREQ, 20, 1, 0, 1);
      break;

      //warning three
      //case 3:
    case AU_WARNING3:
      playNow(BEEP_DEFAULT_FREQ, 30, 1, 0, 1);
      break;

      //error
      //case 4:
    case AU_ERROR:
      playNow(BEEP_DEFAULT_FREQ, 40, 1, 0, 1);
      break;

      //keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
      //case 5:
    case AU_KEYPAD_UP:
      if (beepVal != BEEP_NOKEYS) {
        playNow(BEEP_KEY_UP_FREQ, 10, 1);
      }
      break;

      //keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
      //case 6:
    case AU_KEYPAD_DOWN:
      if (beepVal != BEEP_NOKEYS) {
        playNow(BEEP_KEY_DOWN_FREQ, 10, 1);
      }
      break;

      //trim sticks move
      //case 7:
    case AU_TRIM_MOVE:
      //if(beepVal != BEEP_NOKEYS){
      playNow(f, 6, 1);
      //}
      break;

      //trim sticks center
      //case 8:
    case AU_TRIM_MIDDLE:
      //if(beepVal != BEEP_NOKEYS){
      playNow(BEEP_DEFAULT_FREQ, 10, 2, 0, 1);
      //}
      break;

      //menu display (also used by a few generic beeps)
      //case 9:
    case AU_MENUS:
      if (beepVal != BEEP_NOKEYS) {
        playNow(BEEP_DEFAULT_FREQ, 10, 2, 0, 0);
      }
      break;
      //pot/stick center
      //case 10:
    case AU_POT_STICK_MIDDLE:
      playNow(BEEP_DEFAULT_FREQ + 50, 10, 1, 0, 0);
      break;

      //mix warning 1
      //case 11:
    case AU_MIX_WARNING_1:
      playNow(BEEP_DEFAULT_FREQ + 50, 10, 1, 1, 1);
      break;

      //mix warning 2
      //case 12:
    case AU_MIX_WARNING_2:
      playNow(BEEP_DEFAULT_FREQ + 52, 10, 1, 2, 1);
      break;

      //mix warning 3
      //case 13:
    case AU_MIX_WARNING_3:
      playNow(BEEP_DEFAULT_FREQ + 54, 10, 1, 3, 1);
      break;

      //time 30 seconds left
      //case 14:
    case AU_TIMER_30:
      playNow(BEEP_DEFAULT_FREQ + 50, 15, 3, 3, 1);
      break;

      //time 20 seconds left
      //case 15:
    case AU_TIMER_20:
      playNow(BEEP_DEFAULT_FREQ + 50, 15, 3, 2, 1);
      break;

      //time 10 seconds left
      //case 16:
    case AU_TIMER_10:
      playNow(BEEP_DEFAULT_FREQ + 50, 15, 3, 1, 1);
      break;

      //time <3 seconds left
      //case 17:
    case AU_TIMER_LT3:
      playNow(BEEP_DEFAULT_FREQ, 20, 25, 1, 1);
      break;

      //inactivity timer alert
      //case 18:
    case AU_INACTIVITY:
      playNow(70, 10, 2,2);
      break;

      //low battery in tx
      //case 19:
    case AU_TX_BATTERY_LOW:
      if (empty()) {
        playASAP(60, 20, 3, 2, 0, 1);
        playASAP(80, 20, 3, 2, 1, -1);
      }
      break;

    default:
      break;
  }
}

void audioDefevent(uint8_t e)
{
  audio.event(e, BEEP_DEFAULT_FREQ);
}
