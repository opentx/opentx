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

#ifndef audio_h
#define audio_h

//audio
#define AUDIO_QUEUE_LENGTH (8)  //8 seems to suit most alerts
#define BEEP_DEFAULT_FREQ (70)
#define BEEP_OFFSET (10)
#define BEEP_KEY_UP_FREQ  (BEEP_DEFAULT_FREQ+5)
#define BEEP_KEY_DOWN_FREQ (BEEP_DEFAULT_FREQ-5)

#define AUDIO_QUEUE_HEARTBEAT_XSHORT (30) //speaker timing [Norm]
#define AUDIO_QUEUE_HEARTBEAT_SHORT (50) //speaker timing [Norm]
#define AUDIO_QUEUE_HEARTBEAT_NORM (77) //speaker timing [Norm]
#define AUDIO_QUEUE_HEARTBEAT_LONG (130) //speaker timing [Long]
#define AUDIO_QUEUE_HEARTBEAT_XLONG (200) //speaker timing [xLong]
#ifdef HAPTIC
#define HAPTIC_ON    PORTG |=  (1<<2)
#define HAPTIC_OFF   PORTG &= ~(1<<2)
#else
#define HAPTIC_ON
#define HAPTIC_OFF
#endif

/* make sure the defines below always go in numeric order */
#define AU_TADA (0)
#define AU_WARNING1 (1)
#define AU_WARNING2 (2)
#define AU_WARNING3 (3)
#define AU_ERROR (4)
#define AU_KEYPAD_UP (5)
#define AU_KEYPAD_DOWN (6)
#define AU_TRIM_MOVE (7)
#define AU_TRIM_MIDDLE (8)
#define AU_MENUS (9)
#define AU_POT_STICK_MIDDLE (10)
#define AU_MIX_WARNING_1 (11)
#define AU_MIX_WARNING_2 (12)
#define AU_MIX_WARNING_3 (13)
#define AU_TIMER_30 (14)
#define AU_TIMER_20 (15)
#define AU_TIMER_10 (16)
#define AU_TIMER_LT3 (17)
#define AU_INACTIVITY (18)
#define AU_TX_BATTERY_LOW (19)

#ifdef FRSKY	

#define AU_FRSKY_WARN1 (0)
#define AU_FRSKY_WARN2 (1)
#define AU_FRSKY_CHEEP (2)
#define AU_FRSKY_RING (3)
#define AU_FRSKY_SCIFI (4)
#define AU_FRSKY_ROBOT (5)
#define AU_FRSKY_CHIRP (6)
#define AU_FRSKY_TADA (7)
#define AU_FRSKY_CRICKET (8)
#define AU_FRSKY_SIREN (9)
#define AU_FRSKY_ALARMC (10)
#define AU_FRSKY_RATATA (11)
#define AU_FRSKY_TICK (12)
#define AU_FRSKY_HAPTIC1 (13)
#define AU_FRSKY_HAPTIC2 (14)
#define AU_FRSKY_HAPTIC3 (15)
#endif

#define BEEP_QUIET (0)
#define BEEP_NOKEYS (1)
#define BEEP_XSHORT (2)
#define BEEP_SHORT (3)
#define BEEP_NORMAL (4)
#define BEEP_LONG (5)
#define BEEP_XLONG (6)

struct audioQueue {

  //queue temporaries
  uint8_t t_queueToneStart;
  uint8_t t_queueToneEnd;
  uint8_t t_queueToneLength;
  uint8_t t_queueTonePause;
  uint8_t t_queueToneRepeat;
#ifdef HAPTIC
  uint8_t t_queueToneHaptic;
#endif

  //queue general vars
  uint8_t toneFreq;
  uint8_t toneFreqEnd;
  uint8_t toneTimeLeft;
  int8_t rateOfChange;
  uint8_t tonePause;
  uint8_t queueState;
  uint8_t toneRepeat;
  uint8_t toneRepeatCnt;
  uint8_t inToneRepeat;
#ifdef HAPTIC
  uint8_t toneHaptic;
  uint8_t hapticTick;
#endif
  uint8_t heartbeatTimer;

#ifdef FRSKY
  uint8_t frskySample;
#endif

  //queue arrays
  uint8_t queueToneStart[AUDIO_QUEUE_LENGTH];
  uint8_t queueToneEnd[AUDIO_QUEUE_LENGTH];
  uint8_t queueToneLength[AUDIO_QUEUE_LENGTH];
  uint8_t queueTonePause[AUDIO_QUEUE_LENGTH];
  uint8_t queueToneRepeat[AUDIO_QUEUE_LENGTH];
#ifdef HAPTIC
  uint8_t queueToneHaptic[AUDIO_QUEUE_LENGTH];
#endif

  //beep length table
  //    uint8_t beepLenTable[10];

  audioQueue();

  void aqinit(); // To stop constructor being compiled twice

  //only difference between these two functions is that one does the
  //interupt queue (Now) and the other queues for playing ASAP.
  void playNow(uint8_t tStart,uint8_t tLen,uint8_t tPause,uint8_t tRepeat=0,uint8_t tHaptic=0,uint8_t tEnd=0);
  void playASAP(uint8_t tStart,uint8_t tLen,uint8_t tPause,uint8_t tRepeat=0,uint8_t tHaptic=0,uint8_t tEnd=0);

  bool busy();

  void event(uint8_t e,uint8_t f=BEEP_DEFAULT_FREQ);

#ifdef FRSKY		
  void frskyevent(uint8_t e);
  void frskyeventSample(uint8_t e);
#endif

  void commit(uint8_t toneInterupt);

  //set all temporary buffers to default
  void flushTemp();
  void flushqueue(uint8_t startpos);

  void restack();
  //heartbeat is responsibile for issueing the audio tones and general square waves
  // it is essentially the life of the class.
  void heartbeat();
  bool freeslots(uint8_t slots);
};

//wrapper function - dirty but results in a space saving!!!
extern audioQueue audio;
//void audioevent(uint8_t e,uint8_t f=BEEP_DEFAULT_FREQ);
void audioDefevent(uint8_t e);

void audioError();

#define AUDIO_KEYPAD_UP()   audioDefevent(AU_KEYPAD_UP)
#define AUDIO_KEYPAD_DOWN() audioDefevent(AU_KEYPAD_DOWN)
#define AUDIO_MENUS()       audioDefevent(AU_MENUS)
#define AUDIO_WARNING1()    audioDefevent(AU_WARNING1)
#define AUDIO_WARNING2()    audioDefevent(AU_WARNING2)
#define AUDIO_ERROR()       audioDefevent(AU_ERROR)

#define IS_AUDIO_BUSY()     audio.busy()

#define AUDIO_TIMER_30()    audioDefevent(AU_TIMER_30)
#define AUDIO_TIMER_20()    audioDefevent(AU_TIMER_20)
#define AUDIO_TIMER_10()    audioDefevent(AU_TIMER_10)
#define AUDIO_TIMER_LT3()   audioDefevent(AU_TIMER_LT3)
#define AUDIO_MINUTE_BEEP() audioDefevent(AU_WARNING1)
#define AUDIO_INACTIVITY()  audioDefevent(AU_INACTIVITY)
#define AUDIO_MIX_WARNING_1() audioDefevent(AU_MIX_WARNING_1)
#define AUDIO_MIX_WARNING_3() audioDefevent(AU_MIX_WARNING_3)

#define AUDIO_HEARTBEAT()   audio.heartbeat()

#endif // audio_h
