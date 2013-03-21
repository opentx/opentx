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


// Bits in VoiceLatch
#define VOICE_CLOCK_BIT         0x01
#define VOICE_DATA_BIT          0x02
#define BACKLIGHT_BIT           0x04
#define SPARE_BIT               0x08

#if defined(CPUM64)
  #define VOICE_Q_LENGTH        8
#else
  #define VOICE_Q_LENGTH        16
#endif

// Voice states
#define V_STARTUP               0
#define V_IDLE                  1
#define V_CLOCKING              2
#define V_WAIT_BUSY_ON          3
#define V_WAIT_BUSY_OFF         4
#define V_WAIT_BUSY_DELAY       5
#define V_WAIT_START_BUSY_OFF   6

#define PROMPT_SYSTEM_BASE      230
#define PROMPT_CUSTOM_BASE      256

struct t_voice
{
    uint16_t VoiceQueue[VOICE_Q_LENGTH] ;
    uint8_t Backlight ;
    uint8_t VoiceLatch ;
    uint8_t VoiceCounter ;
    uint8_t VoiceTimer ;
    uint16_t VoiceSerial ;
    uint8_t VoiceState ;
    uint8_t VoiceQueueCount ;
    uint8_t VoiceQueueInIndex ;
    uint8_t VoiceQueueOutIndex ;

    void voice_process( void ) ;
};

extern void pushCustomPrompt( uint8_t value );
extern void pushPrompt( uint8_t value );
extern void pushPrompt16(uint16_t value);
extern struct t_voice *voiceaddress( void ) ;
extern struct t_voice Voice ;

#define VOLUME_LEVEL_MAX  7
#define VOLUME_LEVEL_DEF  7
#define setVolume(v) pushPrompt16((v) | 0xFFF0)

inline bool isPlaying()
{
#if defined(SIMU)
  return false;
#else
  return Voice.VoiceState != V_IDLE;
#endif
}

/*
 * Handle the Voice output
 * Check for LcdLocked (in interrupt), and voice_enabled
 */
#define VOICE_DRIVER() \
  if ( LcdLock == 0 ) /* LCD not in use */ \
  { \
    struct t_voice *vptr; \
    vptr = voiceaddress(); \
    if ( vptr->VoiceState == V_CLOCKING ) \
    { \
      if ( vptr->VoiceTimer ) \
      { \
        vptr->VoiceTimer -= 1; \
      } \
      else \
      { \
        PORTB |= (1<<OUT_B_LIGHT); /* Latch clock high */ \
        if ((vptr->VoiceCounter & 1) == 0) \
        { \
          vptr->VoiceLatch &= ~VOICE_DATA_BIT; \
          if ( vptr->VoiceSerial & 0x4000 ) \
          { \
            vptr->VoiceLatch |= VOICE_DATA_BIT; \
          } \
          vptr->VoiceSerial <<= 1; \
        } \
        vptr->VoiceLatch ^= VOICE_CLOCK_BIT; \
        PORTA_LCD_DAT = vptr->VoiceLatch; /* Latch data set */ \
        PORTB &= ~(1<<OUT_B_LIGHT); /* Latch clock low */ \
        if ( --vptr->VoiceCounter == 0 ) \
        { \
          vptr->VoiceState = V_WAIT_BUSY_ON; \
          vptr->VoiceTimer = 5; /* 50 mS */ \
        } \
      } \
    } \
  }

#define PLAY_FUNCTION(x, ...) void x(__VA_ARGS__)
#define PUSH_PROMPT(p) pushPrompt((p))
#define PUSH_CUSTOM_PROMPT(p, ...) pushCustomPrompt((p))
#define PLAY_NUMBER(n, u, a) playNumber((n), (u), (a))
#define PLAY_DURATION(d) playDuration((d))
#define IS_PLAYING(id) (0) /* isPlaying() */
#define PLAY_VALUE(v, id) playValue((v))
