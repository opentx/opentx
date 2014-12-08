/*
 * Authors (alphabetical order)
 * - Aguerre Franck  
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

#ifndef board_mega2560_h
#define board_mega2560_h

#include "../common_avr/board_avr.h"

#define GPIO_BUTTON_MENU         pinl                          //ok
#define GPIO_BUTTON_EXIT         pinl                          //ok
#define GPIO_BUTTON_RIGHT        pinl                          //ok
#define GPIO_BUTTON_LEFT         pinl                          //ok
#define GPIO_BUTTON_UP           pinl                          //ok
#define GPIO_BUTTON_DOWN         pinl                          //ok
#define PIN_BUTTON_MENU          (1<<INP_L_KEY_MEN)            //ok
#define PIN_BUTTON_EXIT          (1<<INP_L_KEY_EXT)            //ok
#define PIN_BUTTON_UP            (1<<INP_L_KEY_UP)             //ok
#define PIN_BUTTON_DOWN          (1<<INP_L_KEY_DWN)            //ok
#define PIN_BUTTON_RIGHT         (1<<INP_L_KEY_RGT)            //ok
#define PIN_BUTTON_LEFT          (1<<INP_L_KEY_LFT)            //ok

#define GPIO_TRIM_LH_L           pinf                          //ok
#define GPIO_TRIM_LV_DN          pinf                          //ok
#define GPIO_TRIM_RV_UP          pinf                          //ok
#define GPIO_TRIM_RH_L           pinf                          //ok
#define GPIO_TRIM_LH_R           pinf                          //ok
#define GPIO_TRIM_LV_UP          pinf                          //ok
#define GPIO_TRIM_RV_DN          pinf                          //ok
#define GPIO_TRIM_RH_R           pinf                          //ok
#define PIN_TRIM_LH_L            (1<<INP_F_TRM_LH_DWN)         //ok
#define PIN_TRIM_LV_DN           (1<<INP_F_TRM_LV_DWN)         //ok
#define PIN_TRIM_RV_UP           (1<<INP_F_TRM_RV_UP)          //ok
#define PIN_TRIM_RH_L            (1<<INP_F_TRM_RH_DWN)         //ok
#define PIN_TRIM_LH_R            (1<<INP_F_TRM_LH_UP)          //ok
#define PIN_TRIM_LV_UP           (1<<INP_F_TRM_LV_UP)          //ok
#define PIN_TRIM_RV_DN           (1<<INP_F_TRM_RV_DWN)         //ok
#define PIN_TRIM_RH_R            (1<<INP_F_TRM_RH_UP)          //ok

#define TIMER_16KHZ_VECT         TIMER2_OVF_vect
#define COUNTER_16KHZ            TCNT2
#define TIMER_10MS_VECT          TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL       OCR2A
#define PAUSE_10MS_INTERRUPT()   TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT()  TIMSK2 |= (1<<OCIE2A)
#define PAUSE_PPMIN_INTERRUPT()  TIMSK3 &= ~(1<<ICIE3)
#define RESUME_PPMIN_INTERRUPT() TIMSK3 |= (1<<ICIE3)

#define SLAVE_MODE()             (PINH & (1<<INP_H_RF_POW))     //not tested
#define JACK_PPM_OUT()           PORTB |= (1<<OUT_B_SIM_CTL)    //not tested
#define JACK_PPM_IN()            PORTB &= ~(1<<OUT_B_SIM_CTL)   //not tested
#define speakerOn()              TCCR0A |=  (1 << COM0A0)       //not tested
#define speakerOff()             TCCR0A &= ~(1 << COM0A0)       //not tested

#define __BACKLIGHT_ON           PORTC |=  (1 << OUT_C_LIGHT)   //ok
#define __BACKLIGHT_OFF          PORTC &= ~(1 << OUT_C_LIGHT)   //ok
#define IS_BACKLIGHT_ON()        (PORTC & (1<<OUT_C_LIGHT))     //ok


// SD driver                      //to verify
#define sdDone() 
#if !defined(SIMU)
  bool sdMounted();
  void sdMountPoll();
  void sdPoll10ms();
  #define SD_IS_HC()             (0)
  #define SD_GET_BLOCKNR()       (0)
  #define SD_GET_SIZE_MB()       (0)
  #define SD_GET_SPEED()         (0)
#endif

// Keyboard driver
#  define INP_L_KEY_EXT          5    //ok
#  define INP_L_KEY_MEN          4    //ok
#  define INP_L_KEY_LFT          3    //ok
#  define INP_L_KEY_RGT          2    //ok
#  define INP_L_KEY_UP           1    //ok
#  define INP_L_KEY_DWN          0    //ok

// Trims driver
#  define INP_F_TRM_RH_UP        7    //ok
#  define INP_F_TRM_RH_DWN       6    //ok
#  define INP_F_TRM_RV_UP        5    //ok
#  define INP_F_TRM_RV_DWN       4    //ok
#  define INP_F_TRM_LV_UP        3    //ok
#  define INP_F_TRM_LV_DWN       2    //ok
#  define INP_F_TRM_LH_UP        1    //ok
#  define INP_F_TRM_LH_DWN       0    //ok

// Switchs driver
#  define INP_C_ID2              1    //ok
#  define INP_C_ID1              0    //ok
#  define INP_D_AileDR           7    //ok
#  define INP_G_ThrCt            2    //ok
#  define INP_G_Gear             1    //ok
#  define INP_G_RuddDR           0    //ok
#  define INP_L_ElevDR           6    //ok
#  define INP_L_Trainer          7    //ok

// Servitudes driver
#  define INP_B_PPM_IN           7    //not tested
#  define OUT_B_PPM              6    //to verify
#  define OUT_B_SIM_CTL          5    //not tested
#  define OUT_B_BUZZER           4    //to verify
#  define INP_H_RF_POW           6    //not tested
//#define INP_H_                 5    //reserved JACKPRES                                                                          
//#define INP_H_                 4    //reserved HOLDPWR
#  define OUT_H_Speaker          3    //not tested
#  define INP_E_TELEM_RX         1    //same as Gruvin9x
#  define OUT_E_TELEM_TX         0    //same as Gruvin9x
  
// Rotary encoders driver
#  define INP_E_ROT_ENC_1_A      4    //ok
#  define INP_E_ROT_ENC_1_B      5    //ok
#  define INP_D_ROT_ENC_2_A      2    //ok
#  define INP_D_ROT_ENC_2_B      3    //ok
#  define INP_J_ROT_ENC_1_PUSH   0    //ok
#  define INP_J_ROT_ENC_2_PUSH   1    //ok
#  define REA_DOWN()    (~PINJ & (1<<INP_J_ROT_ENC_1_PUSH))   //ok
#  define REB_DOWN()    (~PINJ & (1<<INP_J_ROT_ENC_2_PUSH))   //ok
#  define ROTENC_DOWN() (REA_DOWN() || REB_DOWN())            //ok

// Old #define from Gruvin9x
/*
#  define OUT_E_BUZZER           3    //to erase
#  define INP_E_USB_D_PLS        4    //to erase
#  define INP_E_USB_D_NEG        2    //to erase
#  define OUT_D_HAPTIC           7    //to erase
#  define INP_D_SPARE4           6    //to erase
#  define INP_D_I2C_SCL          1    //to erase
#  define INP_D_I2C_SDA          0    //to erase
*/

// LCD driver
#define PORTA_LCD_DAT            PORTA    //ok
#define PORTC_LCD_CTRL           PORTC    //ok
#define OUT_C_LCD_E              7        //ok
#define OUT_C_LCD_RnW            6        //ok
#define OUT_C_LCD_A0             5        //ok
#define OUT_C_LCD_RES            4        //ok
#define OUT_C_LCD_CS1            3        //ok
#define OUT_C_LIGHT              2        //ok

// DBLKeys driver                         //ok
#define KEYS_PRESSED() (~PINL)
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_LFT))) == ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_LFT)))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & ((1<<INP_L_KEY_UP)  + (1<<INP_L_KEY_DWN))) == ((1<<INP_L_KEY_UP)  + (1<<INP_L_KEY_DWN)))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_UP)))  == ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_UP)))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & ((1<<INP_L_KEY_LFT) + (1<<INP_L_KEY_DWN))) == ((1<<INP_L_KEY_LFT) + (1<<INP_L_KEY_DWN)))

// Power driver
uint8_t pwrCheck();
void pwrOff();
#define UNEXPECTED_SHUTDOWN() ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)

// USB fake driver
#define usbPlugged()    false

// Haptic driver
#define hapticOff() // TODO hapticOn() cleaner ...

// Buzzer driver
#define buzzerOn()               PORTB |=  (1 << OUT_B_BUZZER)
#define buzzerOff()              PORTB &= ~(1 << OUT_B_BUZZER)

#endif
