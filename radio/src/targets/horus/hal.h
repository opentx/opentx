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

#ifndef _HAL_H_
#define _HAL_H_

// Keys
#define KEYS_RCC_AHB1Periph             (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOJ)
#if defined(PCBX12S)
  #define KEYS_GPIO_REG_PGUP            GPIOC->IDR
  #define KEYS_GPIO_PIN_PGUP            GPIO_Pin_13 // PC.13
  #define KEYS_GPIO_REG_PGDN            GPIOI->IDR
  #define KEYS_GPIO_PIN_PGDN            GPIO_Pin_8  // PI.08
  #define KEYS_GPIO_REG_LEFT            GPIOI->IDR
  #define KEYS_GPIO_PIN_LEFT            GPIO_Pin_7  // PI.07
  #define KEYS_GPIO_REG_ENTER           GPIOC->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_1  // PC.01
  #define KEYS_GPIO_REG_UP              GPIOG->IDR
  #define KEYS_GPIO_PIN_UP              GPIO_Pin_13 // PG.13
  #define KEYS_GPIO_REG_DOWN            GPIOI->IDR
  #define KEYS_GPIO_PIN_DOWN            GPIO_Pin_6  // PI.06
  #define KEYS_GPIO_REG_RIGHT           GPIOC->IDR
  #define KEYS_GPIO_PIN_RIGHT           GPIO_Pin_4  // PC.04
#elif defined(PCBX10)
  // #define KEYS_GPIO_REG_UNUSED          GPIOH->IDR
  // #define KEYS_GPIO_PIN_UNUSED          GPIO_Pin_14 // PH.14
  // #define KEYS_GPIO_REG_UNUSED          GPIOH->IDR
  // #define KEYS_GPIO_PIN_UNUSED          GPIO_Pin_15 // PH.15
  #define KEYS_GPIO_REG_ENTER           GPIOI->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_8  // PI.08
  #define KEYS_GPIO_REG_PGDN            GPIOI->IDR
  #define KEYS_GPIO_PIN_PGDN            GPIO_Pin_11 // PI.11
  #define KEYS_GPIO_REG_UP              GPIOI->IDR
  #define KEYS_GPIO_PIN_UP              GPIO_Pin_4  // PI.04
  #define KEYS_GPIO_REG_DOWN            GPIOI->IDR
  #define KEYS_GPIO_PIN_DOWN            GPIO_Pin_6  // PI.06
  #define KEYS_GPIO_REG_LEFT            GPIOI->IDR
  #define KEYS_GPIO_PIN_LEFT            GPIO_Pin_7  // PI.07
  #define KEYS_GPIO_REG_RIGHT           GPIOI->IDR
  #define KEYS_GPIO_PIN_RIGHT           GPIO_Pin_5  // PI.05
#endif

// Rotary Encoder
#define ROTARY_ENCODER_GPIO             GPIOH
#define ROTARY_ENCODER_GPIO_PIN_A       GPIO_Pin_11 // PH.11
#define ROTARY_ENCODER_GPIO_PIN_B       GPIO_Pin_10 // PH.10
#define ROTARY_ENCODER_POSITION()       ((ROTARY_ENCODER_GPIO->IDR >> 10) & 0x03)

// Switches
#define SWITCHES_GPIO_REG_A_H           GPIOH->IDR
#define SWITCHES_GPIO_PIN_A_H           GPIO_Pin_9  // PH.09
#define SWITCHES_GPIO_REG_A_L           GPIOI->IDR
#define SWITCHES_GPIO_PIN_A_L           GPIO_Pin_15 // PI.15
#define SWITCHES_GPIO_REG_B_H           GPIOH->IDR
#define SWITCHES_GPIO_PIN_B_H           GPIO_Pin_12 // PH.12
#define SWITCHES_GPIO_REG_B_L           GPIOB->IDR
#define SWITCHES_GPIO_PIN_B_L           GPIO_Pin_12 // PB.12
#define SWITCHES_GPIO_REG_C_H           GPIOD->IDR
#define SWITCHES_GPIO_PIN_C_H           GPIO_Pin_11 // PD.11
#define SWITCHES_GPIO_REG_C_L           GPIOB->IDR
#define SWITCHES_GPIO_PIN_C_L           GPIO_Pin_15 // PB.15
#define SWITCHES_GPIO_REG_D_H           GPIOJ->IDR
#define SWITCHES_GPIO_PIN_D_H           GPIO_Pin_7  // PJ.07
#define SWITCHES_GPIO_REG_D_L           GPIOG->IDR
#define SWITCHES_GPIO_PIN_D_L           GPIO_Pin_2  // PG.02
#define SWITCHES_GPIO_REG_E_H           GPIOH->IDR
#define SWITCHES_GPIO_PIN_E_H           GPIO_Pin_4  // PH.04
#define SWITCHES_GPIO_REG_E_L           GPIOE->IDR
#define SWITCHES_GPIO_PIN_E_L           GPIO_Pin_3  // PE.03
#define SWITCHES_GPIO_REG_F             GPIOH->IDR
#define SWITCHES_GPIO_PIN_F             GPIO_Pin_3  // PH.03
#define SWITCHES_GPIO_REG_G_H           GPIOG->IDR
#define SWITCHES_GPIO_PIN_G_H           GPIO_Pin_6  // PG.06
#define SWITCHES_GPIO_REG_G_L           GPIOG->IDR
#define SWITCHES_GPIO_PIN_G_L           GPIO_Pin_3  // PG.03
#define SWITCHES_GPIO_REG_H             GPIOG->IDR
#define SWITCHES_GPIO_PIN_H             GPIO_Pin_7  // PG.07

#if defined(PCBX10)
  #define SWITCHES_GPIO_REG_GMBL        GPIOH->IDR
  #define SWITCHES_GPIO_PIN_GMBL        GPIO_Pin_14
  #define SWITCHES_GPIO_REG_GMBR        GPIOH->IDR
  #define SWITCHES_GPIO_PIN_GMBR        GPIO_Pin_15
#endif

// Trims
#if defined(PCBX12S)
  #define TRIMS_GPIO_REG_RHL            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RHL            GPIO_Pin_0  // PC.00
  #define TRIMS_GPIO_REG_RHR            GPIOI->IDR
  #define TRIMS_GPIO_PIN_RHR            GPIO_Pin_4  // PI.04
  #define TRIMS_GPIO_REG_RVD            GPIOG->IDR
  #define TRIMS_GPIO_PIN_RVD            GPIO_Pin_12 // PG.12
  #define TRIMS_GPIO_REG_RVU            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_RVU            GPIO_Pin_14 // PJ.14
  #define TRIMS_GPIO_REG_LVD            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_13 // PJ.13
  #define TRIMS_GPIO_REG_LHL            GPIOD->IDR
  #define TRIMS_GPIO_PIN_LHL            GPIO_Pin_3  // PD.03
  #define TRIMS_GPIO_REG_LVU            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_12 // PJ.12
  #define TRIMS_GPIO_REG_LHR            GPIOD->IDR
  #define TRIMS_GPIO_PIN_LHR            GPIO_Pin_7  // PD.07
  #define TRIMS_GPIO_REG_RSD            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_RSD            GPIO_Pin_8  // PJ.08
  #define TRIMS_GPIO_REG_RSU            GPIOD->IDR
  #define TRIMS_GPIO_PIN_RSU            GPIO_Pin_13 // PD.13
  #define TRIMS_GPIO_REG_LSD            GPIOB->IDR
  #define TRIMS_GPIO_PIN_LSD            GPIO_Pin_14 // PB.14
  #define TRIMS_GPIO_REG_LSU            GPIOB->IDR
  #define TRIMS_GPIO_PIN_LSU            GPIO_Pin_13 // PB.13
#elif defined(PCBX10)
  #define TRIMS_GPIO_REG_LHL            GPIOB->IDR
  #define TRIMS_GPIO_PIN_LHL            GPIO_Pin_8  // PB.08
  #define TRIMS_GPIO_REG_LHR            GPIOB->IDR
  #define TRIMS_GPIO_PIN_LHR            GPIO_Pin_9  // PB.09
  #define TRIMS_GPIO_REG_LVD            GPIOG->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_12 // PG.12
  #define TRIMS_GPIO_REG_LVU            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_14 // PJ.14
  #define TRIMS_GPIO_REG_RVD            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_RVD            GPIO_Pin_13 // PJ.13
  #define TRIMS_GPIO_REG_RHL            GPIOD->IDR
  #define TRIMS_GPIO_PIN_RHL            GPIO_Pin_3  // PD.03
  #define TRIMS_GPIO_REG_RVU            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_RVU            GPIO_Pin_12 // PJ.12
  #define TRIMS_GPIO_REG_RHR            GPIOD->IDR
  #define TRIMS_GPIO_PIN_RHR            GPIO_Pin_7  // PD.07
  #define TRIMS_GPIO_REG_LSU            GPIOJ->IDR
  #define TRIMS_GPIO_PIN_LSU            GPIO_Pin_8  // PJ.08
  #define TRIMS_GPIO_REG_LSD            GPIOD->IDR
  #define TRIMS_GPIO_PIN_LSD            GPIO_Pin_13 // PD.13
  #define TRIMS_GPIO_REG_RSU            GPIOB->IDR
  #define TRIMS_GPIO_PIN_RSU            GPIO_Pin_14 // PB.14
  #define TRIMS_GPIO_REG_RSD            GPIOB->IDR
  #define TRIMS_GPIO_PIN_RSD            GPIO_Pin_13 // PB.13
#endif

// Index of all keys
#if defined(PCBX12S)
  #define KEYS_GPIOB_PINS               (SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_L | TRIMS_GPIO_PIN_LSD | TRIMS_GPIO_PIN_LSU)
  #define KEYS_GPIOC_PINS               (KEYS_GPIO_PIN_PGUP | KEYS_GPIO_PIN_ENTER | KEYS_GPIO_PIN_RIGHT | TRIMS_GPIO_PIN_RHL)
  #define KEYS_GPIOD_PINS               (SWITCHES_GPIO_PIN_C_H | TRIMS_GPIO_PIN_LHL | TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_RSU)
  #define KEYS_GPIOE_PINS               (SWITCHES_GPIO_PIN_E_L)
  #define KEYS_GPIOG_PINS               (KEYS_GPIO_PIN_UP | SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_H | TRIMS_GPIO_PIN_RVD)
  #define KEYS_GPIOH_PINS               (SWITCHES_GPIO_PIN_A_H | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_F | ROTARY_ENCODER_GPIO_PIN_A | ROTARY_ENCODER_GPIO_PIN_B)
  #define KEYS_GPIOI_PINS               (KEYS_GPIO_PIN_PGDN | KEYS_GPIO_PIN_LEFT | KEYS_GPIO_PIN_DOWN | SWITCHES_GPIO_PIN_A_L | TRIMS_GPIO_PIN_RHR)
  #define KEYS_GPIOJ_PINS               (SWITCHES_GPIO_PIN_D_H | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU | TRIMS_GPIO_PIN_RSD)
#elif defined(PCBX10)
  #define KEYS_GPIOB_PINS               (SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_L | TRIMS_GPIO_PIN_RSU | TRIMS_GPIO_PIN_RSD | TRIMS_GPIO_PIN_LHL | TRIMS_GPIO_PIN_LHR)
  #define KEYS_GPIOC_PINS               (TRIMS_GPIO_PIN_LHL)
  #define KEYS_GPIOD_PINS               (SWITCHES_GPIO_PIN_C_H | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR | TRIMS_GPIO_PIN_LSD)
  #define KEYS_GPIOE_PINS               (SWITCHES_GPIO_PIN_E_L)
  #define KEYS_GPIOG_PINS               (SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_H | TRIMS_GPIO_PIN_LVD)
  #define KEYS_GPIOH_PINS               (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12) // | GPIO_Pin_14 | GPIO_Pin_15)
  #define KEYS_GPIOI_PINS               (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_15)
  #define KEYS_GPIOJ_PINS               (SWITCHES_GPIO_PIN_D_H | TRIMS_GPIO_PIN_LVU | TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_LSU)
#endif

// ADC
#if defined(PCBX12S)
  #define ADC_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_DMA2)
  #define ADC_RCC_APB1Periph            0
  #define ADC_RCC_APB2Periph            (RCC_APB2Periph_SPI4 | RCC_APB2Periph_ADC3)
  #define ADC_SPI                       SPI4
  #define ADC_GPIO_AF                   GPIO_AF_SPI4
  #define ADC_SPI_GPIO                  GPIOE
  #define ADC_SPI_PIN_SCK               GPIO_Pin_2 // PE.02
  #define ADC_SPI_PIN_CS                GPIO_Pin_4 // PE.04
  #define ADC_SPI_PIN_MOSI              GPIO_Pin_6 // PE.06
  #define ADC_SPI_PIN_MISO              GPIO_Pin_5 // PE.05
  #define ADC_SPI_PinSource_SCK         GPIO_PinSource2
  #define ADC_SPI_PinSource_MISO        GPIO_PinSource5
  #define ADC_SPI_PinSource_MOSI        GPIO_PinSource6
  #define ADC_GPIO_PIN_MOUSE1           GPIO_Pin_8 // PF.08 ADC3_IN6 J5 MOUSE_X
  #define ADC_GPIO_PIN_MOUSE2           GPIO_Pin_9 // PF.09 ADC3_IN7 J6 MOUSE_Y
  #define ADC_GPIO_MOUSE                GPIOF
  #define ADC_IN_MOUSE1                 6
  #define ADC_IN_MOUSE2                 7
  #define ADC_DMA                       DMA2
  #define ADC_DMA_Stream                DMA2_Stream0
  #define ADC_SAMPTIME                  3
#elif defined(PCBX10)
  #define ADC_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_DMA2)
  #define ADC_RCC_APB1Periph            (RCC_APB1Periph_TIM5)
  #define ADC_RCC_APB2Periph            (RCC_APB2Periph_ADC3)
  #define ADC_GPIO_PIN_STICK_LH         GPIO_Pin_0      // PA.00
  #define ADC_GPIO_PIN_STICK_LV         GPIO_Pin_1      // PA.01
  #define ADC_GPIO_PIN_STICK_RH         GPIO_Pin_2      // PA.02
  #define ADC_GPIO_PIN_STICK_RV         GPIO_Pin_3      // PA.03
  #define ADC_GPIO_PIN_POT1             GPIO_Pin_0      // PC.00
  #define ADC_GPIO_PIN_POT2             GPIO_Pin_1      // PC.01
  #define ADC_GPIO_PIN_POT3             GPIO_Pin_2      // PC.02
  #define ADC_GPIO_PIN_SLIDER1          GPIO_Pin_6      // PF.06
  #define ADC_GPIO_PIN_SLIDER2          GPIO_Pin_3      // PC.03
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_7      // PF.07
  #define ADC_GPIO_PIN_EXT1             GPIO_Pin_8      // PF.08
  #define ADC_GPIO_PIN_EXT2             GPIO_Pin_9      // PF.09
  #define PWM_TIMER                     TIM5
  #define PWM_GPIO                      GPIOA
  #define PWM_GPIO_AF                   GPIO_AF_TIM5
  #define PWM_IRQHandler                TIM5_IRQHandler
  #define PWM_IRQn                      TIM5_IRQn
  #define PWM_GPIOA_PINS                (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)
  #define ADC_GPIOA_PINS                (STICKS_PWM_ENABLED() ? 0 : (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3))
  #define ADC_GPIOC_PINS                (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)
  #define ADC_GPIOF_PINS                (GPIO_Pin_6 | GPIO_Pin_7) // | GPIO_Pin_8 | GPIO_Pin_9)
  #define ADC_CHANNEL_STICK_LH          ADC_Channel_0   // ADC3_IN0
  #define ADC_CHANNEL_STICK_LV          ADC_Channel_1   // ADC3_IN1
  #define ADC_CHANNEL_STICK_RH          ADC_Channel_2   // ADC3_IN2
  #define ADC_CHANNEL_STICK_RV          ADC_Channel_3   // ADC3_IN3
  #define ADC_CHANNEL_POT1              ADC_Channel_10  // ADC3_IN10
  #define ADC_CHANNEL_POT2              ADC_Channel_11  // ADC3_IN11
  #define ADC_CHANNEL_POT3              ADC_Channel_12  // ADC3_IN12
  #define ADC_CHANNEL_SLIDER1           ADC_Channel_4   // ADC3_IN4
  #define ADC_CHANNEL_SLIDER2           ADC_Channel_13  // ADC3_IN13
  #define ADC_CHANNEL_BATT              ADC_Channel_5   // ADC3_IN5
  #define ADC_CHANNEL_EXT1              ADC_Channel_6   // ADC3_IN6
  #define ADC_CHANNEL_EXT2              ADC_Channel_7   // ADC3_IN7
  #define ADC_MAIN                      ADC3
  #define ADC_SAMPTIME                  3
  #define ADC_DMA                       DMA2
  #define ADC_DMA_SxCR_CHSEL            DMA_SxCR_CHSEL_1
  #define ADC_DMA_Stream                DMA2_Stream0
  #define ADC_SET_DMA_FLAGS()           ADC_DMA->LIFCR = (DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0)
  #define ADC_TRANSFER_COMPLETE()       (ADC_DMA->LISR & DMA_LISR_TCIF0)
#endif

// Power
#define PWR_RCC_AHB1Periph              RCC_AHB1Periph_GPIOJ
#define PWR_GPIO                        GPIOJ
#define PWR_SWITCH_GPIO_REG             PWR_GPIO->IDR
#define PWR_SWITCH_GPIO_PIN             GPIO_Pin_0  // PJ.00
#define PWR_ON_GPIO_PIN                 GPIO_Pin_1  // PJ.01
#define PWR_ON_GPIO_MODER               GPIO_MODER_MODER1
#define PWR_ON_GPIO_MODER_OUT           GPIO_MODER_MODER1_0

// S.Port update connector
#define SPORT_MAX_BAUDRATE              250000 // < 400000
#if defined(PCBX10)
  #define SPORT_UPDATE_RCC_AHB1Periph   RCC_AHB1Periph_GPIOH
  #define SPORT_UPDATE_PWR_GPIO         GPIOH
  #define SPORT_UPDATE_PWR_GPIO_PIN     GPIO_Pin_13  // PH.13
  #define HAS_SPORT_UPDATE_CONNECTOR()  true
#else
  #define SPORT_UPDATE_RCC_AHB1Periph   0
  #define HAS_SPORT_UPDATE_CONNECTOR()  false
#endif

// PCBREV
#define PCBREV_RCC_AHB1Periph           RCC_AHB1Periph_GPIOI
#define PCBREV_GPIO                     GPIOI
#define PCBREV_GPIO_PIN                 GPIO_Pin_11  // PI.11

// Led
#define STATUS_LEDS
#if defined(PCBX12S)
  #define LED_RCC_AHB1Periph            RCC_AHB1Periph_GPIOI
  #define LED_GPIO                      GPIOI
  #define LED_GPIO_PIN                  GPIO_Pin_5  // PI.05
#elif defined(PCBX10)
  #define LED_RCC_AHB1Periph            RCC_AHB1Periph_GPIOE
  #define LED_GPIO                      GPIOE
  #define LED_RED_GPIO_PIN              GPIO_Pin_2
  #define LED_GREEN_GPIO_PIN            GPIO_Pin_4
  #define LED_BLUE_GPIO_PIN             GPIO_Pin_5
  #define LED_GPIO_PIN                  (LED_RED_GPIO_PIN | LED_GREEN_GPIO_PIN | LED_BLUE_GPIO_PIN)
#endif

// Serial Port (DEBUG)
#define SERIAL_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1)
#define SERIAL_RCC_APB1Periph           RCC_APB1Periph_USART3
#define SERIAL_GPIO                     GPIOB
#define SERIAL_GPIO_PIN_TX              GPIO_Pin_10 // PB.10
#define SERIAL_GPIO_PIN_RX              GPIO_Pin_11 // PB.11
#define SERIAL_GPIO_PinSource_TX        GPIO_PinSource10
#define SERIAL_GPIO_PinSource_RX        GPIO_PinSource11
#define SERIAL_GPIO_AF                  GPIO_AF_USART3
#define SERIAL_USART                    USART3
#define SERIAL_USART_IRQHandler         USART3_IRQHandler
#define SERIAL_USART_IRQn               USART3_IRQn
#define SERIAL_DMA_Stream_RX            DMA1_Stream1
#define SERIAL_DMA_Channel_RX           DMA_Channel_4

// Telemetry
#define TELEMETRY_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1)
#define TELEMETRY_RCC_APB1Periph        RCC_APB1Periph_USART2
#define TELEMETRY_DIR_GPIO              GPIOD
#define TELEMETRY_DIR_GPIO_PIN          GPIO_Pin_4  // PD.04
#define TELEMETRY_GPIO                  GPIOD
#define TELEMETRY_TX_GPIO_PIN           GPIO_Pin_5  // PD.05
#define TELEMETRY_RX_GPIO_PIN           GPIO_Pin_6  // PD.06
#define TELEMETRY_GPIO_PinSource_TX     GPIO_PinSource5
#define TELEMETRY_GPIO_PinSource_RX     GPIO_PinSource6
#define TELEMETRY_GPIO_AF               GPIO_AF_USART2
#define TELEMETRY_USART                 USART2
#if defined(PCBX12S)
#define TELEMETRY_DMA_Stream_RX         DMA1_Stream5
#define TELEMETRY_DMA_Channel_RX        DMA_Channel_4
#endif
#define TELEMETRY_DMA_Stream_TX         DMA1_Stream6
#define TELEMETRY_DMA_Channel_TX        DMA_Channel_4
#define TELEMETRY_DMA_TX_Stream_IRQ     DMA1_Stream6_IRQn
#define TELEMETRY_DMA_TX_IRQHandler     DMA1_Stream6_IRQHandler
#define TELEMETRY_DMA_TX_FLAG_TC        DMA_IT_TCIF6
#define TELEMETRY_USART_IRQHandler      USART2_IRQHandler
#define TELEMETRY_USART_IRQn            USART2_IRQn

// USB
#define USB_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOA
#define USB_GPIO                        GPIOA
#define USB_GPIO_PIN_VBUS               GPIO_Pin_9  // PA.09
#define USB_GPIO_PIN_DM                 GPIO_Pin_11 // PA.11
#define USB_GPIO_PIN_DP                 GPIO_Pin_12 // PA.12
#define USB_GPIO_PinSource_DM           GPIO_PinSource11
#define USB_GPIO_PinSource_DP           GPIO_PinSource12
#define USB_GPIO_AF                     GPIO_AF_OTG1_FS

// LCD
#define LCD_RCC_AHB1Periph              (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOJ | RCC_AHB1Periph_GPIOK | RCC_AHB1Periph_DMA2D)
#define LCD_RCC_APB1Periph              0
#define LCD_RCC_APB2Periph              RCC_APB2Periph_LTDC
#if defined(PCBX12S)
  #define LCD_GPIO_NRST                 GPIOF
  #define LCD_GPIO_PIN_NRST             GPIO_Pin_10 // PF.10
#elif defined(PCBX10)
  #define LCD_GPIO_NRST                 GPIOI
  #define LCD_GPIO_PIN_NRST             GPIO_Pin_10 // PI.10
#endif
#define LTDC_IRQ_PRIO                   4
#define DMA_SCREEN_IRQ_PRIO             6

// Backlight
#if defined(PCBX12S)
  #define BACKLIGHT_RCC_AHB1Periph             RCC_AHB1Periph_GPIOA
  #define BACKLIGHT_GPIO                       GPIOA
  #if PCBREV >= 13
    #define BACKLIGHT_TIMER                    TIM5
    #define BACKLIGHT_GPIO_PIN                 GPIO_Pin_3  // PA.03
    #define BACKLIGHT_GPIO_PinSource           GPIO_PinSource3
    #define BACKLIGHT_RCC_APB1Periph           RCC_APB1Periph_TIM5
    #define BACKLIGHT_RCC_APB2Periph           0
    #define BACKLIGHT_GPIO_AF                  GPIO_AF_TIM5
    #define BACKLIGHT_TIMER_FREQ               (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #else
    #define BACKLIGHT_TIMER                    TIM8
    #define BACKLIGHT_GPIO_PIN                 GPIO_Pin_5  // PA.05
    #define BACKLIGHT_GPIO_PinSource           GPIO_PinSource5
    #define BACKLIGHT_RCC_APB1Periph           0
    #define BACKLIGHT_RCC_APB2Periph           RCC_APB2Periph_TIM8
    #define BACKLIGHT_GPIO_AF                  GPIO_AF_TIM8
    #define BACKLIGHT_TIMER_FREQ               (PERI2_FREQUENCY * TIMER_MULT_APB2)
  #endif
#elif defined(PCBX10)
  #define BACKLIGHT_RCC_AHB1Periph             RCC_AHB1Periph_GPIOB
  #define BACKLIGHT_GPIO                       GPIOB
  #define BACKLIGHT_TIMER                      TIM8
  #define BACKLIGHT_GPIO_PIN                   GPIO_Pin_1  // PB.01
  #define BACKLIGHT_GPIO_PinSource             GPIO_PinSource1
  #define BACKLIGHT_RCC_APB1Periph             0
  #define BACKLIGHT_RCC_APB2Periph             RCC_APB2Periph_TIM8
  #define BACKLIGHT_GPIO_AF                    GPIO_AF_TIM8
  #define BACKLIGHT_TIMER_FREQ                 (PERI2_FREQUENCY * TIMER_MULT_APB2)
#endif

// SD
#define SD_RCC_AHB1Periph               (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2)
#define SD_RCC_APB1Periph               0
#define SD_PRESENT_GPIO                 GPIOC
#define SD_PRESENT_GPIO_PIN             GPIO_Pin_5  // PC.05
#define SD_SDIO_DMA_STREAM              DMA2_Stream3
#define SD_SDIO_DMA_CHANNEL             DMA_Channel_4
#define SD_SDIO_DMA_FLAG_FEIF           DMA_FLAG_FEIF3
#define SD_SDIO_DMA_FLAG_DMEIF          DMA_FLAG_DMEIF3
#define SD_SDIO_DMA_FLAG_TEIF           DMA_FLAG_TEIF3
#define SD_SDIO_DMA_FLAG_HTIF           DMA_FLAG_HTIF3
#define SD_SDIO_DMA_FLAG_TCIF           DMA_FLAG_TCIF3
#define SD_SDIO_DMA_IRQn                DMA2_Stream3_IRQn
#define SD_SDIO_DMA_IRQHANDLER          DMA2_Stream3_IRQHandler
#define SD_SDIO_FIFO_ADDRESS            ((uint32_t)0x40012C80)
#define SD_SDIO_CLK_DIV(fq)             ((48000000 / (fq)) - 2)
#define SD_SDIO_INIT_CLK_DIV            SD_SDIO_CLK_DIV(400000)
#define SD_SDIO_TRANSFER_CLK_DIV        SD_SDIO_CLK_DIV(24000000)

// EEPROM
#if defined(PCBX12S) && PCBREV >= 13
  #define EEPROM_RCC_AHB1Periph           RCC_AHB1Periph_GPIOA
  #define EEPROM_RCC_APB1Periph           RCC_APB1Periph_SPI1
  #define EEPROM_SPI_CS_GPIO              GPIOA
  #define EEPROM_SPI_CS_GPIO_PIN          GPIO_Pin_15 // PA.15
  #define EEPROM_SPI_SCK_GPIO             GPIOA
  #define EEPROM_SPI_SCK_GPIO_PIN         GPIO_Pin_5  // PA.05
  #define EEPROM_SPI_SCK_GPIO_PinSource   GPIO_PinSource5
  #define EEPROM_SPI_MISO_GPIO            GPIOA
  #define EEPROM_SPI_MISO_GPIO_PIN        GPIO_Pin_6  // PA.06
  #define EEPROM_SPI_MISO_GPIO_PinSource  GPIO_PinSource6
  #define EEPROM_SPI_MOSI_GPIO            GPIOA
  #define EEPROM_SPI_MOSI_GPIO_PIN        GPIO_Pin_7  // PA.07
  #define EEPROM_SPI_MOSI_GPIO_PinSource  GPIO_PinSource7
#elif defined(PCBX10)
  #define EEPROM_RCC_AHB1Periph           RCC_AHB1Periph_GPIOI
  #define EEPROM_RCC_APB1Periph           RCC_APB1Periph_SPI2
  #define EEPROM_SPI_CS_GPIO              GPIOI
  #define EEPROM_SPI_CS_GPIO_PIN          GPIO_Pin_0  // PI.00
  #define EEPROM_SPI_SCK_GPIO             GPIOI
  #define EEPROM_SPI_SCK_GPIO_PIN         GPIO_Pin_1  // PI.01
  #define EEPROM_SPI_SCK_GPIO_PinSource   GPIO_PinSource1
  #define EEPROM_SPI_MISO_GPIO            GPIOI
  #define EEPROM_SPI_MISO_GPIO_PIN        GPIO_Pin_2  // PI.02
  #define EEPROM_SPI_MISO_GPIO_PinSource  GPIO_PinSource2
  #define EEPROM_SPI_MOSI_GPIO            GPIOI
  #define EEPROM_SPI_MOSI_GPIO_PIN        GPIO_Pin_3  // PI.03
  #define EEPROM_SPI_MOSI_GPIO_PinSource  GPIO_PinSource3
#endif

// Audio
#if defined(PCBX12S)
  #define AUDIO_RCC_AHB1Periph          (RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOH)
  #define AUDIO_RCC_APB1Periph          RCC_APB1Periph_SPI2
  #define AUDIO_SHUTDOWN_GPIO           GPIOI
  #define AUDIO_SHUTDOWN_GPIO_PIN       GPIO_Pin_9  // PI.09
  #define AUDIO_XDCS_GPIO               GPIOI
  #define AUDIO_XDCS_GPIO_PIN           GPIO_Pin_0  // PI.00
  #define AUDIO_CS_GPIO                 GPIOH
  #define AUDIO_CS_GPIO_PIN             GPIO_Pin_13 // PH.13
  #define AUDIO_DREQ_GPIO               GPIOH
  #define AUDIO_DREQ_GPIO_PIN           GPIO_Pin_14 // PH.14
  #define AUDIO_RST_GPIO                GPIOH
  #define AUDIO_RST_GPIO_PIN            GPIO_Pin_15 // PH.15
  #define AUDIO_SPI                     SPI2
  #define AUDIO_SPI_GPIO_AF             GPIO_AF_SPI2
  #define AUDIO_SPI_SCK_GPIO            GPIOI
  #define AUDIO_SPI_SCK_GPIO_PIN        GPIO_Pin_1  // PI.01
  #define AUDIO_SPI_SCK_GPIO_PinSource  GPIO_PinSource1
  #define AUDIO_SPI_MISO_GPIO           GPIOI
  #define AUDIO_SPI_MISO_GPIO_PIN       GPIO_Pin_2  // PI.02
  #define AUDIO_SPI_MISO_GPIO_PinSource GPIO_PinSource2
  #define AUDIO_SPI_MOSI_GPIO           GPIOI
  #define AUDIO_SPI_MOSI_GPIO_PIN       GPIO_Pin_3  // PI.03
  #define AUDIO_SPI_MOSI_GPIO_PinSource GPIO_PinSource3
#elif defined (PCBX10)
  #define AUDIO_RCC_APB1Periph          (RCC_APB1Periph_TIM6 | RCC_APB1Periph_DAC)
  #define AUDIO_RCC_AHB1Periph          (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA1)
  #define AUDIO_MUTE_GPIO               GPIOA
  #define AUDIO_MUTE_GPIO_PIN           GPIO_Pin_7  // PA.07
  #define AUDIO_OUTPUT_GPIO             GPIOA
  #define AUDIO_OUTPUT_GPIO_PIN         GPIO_Pin_4  // PA.04
  #define AUDIO_GPIO_AF                 GPIO_AF_DAC1
  #define AUDIO_GPIO_PinSource          GPIO_PinSource4
  #define AUDIO_DMA_Stream              DMA1_Stream5
  #define AUDIO_DMA_Stream_IRQn         DMA1_Stream5_IRQn
  #define AUDIO_TIM_IRQn                TIM6_DAC_IRQn
  #define AUDIO_TIM_IRQHandler          TIM6_DAC_IRQHandler
  #define AUDIO_DMA_Stream_IRQHandler   DMA1_Stream5_IRQHandler
  #define AUDIO_TIMER                   TIM6
  #define AUDIO_DMA                     DMA1
#endif

// I2C Bus
#define I2C_RCC_AHB1Periph              RCC_AHB1Periph_GPIOB
#define I2C_RCC_APB1Periph              RCC_APB1Periph_I2C1
#define I2C                             I2C1
#define I2C_GPIO                        GPIOB
#define I2C_SCL_GPIO_PIN                GPIO_Pin_8  // PB.08
#define I2C_SDA_GPIO_PIN                GPIO_Pin_9  // PB.09
#define I2C_GPIO_AF                     GPIO_AF_I2C1
#define I2C_SCL_GPIO_PinSource          GPIO_PinSource8
#define I2C_SDA_GPIO_PinSource          GPIO_PinSource9
#define I2C_SPEED                       400000

// Haptic
#define HAPTIC_PWM
#if defined(PCBX12S)
  #define HAPTIC_RCC_AHB1Periph         RCC_AHB1Periph_GPIOA
  #define HAPTIC_RCC_APB2Periph         RCC_APB2ENR_TIM9EN
  #define HAPTIC_GPIO                   GPIOA
  #define HAPTIC_GPIO_PIN               GPIO_Pin_2
  #define HAPTIC_GPIO_TIMER             TIM9
  #define HAPTIC_GPIO_AF                GPIO_AF_TIM9
  #define HAPTIC_GPIO_PinSource         GPIO_PinSource2
  #define HAPTIC_TIMER_OUTPUT_ENABLE    TIM_CCER_CC1E
  #define HAPTIC_TIMER_MODE             TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2
  #define HAPTIC_TIMER_COMPARE_VALUE    HAPTIC_GPIO_TIMER->CCR1
#elif defined(PCBX10)
  #define HAPTIC_RCC_AHB1Periph         RCC_AHB1Periph_GPIOE
  #define HAPTIC_RCC_APB2Periph         RCC_APB2ENR_TIM9EN
  #define HAPTIC_GPIO                   GPIOE
  #define HAPTIC_GPIO_PIN               GPIO_Pin_6  // PE.06
  #define HAPTIC_GPIO_TIMER             TIM9
  #define HAPTIC_GPIO_AF                GPIO_AF_TIM9
  #define HAPTIC_GPIO_PinSource         GPIO_PinSource6
  #define HAPTIC_TIMER_OUTPUT_ENABLE    TIM_CCER_CC2E
  #define HAPTIC_TIMER_MODE             TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2
  #define HAPTIC_TIMER_COMPARE_VALUE    HAPTIC_GPIO_TIMER->CCR2
#endif

// Internal Module
#define INTMODULE_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2)
#define INTMODULE_PWR_GPIO              GPIOA
#define INTMODULE_PWR_GPIO_PIN          GPIO_Pin_8  // PA.08
#define INTMODULE_TX_GPIO               GPIOB
#define INTMODULE_TX_GPIO_PIN           GPIO_Pin_6  // PB.06
#define INTMODULE_RX_GPIO               GPIOB
#define INTMODULE_RX_GPIO_PIN           GPIO_Pin_7  // PB.07
#define INTMODULE_TX_GPIO_PinSource     GPIO_PinSource6
#define INTMODULE_RX_GPIO_PinSource     GPIO_PinSource7
#define INTMODULE_USART                 USART1
#define INTMODULE_TX_GPIO_AF            GPIO_AF_USART1
#define INTMODULE_USART_IRQn            USART1_IRQn
#define INTMODULE_DMA_STREAM            DMA2_Stream7
#define INTMODULE_DMA_STREAM_IRQ        DMA2_Stream7_IRQn
#define INTMODULE_DMA_STREAM_IRQHandler DMA2_Stream7_IRQHandler
#define INTMODULE_DMA_FLAG_TC           DMA_IT_TCIF7
#define INTMODULE_DMA_CHANNEL           DMA_Channel_4
#if defined(PCBX12S)
  #define INTMODULE_BOOT_GPIO           GPIOI
  #define INTMODULE_BOOT_GPIO_PIN       GPIO_PIN_9  // PC.02
#elif defined(PCBX10)
  #define INTMODULE_BOOT_GPIO           GPIOI
  #define INTMODULE_BOOT_GPIO_PIN       GPIO_PIN_9  // PI.09
#endif
#if defined(PCBX10) || PCBREV >= 13
  #define INTMODULE_RCC_APB1Periph      RCC_APB1Periph_TIM2
  #define INTMODULE_RCC_APB2Periph      RCC_APB2Periph_USART1
  #define INTMODULE_TIMER               TIM2
  #define INTMODULE_TIMER_IRQn          TIM2_IRQn
  #define INTMODULE_TIMER_IRQHandler    TIM2_IRQHandler
  #define INTMODULE_TIMER_FREQ          (PERI1_FREQUENCY * TIMER_MULT_APB1)
#else
  #define INTMODULE_RCC_APB1Periph      0
  #define INTMODULE_RCC_APB2Periph      (RCC_APB2Periph_TIM1 | RCC_APB2Periph_USART1)
  #define INTMODULE_TIMER               TIM1
  #define INTMODULE_TIMER_IRQn          TIM1_CC_IRQn
  #define INTMODULE_TIMER_IRQHandler    TIM1_CC_IRQHandler
  #define INTMODULE_TIMER_FREQ          (PERI2_FREQUENCY * TIMER_MULT_APB2)
#endif

// External Module
#define EXTMODULE_PULSES
#define EXTMODULE_PWR_GPIO              GPIOB
#define EXTMODULE_PWR_GPIO_PIN          GPIO_Pin_3  // PB.03
#if defined(PCBX10) || PCBREV >= 13
  #define EXTMODULE_RCC_AHB1Periph      (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2)
  #define EXTMODULE_RCC_APB1Periph      0
  #define EXTMODULE_RCC_APB2Periph      RCC_APB2Periph_TIM1
  #define EXTMODULE_TX_GPIO             GPIOA
  #define EXTMODULE_TX_GPIO_PIN         GPIO_Pin_10 // PA.10
  #define EXTMODULE_TX_GPIO_PinSource   GPIO_PinSource10
  #define EXTMODULE_TX_GPIO_AF          GPIO_AF_TIM1
  #define EXTMODULE_TIMER               TIM1
  #define EXTMODULE_TIMER_IRQn          TIM1_CC_IRQn
  #define EXTMODULE_TIMER_IRQHandler    TIM1_CC_IRQHandler
  #define EXTMODULE_TIMER_FREQ          (PERI2_FREQUENCY * TIMER_MULT_APB2)
  #define EXTMODULE_DMA_CHANNEL         DMA_Channel_6
  #define EXTMODULE_DMA_STREAM          DMA2_Stream5
  #define EXTMODULE_DMA_IRQn            DMA2_Stream5_IRQn
  #define EXTMODULE_DMA_IRQHandler      DMA2_Stream5_IRQHandler
  #define EXTMODULE_DMA_FLAG_TC         DMA_IT_TCIF5
#else
  #define EXTMODULE_RCC_AHB1Periph      (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1)
  #define EXTMODULE_RCC_APB1Periph      RCC_APB1Periph_TIM2
  #define EXTMODULE_RCC_APB2Periph      0
  #define EXTMODULE_TX_GPIO             GPIOA
  #define EXTMODULE_TX_GPIO_PIN         GPIO_Pin_15 // PA.15
  #define EXTMODULE_TX_GPIO_PinSource   GPIO_PinSource15
  #define EXTMODULE_TX_GPIO_AF          GPIO_AF_TIM2
  #define EXTMODULE_TIMER               TIM2
  #define EXTMODULE_TIMER_IRQn          TIM2_IRQn
  #define EXTMODULE_TIMER_IRQHandler    TIM2_IRQHandler
  #define EXTMODULE_TIMER_FREQ          (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #define EXTMODULE_DMA_CHANNEL         DMA_Channel_3
  #define EXTMODULE_DMA_STREAM          DMA1_Stream7
  #define EXTMODULE_DMA_IRQn            DMA1_Stream7_IRQn
  #define EXTMODULE_DMA_IRQHandler      DMA1_Stream7_IRQHandler
  #define EXTMODULE_DMA_FLAG_TC         DMA_IT_TCIF7
#endif

// Heartbeat (not used)
#define HEARTBEAT_RCC_AHB1Periph        RCC_AHB1Periph_GPIOD
#define HEARTBEAT_GPIO                  GPIOD
#define HEARTBEAT_GPIO_PIN              GPIO_Pin_12 // PD.12

// Trainer Port
#define TRAINER_RCC_AHB1Periph          (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA1)
#define TRAINER_RCC_APB1Periph          RCC_APB1Periph_TIM3
#define TRAINER_GPIO                    GPIOC
#define TRAINER_IN_GPIO_PIN             GPIO_Pin_6  // PC.06
#define TRAINER_IN_GPIO_PinSource       GPIO_PinSource6
#define TRAINER_OUT_GPIO_PIN            GPIO_Pin_7  // PC.07
#define TRAINER_OUT_GPIO_PinSource      GPIO_PinSource7
#define TRAINER_DETECT_GPIO             GPIOB
#define TRAINER_DETECT_GPIO_PIN         GPIO_Pin_4  // PB.04
#define TRAINER_TIMER                   TIM3
#define TRAINER_TIMER_IRQn              TIM3_IRQn
#define TRAINER_TIMER_IRQHandler        TIM3_IRQHandler
#define TRAINER_GPIO_AF                 GPIO_AF_TIM3
#define TRAINER_DMA                     DMA1
#define TRAINER_DMA_CHANNEL             DMA_Channel_5
#define TRAINER_DMA_STREAM              DMA1_Stream2
#define TRAINER_DMA_IRQn                DMA1_Stream2_IRQn
#define TRAINER_DMA_IRQHandler          DMA1_Stream2_IRQHandler
#define TRAINER_DMA_FLAG_TC             DMA_IT_TCIF2
#define TRAINER_TIMER_FREQ              (PERI1_FREQUENCY * TIMER_MULT_APB1)

// Xms Interrupt
#define INTERRUPT_xMS_RCC_APB1Periph    RCC_APB1Periph_TIM14
#define INTERRUPT_xMS_TIMER             TIM14
#define INTERRUPT_xMS_IRQn              TIM8_TRG_COM_TIM14_IRQn
#define INTERRUPT_xMS_IRQHandler        TIM8_TRG_COM_TIM14_IRQHandler

// 2MHz Timer
#define TIMER_2MHz_RCC_APB1Periph       RCC_APB1Periph_TIM7
#define TIMER_2MHz_TIMER                TIM7

// Bluetooth
#define BT_RCC_APB2Periph               RCC_APB2Periph_USART6
#define BT_USART                        USART6
#define BT_GPIO_AF                      GPIO_AF_USART6
#define BT_USART_IRQn                   USART6_IRQn
#define BT_GPIO_TXRX                    GPIOG
#define BT_TX_GPIO_PIN                  GPIO_Pin_14 // PG.14
#define BT_RX_GPIO_PIN                  GPIO_Pin_9  // PG.09
#define BT_TX_GPIO_PinSource            GPIO_PinSource14
#define BT_RX_GPIO_PinSource            GPIO_PinSource9
#define BT_USART_IRQHandler             USART6_IRQHandler
#if defined(PCBX12S)
  #if PCBREV >= 13
    #define BT_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOG)
    #define BT_EN_GPIO                  GPIOI
    #define BT_EN_GPIO_PIN              GPIO_Pin_10 // PI.10
  #else
    #define BT_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOG)
    #define BT_EN_GPIO                  GPIOA
    #define BT_EN_GPIO_PIN              GPIO_Pin_6 // PA.06
  #endif
  #define BT_BRTS_GPIO                  GPIOG
  #define BT_BRTS_GPIO_PIN              GPIO_Pin_10 // PG.10
  #define BT_BCTS_GPIO                  GPIOG
  #define BT_BCTS_GPIO_PIN              GPIO_Pin_11 // PG.11
#elif defined(PCBX10)
  #define BT_RCC_AHB1Periph             RCC_AHB1Periph_GPIOG
  #define BT_EN_GPIO                    GPIOG
  #define BT_EN_GPIO_PIN                GPIO_Pin_10 // PG.10
#endif

// GPS
#if defined(PCBX12S)
  #define GPS_RCC_AHB1Periph            RCC_AHB1Periph_GPIOA
  #define GPS_RCC_APB1Periph            RCC_APB1Periph_UART4
  #define GPS_USART                     UART4
  #define GPS_GPIO_AF                   GPIO_AF_UART4
  #define GPS_USART_IRQn                UART4_IRQn
  #define GPS_USART_IRQHandler          UART4_IRQHandler
  #define GPS_UART_GPIO                 GPIOA
  #define GPS_TX_GPIO_PIN               GPIO_Pin_0 // PA.00
  #define GPS_RX_GPIO_PIN               GPIO_Pin_1 // PA.01
  #define GPS_TX_GPIO_PinSource         GPIO_PinSource0
  #define GPS_RX_GPIO_PinSource         GPIO_PinSource1
#else
  #define GPS_RCC_AHB1Periph            0
  #define GPS_RCC_APB1Periph            0
#endif

#endif // _HAL_H_
