/*
 * Author - Rob Thomson <rob@marotori.com>
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

void audioQueue::aqinit()
{
  //make sure haptic off by default

  HAPTIC_OFF;

  //initialize all arrays
  flushqueue(0);

  //set key vars to 0 to ensure no garbage
  toneFreq = 0;
  toneFreqEnd = 0;
  toneRepeat = 0;
  toneTimeLeft = 0;
  queueState = 0;
  toneRepeatCnt = 0;
  inToneRepeat = 0;

#ifdef HAPTIC
  hapticTick = 0;
#endif

#ifdef FRSKY
  frskySample = 0;
#endif    

  heartbeatTimer = 0;
  flushTemp();
}

bool audioQueue::busy()
{
  if (toneTimeLeft > 0) {
    return true;
  }
  else {
    return false;
  }
}

bool audioQueue::freeslots(uint8_t slots)
{
  //find out if enough queue space exists to add the requested routine!
  for (uint8_t i = AUDIO_QUEUE_LENGTH; i--;) {
    slots--;
    if (slots == 0) {
      break;
    } //only loop for the number of items indicated in slots
    if (queueToneStart[i] != 0) {
      return false;
      break;
    }
  }
  return true;
}

void audioQueue::commit(uint8_t toneInterupt)
{

  if (toneInterupt == 0) {
    //queued events
    if (freeslots(3)) { //do not insert into the queue if less than 3 spare slots! this stops sound collitions from occuring
      for (uint8_t i = 0; i < AUDIO_QUEUE_LENGTH; i++) {
        if (queueToneStart[i] == 0) { //add to the first entry that has a start tone of zero (no sound)
          queueToneStart[i] = t_queueToneStart;
          queueToneEnd[i] = t_queueToneEnd;
          queueToneLength[i] = t_queueToneLength;
          queueTonePause[i] = t_queueTonePause;
          queueToneRepeat[i] = t_queueToneRepeat;
#ifdef HAPTIC
          queueToneHaptic[i] = t_queueToneHaptic;
#endif
          flushTemp();
          break;
        }
      }
    }
  }
  else {
    //interrupt events
    queueToneStart[0] = t_queueToneStart;
    queueToneEnd[0] = t_queueToneEnd;
    queueToneLength[0] = t_queueToneLength;
    queueTonePause[0] = t_queueTonePause;
    queueToneRepeat[0] = t_queueToneRepeat;
#ifdef HAPTIC
    queueToneHaptic[0] = t_queueToneHaptic;
#endif

    flushqueue(1); //purge queue on interrupt events to stop broken audio
    flushTemp();
  }

}

void audioQueue::flushqueue(uint8_t startpos)
{
  for (; startpos < AUDIO_QUEUE_LENGTH; startpos++) {
    queueToneStart[startpos] = 0;
    queueToneEnd[startpos] = 0;
    queueToneLength[startpos] = 0;
    queueTonePause[startpos] = 0;
    queueToneRepeat[startpos] = 0;
#ifdef HAPTIC
    queueToneHaptic[startpos] = 0;
#endif
  }
}

//set all temporary buffers to default
void audioQueue::flushTemp()
{
  t_queueToneStart = 0;
  t_queueToneEnd = 0;
  t_queueToneLength = 0;
  t_queueTonePause = 0;
  t_queueToneRepeat = 0;
#ifdef HAPTIC
  t_queueToneHaptic = 0;
#endif
  rateOfChange = 0;
#ifdef HAPTIC
  toneHaptic = 0;
#endif
}

void audioQueue::restack()
{
  for (uint8_t i = 0; i < AUDIO_QUEUE_LENGTH - 1; i++) {
    queueToneStart[i] = queueToneStart[i + 1];
    queueToneEnd[i] = queueToneEnd[i + 1];
    queueToneLength[i] = queueToneLength[i + 1];
    queueTonePause[i] = queueTonePause[i + 1];
    queueToneRepeat[i] = queueToneRepeat[i + 1];
#ifdef HAPTIC
    queueToneHaptic[i] = queueToneHaptic[i + 1];
#endif
  }
  flushqueue(AUDIO_QUEUE_LENGTH - 1); //set the last entry to 0 as nothing in stack to add too!
}

//heartbeat is responsibile for issueing the audio tones and general square waves
// it is essentially the life of the class.
void audioQueue::heartbeat()
{

  uint8_t z; //direction calulations
  uint8_t hTimer; //heartbeat timer

  if (queueState == 1) {

    if (g_eeGeneral.beeperVal > 0) {
      //never do sounds if we are set to go quiet
#if 0
      switch (g_eeGeneral.speakerMode) {

        case 0:
          //stock beeper. simply turn port on for x time!
          if (toneTimeLeft > 0) {
#if defined (PCBV4)
            TCCR0A &= ~(0b01<<COM0A0);
#else
            PORTE |= (1 << OUT_E_BUZZER); // speaker output 'high'
#endif
          }
          else {
#if defined (PCVV4)
            TCCR0A &= ~(0b01<<COM0A0);
#else
            PORTE &= ~(1 << OUT_E_BUZZER); // speaker output 'low'
#endif
          }
          break;

        case 1:
#endif
          //square wave generator use for speaker mod
          //simply generates a square wave for toneFreq for
          //as long as the toneTimeLeft is more than 0
          static uint8_t toneCounter;
          if (toneTimeLeft > 0) {
            toneCounter += toneFreq;
            if ((toneCounter & 0x80) == 0x80) {
#if defined (PCBV4)
              TCCR0A &= ~(0b01<<COM0A0);
#else
              PORTE |= (1 << OUT_E_BUZZER); // speaker output 'high'
#endif

            }
            else {
#if defined (PCVV3)
              TCCR0A &= ~(0b01<<COM0A0);
#else
              PORTE &= ~(1 << OUT_E_BUZZER); // speaker output 'low'
#endif
            }
          }
          else {
#if defined (PCVV3)
            TCCR0A &= ~(0b01<<COM0A0);
#else
            PORTE &= ~(1 << OUT_E_BUZZER); // speaker output 'low'
#endif
          }
#if 0
          break;

        case 2:
          //PCMWav

          break;

      }
#endif

    }

#ifdef HAPTIC
    uint8_t hapticStrength = g_eeGeneral.hapticStrength;
    if (toneHaptic == 1) {
      if ((hapticTick <= hapticStrength - 1) && hapticStrength > 0) {
        HAPTIC_ON; // haptic output 'high'
        hapticTick++;
      }
      else {
        HAPTIC_OFF; //haptic output low
        hapticTick = 0;
      }
    }
    else {
      HAPTIC_OFF; // haptic output 'low'
    }
#endif

  }
  else {
    PORTE &= ~(1 << OUT_E_BUZZER); // speaker output 'low'
    HAPTIC_OFF;
  }

  //step through array checking if we have any tones to play
  //next heartbeat will play whatever we put in queue
  if ((queueToneStart[0] > 0
#ifdef HAPTIC
      || queueToneHaptic[0] == 1
#endif
      ) && toneTimeLeft <= 0
      && queueState == 0) {
    if (queueToneEnd[0] > 0 && queueToneEnd[0] != queueToneStart[0]) {
      if (queueToneStart[0] > queueToneEnd[0]) { //tone going down
        z = queueToneStart[0] - queueToneEnd[0];
        rateOfChange = -2;
      }
      else { //tone going up
        z = queueToneEnd[0] - queueToneStart[0];
        rateOfChange = 2;
      }
      if (queueToneStart[0] > 0) {
        toneFreq = queueToneStart[0] + g_eeGeneral.speakerPitch + BEEP_OFFSET; // add pitch compensator
      }
      else {
        toneFreq = queueToneStart[0]; // done so hapticOnly option can work
      }
      toneFreqEnd = queueToneEnd[0] + g_eeGeneral.speakerPitch + BEEP_OFFSET;
      toneTimeLeft = z;
      tonePause = queueTonePause[0];
      toneRepeat = queueToneRepeat[0];
#ifdef HAPTIC
      toneHaptic = queueToneHaptic[0];
#endif
    }
    else {
      //simple tone handler
      if (queueToneStart[0] > 0) {
        toneFreq = (queueToneStart[0] + g_eeGeneral.speakerPitch) + BEEP_OFFSET; // add pitch compensator
      }
      else {
        toneFreq = queueToneStart[0];
      }
      rateOfChange = 0;
      toneFreqEnd = 0;
      toneTimeLeft = queueToneLength[0];
      tonePause = queueTonePause[0];
      toneRepeat = queueToneRepeat[0];
#ifdef HAPTIC
      toneHaptic = queueToneHaptic[0];
#endif
    }
    queueState = 1;
    if (toneRepeat != 0 && inToneRepeat == 0) {
      inToneRepeat = 1;
      toneRepeatCnt = toneRepeat;
    }

    if (inToneRepeat == 1) {
      toneRepeatCnt--;
      if (toneRepeatCnt <= 0) {
        inToneRepeat = 0;
        restack();
      }
    }
    else {
      restack();
    }

  }

  hTimer = AUDIO_QUEUE_HEARTBEAT_NORM; // default
  if (g_eeGeneral.beeperVal == 2) {
    //xshort
    hTimer = AUDIO_QUEUE_HEARTBEAT_XSHORT;
  }
  else if (g_eeGeneral.beeperVal == 3) {
    //short
    hTimer = AUDIO_QUEUE_HEARTBEAT_SHORT;
  }
  else if (g_eeGeneral.beeperVal == 5) {
    //long
    hTimer = AUDIO_QUEUE_HEARTBEAT_LONG;
  }
  else if (g_eeGeneral.beeperVal == 6) {
    //xlong
    hTimer = AUDIO_QUEUE_HEARTBEAT_XLONG;
  }

  heartbeatTimer++;
  if (heartbeatTimer >= hTimer) {

    heartbeatTimer = 0;

    if (queueState == 1) {
      if (toneTimeLeft > 0) {
        toneFreq += rateOfChange; // -2, 0 or 2
        toneTimeLeft--; //time gets counted down
      }
      if (toneTimeLeft <= 0) {
        if (tonePause-- <= 0) {
          queueState = 0;
        }
      }
    }

  }

}

void audioQueue::playNow(uint8_t tStart, uint8_t tLen, uint8_t tPause,
    uint8_t tRepeat, uint8_t tHaptic, uint8_t tEnd)
{
  t_queueToneStart = tStart;
  t_queueToneLength = tLen;
  t_queueTonePause = tPause;
#ifdef HAPTIC
  t_queueToneHaptic = tHaptic;
#endif
  t_queueToneRepeat = tRepeat;
  t_queueToneEnd = tEnd;
  commit(1);
}

void audioQueue::playASAP(uint8_t tStart, uint8_t tLen, uint8_t tPause,
    uint8_t tRepeat, uint8_t tHaptic, uint8_t tEnd)
{
  t_queueToneStart = tStart;
  t_queueToneLength = tLen;
  t_queueTonePause = tPause;
#ifdef HAPTIC
  t_queueToneHaptic = tHaptic;
#endif
  t_queueToneRepeat = tRepeat;
  t_queueToneEnd = tEnd;
  commit(0);
}

#ifdef FRSKY

//this is done so the menu selections only plays tone once!
void audioQueue::frskyeventSample(uint8_t e) {
  if(frskySample != e) {
    aqinit(); //flush the queue
    frskyevent(e);
    frskySample = e;
  }
}

void audioQueue::frskyevent(uint8_t e) {
  // example playASAP(tStart,tLen,tPause,tRepeat,tHaptic,tEnd);
  switch(e) {
    case AU_FRSKY_WARN1:
    playASAP(BEEP_DEFAULT_FREQ+20,25,5,2,1);
    break;
    case AU_FRSKY_WARN2:
    playASAP(BEEP_DEFAULT_FREQ+30,25,5,2,1);
    break;
    case AU_FRSKY_CHEEP:
    playASAP(BEEP_DEFAULT_FREQ+30,20,2,2,1,BEEP_DEFAULT_FREQ+25);
    break;
    case AU_FRSKY_RING:
    playASAP(BEEP_DEFAULT_FREQ+25,2,2,10,1);
    playASAP(BEEP_DEFAULT_FREQ+25,2,10,1,1);
    playASAP(BEEP_DEFAULT_FREQ+25,2,2,10,1);
    break;
    case AU_FRSKY_SCIFI:
    playASAP(80,4,3,2,0,70);
    playASAP(60,4,3,2,0,70);
    playASAP(70,2,1,0,2);
    break;
    case AU_FRSKY_ROBOT:
    playASAP(70,2,1,1,1);
    playASAP(50,6,2,1,1);
    playASAP(80,6,2,1,1);
    break;
    case AU_FRSKY_CHIRP:

    playASAP(BEEP_DEFAULT_FREQ+40,2,1,2,1);
    playASAP(BEEP_DEFAULT_FREQ+54,2,1,3,1);
    break;
    case AU_FRSKY_TADA:
    playASAP(50,10,5);
    playASAP(90,10,5);
    playASAP(110,6,4,2);
    break;
    case AU_FRSKY_CRICKET:
    playASAP(80,1,10,3,1);
    playASAP(80,1,20,1,1);
    playASAP(80,1,10,3,1);
    break;
    case AU_FRSKY_SIREN:
    playASAP(10,5,5,2,1,40);
    break;
    case AU_FRSKY_ALARMC:
    playASAP(50,5,10,2,1);
    playASAP(70,5,20,1,1);
    playASAP(50,5,10,2,1);
    playASAP(70,5,20,1,1);
    break;
    case AU_FRSKY_RATATA:
    playASAP(BEEP_DEFAULT_FREQ+50,2,10,10,1);
    break;
    case AU_FRSKY_TICK:
    playASAP(BEEP_DEFAULT_FREQ+50,2,50,2,1);
    break;
    case AU_FRSKY_HAPTIC1:
    playASAP(0,2,10,1,1);
    break;
    case AU_FRSKY_HAPTIC2:
    playASAP(0,2,10,2,1);
    break;
    case AU_FRSKY_HAPTIC3:
    playASAP(0,2,10,3,1);
    break;
    default:
    break;
  }

}
#endif

void audioQueue::event(uint8_t e, uint8_t f) {

  uint8_t beepVal = g_eeGeneral.beeperVal;

  switch (e) {
    //startup tune
    // case 0:
    case AU_TADA:
      playASAP(50, 10, 5);
      playASAP(90, 10, 5);
      playASAP(110, 6, 4, 2);
      break;

      //warning one
      // case 1:
    case AU_WARNING1:
      playNow(BEEP_DEFAULT_FREQ, 25, 1, 0, 1);
      break;

      //warning two
      //case 2:
    case AU_WARNING2:
      playNow(BEEP_DEFAULT_FREQ, 34, 1, 0, 1);
      break;

      //warning three
      //case 3:
    case AU_WARNING3:
      playNow(BEEP_DEFAULT_FREQ, 15, 1, 0, 1);
      break;

      //error
      //case 4:
    case AU_ERROR:
      playNow(BEEP_DEFAULT_FREQ, 30, 1, 0, 1);
      break;

      //keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
      //case 5:
    case AU_KEYPAD_UP:
      if (beepVal != BEEP_NOKEYS) {
        playNow(BEEP_KEY_UP_FREQ, 2, 1);
      }
      break;

      //keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
      //case 6:
    case AU_KEYPAD_DOWN:
      if (beepVal != BEEP_NOKEYS) {
        playNow(BEEP_KEY_DOWN_FREQ, 2, 1);
      }
      break;

      //trim sticks move
      //case 7:
    case AU_TRIM_MOVE:
      //if(beepVal != BEEP_NOKEYS){
      playNow(f, 2, 1);
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
        playNow(BEEP_DEFAULT_FREQ, 2, 2, 0, 1);
      }
      break;
      //pot/stick center
      //case 10:
    case AU_POT_STICK_MIDDLE:
      playNow(BEEP_DEFAULT_FREQ + 50, 3, 1, 0, 1);
      break;

      //mix warning 1
      //case 11:
    case AU_MIX_WARNING_1:
      playNow(BEEP_DEFAULT_FREQ + 50, 2, 1, 1, 1);
      break;

      //mix warning 2
      //case 12:
    case AU_MIX_WARNING_2:
      playNow(BEEP_DEFAULT_FREQ + 52, 2, 1, 2, 1);
      break;

      //mix warning 3
      //case 13:
    case AU_MIX_WARNING_3:
      playNow(BEEP_DEFAULT_FREQ + 54, 2, 1, 3, 1);
      break;

      //time 30 seconds left
      //case 14:
    case AU_TIMER_30:
      playNow(BEEP_DEFAULT_FREQ + 50, 5, 3, 3, 1);
      break;

      //time 20 seconds left
      //case 15:
    case AU_TIMER_20:
      playNow(BEEP_DEFAULT_FREQ + 50, 5, 3, 2, 1);
      break;

      //time 10 seconds left
      //case 16:
    case AU_TIMER_10:
      playNow(BEEP_DEFAULT_FREQ + 50, 5, 3, 1, 1);
      break;

      //time <3 seconds left
      //case 17:
    case AU_TIMER_LT3:
      playNow(BEEP_DEFAULT_FREQ, 20, 5, 1, 1);
      break;

      //inactivity timer alert
      //case 18:
    case AU_INACTIVITY:
      playASAP(70, 3, 2);
      playASAP(50, 3, 5);
      break;

      //low battery in tx
      //case 19:
    case AU_TX_BATTERY_LOW:
      playASAP(60, 4, 3, 2, 1, 70);
      playASAP(80, 4, 3, 2, 1, 70);
      break;

    default:
      break;

  }

}

void audioDefevent(uint8_t e) {
  audio.event(e, BEEP_DEFAULT_FREQ);
}

