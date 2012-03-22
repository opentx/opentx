/****************************************************************************
*  Copyright (c) 2011 by Michael Blandford. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may
*     be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
****************************************************************************/




/* make sure the defines below always go in numeric order */
#define AUDIO_TADA (0)
#define AUDIO_WARNING1 (1)
#define AUDIO_WARNING2 (2)
#define AUDIO_WARNING3 (3)
#define AUDIO_ERROR (4)
#define AUDIO_KEYPAD_UP (5)
#define AUDIO_KEYPAD_DOWN (6)
#define AUDIO_TRIM_MOVE (7)
#define AUDIO_TRIM_MIDDLE (8)
#define AUDIO_MENUS (9)
#define AUDIO_POT_STICK_MIDDLE (10)
#define AUDIO_MIX_WARNING_1 (11)
#define AUDIO_MIX_WARNING_2 (12)
#define AUDIO_MIX_WARNING_3 (13)
#define AUDIO_TIMER_30 (14)
#define AUDIO_TIMER_20 (15)
#define AUDIO_TIMER_10 (16)
#define AUDIO_TIMER_LT3 (17)
#define AUDIO_INACTIVITY (18)
#define AUDIO_TX_BATTERY_LOW (19)


#define NUM_VOL_LEVELS	24

extern volatile uint8_t Buzzer_count ;


extern void start_sound( void ) ;
extern void buzzer_on( void ) ;
extern void buzzer_off( void ) ;
extern void buzzer_sound( uint8_t time ) ;
extern void set_frequency( uint32_t frequency ) ;
extern void start_timer1( void ) ;
extern void init_dac( void ) ;
extern "C" void DAC_IRQHandler( void ) ;
extern void end_sound( void ) ;
extern void tone_start( register uint32_t time ) ;
extern void tone_stop( void ) ;
extern void init_twi( void ) ;
extern void set_volume( register uint8_t volume ) ;
extern "C" void TWI0_IRQHandler (void) ;
extern void audioDefevent( uint8_t e ) ;



