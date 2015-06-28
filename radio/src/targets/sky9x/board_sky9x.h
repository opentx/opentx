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

#ifndef board_sky9x_h
#define board_sky9x_h

#include <stdio.h>
#include "board.h"
#include "audio_driver.h"

extern uint16_t ResetReason;

#define BOOTLOADER_SIZE                0x8000
#define FIRMWARE_SIZE                  (256*1024)
#define FIRMWARE_ADDRESS               0x00400000

#if defined(REVA)
  #define KEYS_GPIO_REG_MENU           PIOB->PIO_PDSR
  #define KEYS_GPIO_REG_EXIT           PIOA->PIO_PDSR
  #define KEYS_GPIO_REG_UP             PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_DOWN           PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_RIGHT          PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_LEFT           PIOC->PIO_PDSR
  #define KEYS_GPIO_PIN_MENU           0x00000040
  #define KEYS_GPIO_PIN_EXIT           0x80000000
  #define KEYS_GPIO_PIN_UP             0x00000004
  #define KEYS_GPIO_PIN_DOWN           0x00000008
  #define KEYS_GPIO_PIN_RIGHT          0x00000010
  #define KEYS_GPIO_PIN_LEFT           0x00000020
#else
  #define KEYS_GPIO_REG_MENU           PIOB->PIO_PDSR
  #define KEYS_GPIO_REG_EXIT           PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_UP             PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_DOWN           PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_RIGHT          PIOC->PIO_PDSR
  #define KEYS_GPIO_REG_LEFT           PIOC->PIO_PDSR
  #define KEYS_GPIO_PIN_MENU           0x00000020
  #define KEYS_GPIO_PIN_EXIT           0x01000000
  #define KEYS_GPIO_PIN_UP             0x00000002
  #define KEYS_GPIO_PIN_DOWN           0x00000020
  #define KEYS_GPIO_PIN_RIGHT          0x00000010
  #define KEYS_GPIO_PIN_LEFT           0x00000008
#endif

#if defined(REVX)
  #define TRIMS_GPIO_REG_LHL           PIOB->PIO_PDSR
  #define TRIMS_GPIO_REG_LVD           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_RVU           PIOC->PIO_PDSR
  #define TRIMS_GPIO_REG_RHL           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_LHR           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_LVU           PIOC->PIO_PDSR
  #define TRIMS_GPIO_REG_RVD           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_RHR           PIOC->PIO_PDSR
#else
  #define TRIMS_GPIO_REG_LHL           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_LVD           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_RVU           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_RHL           PIOA->PIO_PDSR
  #define TRIMS_GPIO_REG_LHR           PIOB->PIO_PDSR
  #define TRIMS_GPIO_REG_LVU           PIOC->PIO_PDSR
  #define TRIMS_GPIO_REG_RVD           PIOC->PIO_PDSR
  #define TRIMS_GPIO_REG_RHR           PIOC->PIO_PDSR
#endif

#if defined(REVX)
  #define TRIMS_GPIO_PIN_LHL           0x00000010
  #define TRIMS_GPIO_PIN_LVD           0x01000000
  #define TRIMS_GPIO_PIN_RVU           0x00000400
  #define TRIMS_GPIO_PIN_RHL           0x00000001
  #define TRIMS_GPIO_PIN_LHR           0x00800000
  #define TRIMS_GPIO_PIN_LVU           0x10000000
  #define TRIMS_GPIO_PIN_RVD           0x00000002
  #define TRIMS_GPIO_PIN_RHR           0x00000200
#elif defined(REVA)
  #define TRIMS_GPIO_PIN_LHL           0x00000080
  #define TRIMS_GPIO_PIN_LVD           0x08000000
  #define TRIMS_GPIO_PIN_RVU           0x40000000
  #define TRIMS_GPIO_PIN_RHL           0x20000000
  #define TRIMS_GPIO_PIN_LHR           0x00000010
  #define TRIMS_GPIO_PIN_LVU           0x10000000
  #define TRIMS_GPIO_PIN_RVD           0x00000400
  #define TRIMS_GPIO_PIN_RHR           0x00000200
#else
  #define TRIMS_GPIO_PIN_LHL           0x00800000
  #define TRIMS_GPIO_PIN_LVD           0x01000000
  #define TRIMS_GPIO_PIN_RVU           0x00000002
  #define TRIMS_GPIO_PIN_RHL           0x00000001
  #define TRIMS_GPIO_PIN_LHR           0x00000010
  #define TRIMS_GPIO_PIN_LVU           0x10000000
  #define TRIMS_GPIO_PIN_RVD           0x00000400
  #define TRIMS_GPIO_PIN_RHR           0x00000200
#endif

void usbMassStorage();

#define PIN_ENABLE                     0x001
#define PIN_PERIPHERAL                 0x000
#define PIN_INPUT                      0x002
#define PIN_OUTPUT                     0x000
#define PIN_PULLUP                     0x004
#define PIN_NO_PULLUP                  0x000
#define PIN_PULLDOWN                   0x008
#define PIN_NO_PULLDOWN                0x000
#define PIN_PERI_MASK_L                0x010
#define PIN_PERI_MASK_H                0x020
#define PIN_PER_A                      0x000
#define PIN_PER_B                      0x010
#define PIN_PER_C                      0x020
#define PIN_PER_D                      0x030
#define PIN_PORT_MASK                  0x0C0
#define PIN_PORTA                      0x000
#define PIN_PORTB                      0x040
#define PIN_PORTC                      0x080
#define PIN_LOW                        0x000
#define PIN_HIGH                       0x100

// Telemetry port
#define SECOND_USART                   USART0
#define SECOND_ID                      ID_USART0
#define SECOND_PINS                    { PINS_USART0 }

void configure_pins( uint32_t pins, uint16_t config );
uint16_t getCurrent();

extern uint8_t temperature ;              // Raw temp reading
extern uint8_t maxTemperature ;           // Raw temp reading
uint8_t getTemperature();

#define strcpy_P strcpy
#define strcat_P strcat

#if !defined(REVA)
extern uint16_t Current_analogue;
extern uint16_t Current_max;
extern uint32_t Current_accumulator;
extern uint32_t Current_used;
extern uint16_t sessionTimer;
void calcConsumption();
#endif

#define SLAVE_MODE()                   (pwrCheck() == e_power_trainer)
// #define JACK_PPM_OUT() PIOC->PIO_PDR = PIO_PC22
// #define JACK_PPM_IN() PIOC->PIO_PER = PIO_PC22
void checkTrainerSettings();

void setSticksGain(uint8_t gains);

// Write Flash driver
#define FLASH_PAGESIZE                 256
void writeFlash(uint32_t * address, uint32_t * buffer);

// Keys driver
extern uint32_t readKeys();
extern uint32_t readTrims();
#define TRIMS_PRESSED()                readTrims()
#define KEYS_PRESSED()                 readKeys()

// Pulses driver
void init_no_pulses(uint32_t port);
void disable_no_pulses(uint32_t port);
void init_ppm(uint32_t port);
void disable_ppm(uint32_t port);
void init_pxx(uint32_t port);
void disable_pxx(uint32_t port);
void init_dsm2(uint32_t port);
void disable_dsm2(uint32_t port);

// SD driver
#if defined(SIMU)
  #define sdInit()
  #define sdDone()
#else
  #define sdPoll10ms()
  void sdMountPoll();
  extern "C" {
    void init_SDcard();
    void sdInit();
    void sdDone();
    uint32_t sd_card_ready();
    uint32_t sdMounted();
  }
#endif

// WDT driver
#if !defined(SIMU)
  #define wdt_disable()
  #define wdt_enable(x)                WDT->WDT_MR = 0x3FFF207F
  #define wdt_reset()                  WDT->WDT_CR = 0xA5000001
#endif

// Backlight driver
#define setBacklight(xx)               (PWM->PWM_CH_NUM[0].PWM_CDTYUPD = xx)
#define backlightEnable()              (PWM->PWM_CH_NUM[0].PWM_CDTY = g_eeGeneral.backlightBright)
#define backlightDisable()             (PWM->PWM_CH_NUM[0].PWM_CDTY = 100)
#define isBacklightEnable()            (PWM->PWM_CH_NUM[0].PWM_CDTY != 100)

// ADC driver
void adcInit();
void adcRead(void);
inline uint16_t getAnalogValue(uint32_t value);

// Buzzer driver
void buzzerSound(uint8_t duration);
void buzzerHeartbeat();
#define BUZZER_HEARTBEAT               buzzerHeartbeat

// i2c driver
void i2cCheck();

// Coproc driver
void coprocInit();
void coprocWriteData(uint8_t *data, uint32_t size);
void coprocReadData(bool onlytemp=false);
extern int8_t volumeRequired;
extern uint8_t Coproc_read;
extern int8_t Coproc_valid;
extern int8_t Coproc_temp;
extern int8_t Coproc_maxtemp;

// Haptic driver
void hapticOff(void);
void hapticOn(uint32_t pwmPercent);

// BlueTooth driver
#if defined(BLUETOOTH)
void btInit();
void btTask(void* pdata);
void btPushByte(uint8_t data);
#endif

// Power driver
void pwrInit();
void pwrOff();
uint32_t pwrCheck();
#define UNEXPECTED_SHUTDOWN()          (g_eeGeneral.unexpectedShutdown)

// EEPROM driver
void eepromInit();
uint32_t eepromReadStatus();

// Rotary Encoder driver
void rotencInit();
void rotencEnd();

#if ROTARY_ENCODERS > 0
  #define REA_DOWN()                   (!(PIOB->PIO_PDSR & 0x40))
#else
  #define REA_DOWN()                   (0)
#endif

// Debug driver
void debugPutc(const char c);

// Telemetry driver
void telemetryPortInit(uint32_t baudrate);
uint32_t telemetryTransmitPending();
void telemetryTransmitBuffer(uint8_t * buffer, uint32_t size);

// Second UART driver
void telemetrySecondPortInit(unsigned int protocol);
bool telemetrySecondPortReceive(uint8_t & data);

#endif
