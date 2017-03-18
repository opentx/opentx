/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

audioQueue::audioQueue()
{
  toneTimeLeft = 0;
  tonePause = 0;
  tone2TimeLeft = 0;

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
#else
  if (toneTimeLeft > 0) {
    if (toneFreq == 0) {  //pause only events
      speakerOff();
    }
    else {
#if defined(CPUM2560)
      if (toneFreq) {
        OCR4A = (5000 / toneFreq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
        speakerOn();
      }
#endif
      toneFreq += toneFreqIncr;
    }
    toneTimeLeft--; //time gets counted down
  }
  else {
    if (tonePause > 0) {
      speakerOff();
      tonePause--; //time gets counted down
    }
    else if (t_queueRidx != t_queueWidx) {
      toneFreq = queueToneFreq[t_queueRidx];
      toneTimeLeft = queueToneLength[t_queueRidx];
      toneFreqIncr = queueToneFreqIncr[t_queueRidx];
      tonePause = queueTonePause[t_queueRidx];
      if (!queueToneRepeat[t_queueRidx]--) {
        t_queueRidx = (t_queueRidx + 1) & (AUDIO_QUEUE_LENGTH-1);
      }
    }
    else {
      if (tone2TimeLeft > 0) {
#if defined(CPUM2560)
        if (tone2Freq) {
          OCR4A = (5000 / tone2Freq); // sticking with old values approx 20(abs. min) to 90, 60 being the default tone(?).
          speakerOn();
        }
#else
        toneFreq = tone2Freq;
#endif
        tone2TimeLeft--; //time gets counted down
      }
      else {
        speakerOff();
      }
    }
  }
#endif // defined(SIMU)
}

inline uint8_t audioQueue::getToneLength(uint8_t tLen)
{
  uint8_t result = tLen; // default
  if (g_eeGeneral.beepLength < 0) {
    result /= (1-g_eeGeneral.beepLength);
  }
  if (g_eeGeneral.beepLength > 0) {
    result *= (1+g_eeGeneral.beepLength);
  }
  return result;
}

void audioQueue::pause(uint8_t tLen)
{
  play(0, tLen, 5); // a pause
}

void audioQueue::play(uint8_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tFlags)
{
  if (tFlags & PLAY_BACKGROUND) {
    tone2Freq = tFreq;
    tone2TimeLeft = tLen;
  }
  else {
    int8_t tFreqIncr = (tFlags >> 6);
    if (tFreqIncr == 3) tFreqIncr = -1;

    tFreq += g_eeGeneral.speakerPitch + BEEP_OFFSET; // add pitch compensator
    tLen = getToneLength(tLen);

    if ((tFlags & PLAY_NOW) || (!busy() && empty())) {
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
      uint8_t next_queueWidx = (t_queueWidx + 1) & (AUDIO_QUEUE_LENGTH-1);
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

void audioQueue::event(uint8_t e)
{
#if defined(HAPTIC)
  haptic.event(e); //do this before audio to help sync timings
#endif

  if (e <= AU_ERROR || (e >= AU_WARNING1 && e < AU_SPECIAL_SOUND_FIRST)) {
    if (g_eeGeneral.alarmsFlash) {
      flashCounter = FLASH_DURATION;
    }
  }

  if (g_eeGeneral.beepMode >= e_mode_nokeys || (g_eeGeneral.beepMode >= e_mode_alarms && e <= AU_ERROR)) {
    if (e < AU_SPECIAL_SOUND_FIRST || empty()) {
      // TODO when VOICE enable some cases here are not needed!
      switch (e) {
#if !defined(VOICE)
        case AU_TX_BATTERY_LOW:
          if (empty()) {
            play(60, 20, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(1));
            play(80, 20, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(-1));
          }
          break;
#endif
        case AU_SPECIAL_SOUND_RING:
          play(BEEP_DEFAULT_FREQ+25, 5, 2, PLAY_REPEAT(10));
          play(BEEP_DEFAULT_FREQ+25, 5, 10, PLAY_REPEAT(1));
          play(BEEP_DEFAULT_FREQ+25, 5, 2, PLAY_REPEAT(10));
          break;
        case AU_SPECIAL_SOUND_SCIFI:
          play(80, 10, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(-1));
          play(60, 10, 3, PLAY_REPEAT(2)|PLAY_INCREMENT(1));
          play(70, 10, 1);
          break;
        case AU_SPECIAL_SOUND_ROBOT:
          play(70, 5, 1, PLAY_REPEAT(1));
          play(50, 15, 2, PLAY_REPEAT(1));
          play(80, 15, 2, PLAY_REPEAT(1));
          break;
        case AU_SPECIAL_SOUND_CHIRP:
          play(BEEP_DEFAULT_FREQ+40, 5, 1, PLAY_REPEAT(2));
          play(BEEP_DEFAULT_FREQ+54, 5, 1, PLAY_REPEAT(3));
          break;
        case AU_SPECIAL_SOUND_TADA:
          play(50, 5, 5);
          play(90, 5, 5);
          play(110, 3, 4, PLAY_REPEAT(2));
          break;
        case AU_SPECIAL_SOUND_CRICKET:
          play(80, 5, 10, PLAY_REPEAT(3));
          play(80, 5, 20, PLAY_REPEAT(1));
          play(80, 5, 10, PLAY_REPEAT(3));
          break;
        case AU_SPECIAL_SOUND_ALARMC:
          play(50, 4, 10, PLAY_REPEAT(2));
          play(70, 8, 20, PLAY_REPEAT(1));
          play(50, 8, 10, PLAY_REPEAT(2));
          play(70, 4, 20, PLAY_REPEAT(1));
          break;
        default:
        {
          static const pm_uint8_t singleSounds[] PROGMEM = {
              70, 10, 2, PLAY_REPEAT(2)|PLAY_NOW,  // INACTIVITY
              BEEP_DEFAULT_FREQ, 40, 1, PLAY_NOW,  // ERROR
              BEEP_DEFAULT_FREQ, 10, 1, PLAY_NOW,  // WARNING1
              BEEP_DEFAULT_FREQ, 20, 1, PLAY_NOW,  // WARNING2
              BEEP_DEFAULT_FREQ, 30, 1, PLAY_NOW,  // WARNING3
              60, 10, 2, PLAY_NOW,         // TRIM_MIDDLE
              BEEP_DEFAULT_FREQ + 50, 10, 1, PLAY_NOW,      // POT_STICK_MIDDLE
              BEEP_DEFAULT_FREQ + 50, 6, 0, 0,              // MIX_WARNING_1
              BEEP_DEFAULT_FREQ + 52, 6, 3, PLAY_REPEAT(1), // MIX_WARNING_2
              BEEP_DEFAULT_FREQ + 54, 6, 3, PLAY_REPEAT(2), // MIX_WARNING_3
              BEEP_DEFAULT_FREQ, 10, 1, 0, // FRSKY_BEEP1
              BEEP_DEFAULT_FREQ, 20, 1, 0, // FRSKY_BEEP2
              BEEP_DEFAULT_FREQ, 30, 1, 0, // FRSKY_BEEP3
              BEEP_DEFAULT_FREQ+20, 15, 5, PLAY_REPEAT(2), // FRSKY_WARN1
              BEEP_DEFAULT_FREQ+30, 15, 5, PLAY_REPEAT(2), // FRSKY_WARN2
              BEEP_DEFAULT_FREQ+30, 10, 2, PLAY_REPEAT(2)|PLAY_INCREMENT(2), // FRSKY_CHEEP
              BEEP_DEFAULT_FREQ+50, 5, 10, PLAY_REPEAT(10), // FRSKY_RATATA
              BEEP_DEFAULT_FREQ+50, 5, 50, PLAY_REPEAT(2), // FRSKY_TICK
              10, 20, 5, PLAY_REPEAT(2)|PLAY_INCREMENT(1), // FRSKY_SIREN
          };

          const pm_uint8_t *ptr = &singleSounds[(e-AU_INACTIVITY)<<2];
          uint8_t tFreq = pgm_read_byte(ptr++);
          uint8_t tLen = pgm_read_byte(ptr++);
          uint8_t tPause = pgm_read_byte(ptr++);
          uint8_t tFlags = pgm_read_byte(ptr);
          play(tFreq, tLen, tPause, tFlags);
          break;
        }
      }
    }
  }
}

void audioDefevent(uint8_t e)
{
  audio.event(e);
}

void audioKeyPress()
{
#if defined(AUDIO)
  if (g_eeGeneral.beepMode == e_mode_all) {
    audio.play(BEEP_DEFAULT_FREQ, 10, 1, PLAY_NOW);
  }
#else
  beep(0);
#endif

#if defined(HAPTIC)
  if (g_eeGeneral.hapticMode == e_mode_all) {
    haptic.play(5, 0, PLAY_NOW);
  }
#endif
}

void audioTrimPress(int16_t value)
{
  if (g_eeGeneral.beepMode >= e_mode_nokeys) {
#if defined(AUDIO)
    value = limit<int16_t>(TRIM_MIN, value, TRIM_MAX);
    value >>= 2;
    value += 60;
    audio.play(value, 6, 1, PLAY_NOW);
#else
    warble = true;
    beep(1);
#endif
  }
}

void audioTimerCountdown(uint8_t timer, int value)
{
  if (0) {
    // pass
  }

#if defined(CPUM2560) && (defined(VOICE_WTV20) || defined(VOICE_JQ6500))
  else if (g_model.timers[timer].countdownBeep == COUNTDOWN_VOICE) {
    if (value >= 0 && value <= TIMER_COUNTDOWN_START(timer)) {
      playNumber(value, 0, 0);
    }
    else if (value == 30 || value == 20) {
      playDuration(value);
    }
  }
#endif

#if defined(HAPTIC)
  else if (g_model.timers[timer].countdownBeep == COUNTDOWN_HAPTIC) {
    if (value == 0) {
      haptic.play(15, 3, PLAY_NOW);
    }
    else if (value > 0 && value <= 10) {
      haptic.play(5, 0, PLAY_NOW);
    }
    else if (value == 30) {
      haptic.play(10, 3, PLAY_REPEAT(2) | PLAY_NOW);
    }
    else if (value == 20) {
      haptic.play(10, 3, PLAY_REPEAT(1) | PLAY_NOW);
    }
  }
#endif

  else {
    if (value == 0) {
      audio.play(BEEP_DEFAULT_FREQ + 50, 30, 3, PLAY_NOW);
    }
    else if (value > 0 && value <= 10) {
      audio.play(BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_NOW);
    }
    else if (value == 30) {
      audio.play(BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_REPEAT(2) | PLAY_NOW);
    }
    else if (value == 20) {
      audio.play(BEEP_DEFAULT_FREQ + 50, 15, 3, PLAY_REPEAT(1) | PLAY_NOW);
    }
  }
}
