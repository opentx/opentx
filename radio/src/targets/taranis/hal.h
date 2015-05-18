#ifndef _HAL_
#define _HAL_

// Keys
#if defined(REV9E)
  #define RCC_AHB1Periph_GPIOBUTTON     (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG)
#else
  #define RCC_AHB1Periph_GPIOBUTTON     (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
#endif

#define GPIO_BUTTON_PLUS                GPIOE->IDR
#define GPIO_BUTTON_MINUS               GPIOE->IDR
#define GPIO_BUTTON_MENU                GPIOD->IDR
#define GPIO_BUTTON_EXIT                GPIOD->IDR
#define GPIO_BUTTON_PAGE                GPIOD->IDR
#if defined(REV9E)
  #if defined(SIMU)
    #define PIN_BUTTON_PLUS             GPIO_Pin_5  // This is for SIMU: reuse rotary encoder pins to map UP and DOWN keyboard keys
    #define PIN_BUTTON_MINUS            GPIO_Pin_6  // This is for SIMU: reuse rotary encoder pins to map UP and DOWN keyboard keys
  #endif
  #define GPIO_BUTTON_ENTER             GPIOF->IDR
  #define PIN_BUTTON_ENTER              GPIO_Pin_0  // PF.00
#else
  #define PIN_BUTTON_PLUS               GPIO_Pin_10 // PE.10
  #define PIN_BUTTON_MINUS              GPIO_Pin_11 // PE.11
  #define GPIO_BUTTON_ENTER             GPIOE->IDR
  #define PIN_BUTTON_ENTER              GPIO_Pin_12 // PE.12
#endif
#define PIN_BUTTON_MENU                 GPIO_Pin_7  // PD.07
#define PIN_BUTTON_PAGE                 GPIO_Pin_3  // PD.02
#define PIN_BUTTON_EXIT                 GPIO_Pin_2  // PD.03

// Trims
#define GPIO_TRIM_RV_DN                 GPIOC->IDR
#define PIN_TRIM_RV_DN                  GPIO_Pin_3  // PC.03
#define GPIO_TRIM_RV_UP                 GPIOC->IDR
#define PIN_TRIM_RV_UP                  GPIO_Pin_2  // PC.02
#define GPIO_TRIM_RH_L                  GPIOC->IDR
#define PIN_TRIM_RH_L                   GPIO_Pin_1  // PC.01
#define GPIO_TRIM_RH_R                  GPIOC->IDR
#define PIN_TRIM_RH_R                   GPIO_Pin_13 // PC.13
#define GPIO_TRIM_LH_L                  GPIOE->IDR
#define PIN_TRIM_LH_L                   GPIO_Pin_4  // PE.04
#define GPIO_TRIM_LH_R                  GPIOE->IDR
#define PIN_TRIM_LH_R                   GPIO_Pin_3  // PE.03
#if defined(REV9E)
  #define GPIO_TRIM_LV_DN               GPIOG->IDR
  #define PIN_TRIM_LV_DN                GPIO_Pin_1  // PG.01
  #define GPIO_TRIM_LV_UP               GPIOG->IDR
  #define PIN_TRIM_LV_UP                GPIO_Pin_0  // PG.00
#else
  #define GPIO_TRIM_LV_DN               GPIOE->IDR
  #define PIN_TRIM_LV_DN                GPIO_Pin_6  // PE.06
  #define GPIO_TRIM_LV_UP               GPIOE->IDR
  #define PIN_TRIM_LV_UP                GPIO_Pin_5  // PE.05
#endif

// Switches
#define GPIO_PIN_SW_A_H                 GPIOB->IDR
#define PIN_SW_A_H                      GPIO_Pin_5  // PB.05
#define GPIO_PIN_SW_A_L                 GPIOE->IDR
#define PIN_SW_A_L                      GPIO_Pin_0  // PE.00

#define GPIO_PIN_SW_B_H                 GPIOE->IDR
#define PIN_SW_B_H                      GPIO_Pin_1  // PE.01
#define GPIO_PIN_SW_B_L                 GPIOE->IDR
#define PIN_SW_B_L                      GPIO_Pin_2  // PE.02

#define GPIO_PIN_SW_C_H                 GPIOE->IDR
#define PIN_SW_C_H                      GPIO_Pin_15 // PE.15
#define GPIO_PIN_SW_C_L                 GPIOA->IDR
#define PIN_SW_C_L                      GPIO_Pin_5  // PA.05

#if defined(REVPLUS)
  #define GPIO_PIN_SW_D_H               (GPIOE->IDR)
  #define PIN_SW_D_H                    GPIO_Pin_7  // PE.07
  #define GPIO_PIN_SW_D_L               (GPIOE->IDR)
  #define PIN_SW_D_L                    GPIO_Pin_13 // PE.13
#else
  #define GPIO_PIN_SW_D_H               GPIOE->IDR
  #define PIN_SW_D_H                    GPIO_Pin_7  // PE.07
  #define GPIO_PIN_SW_D_L               GPIOB->IDR
  #define PIN_SW_D_L                    GPIO_Pin_1  // PB.01
#endif

#define GPIO_PIN_SW_E_L                 GPIOB->IDR
#define PIN_SW_E_L                      GPIO_Pin_3  // PB.03
#define GPIO_PIN_SW_E_H                 GPIOB->IDR
#define PIN_SW_E_H                      GPIO_Pin_4  // PB.04

#if defined(REV9E)
  #define GPIO_PIN_SW_F_L               GPIOF->IDR
  #define PIN_SW_F_L                    GPIO_Pin_15 // PF.15
  #define GPIO_PIN_SW_F_H               GPIOE->IDR
  #define PIN_SW_F_H                    GPIO_Pin_14 // PE.14
#else
  #define GPIO_PIN_SW_F                 GPIOE->IDR
  #define PIN_SW_F                      GPIO_Pin_14 // PE.14
#endif

#define GPIO_PIN_SW_G_H                 GPIOE->IDR
#define PIN_SW_G_H                      GPIO_Pin_9  // PE.09
#define GPIO_PIN_SW_G_L                 GPIOE->IDR
#define PIN_SW_G_L                      GPIO_Pin_8  // PE.08

#if defined(REV9E)
  #define GPIO_PIN_SW_H_H               GPIOD->IDR
  #define PIN_SW_H_H                    GPIO_Pin_10 // PD.10
  #define GPIO_PIN_SW_H_L               GPIOD->IDR
  #define PIN_SW_H_L                    GPIO_Pin_14 // PD.14
#elif defined(REVPLUS)
  #define GPIO_PIN_SW_H                 GPIOD->IDR
  #define PIN_SW_H                      GPIO_Pin_14 // PD.14
#else
  #define GPIO_PIN_SW_H                 GPIOE->IDR
  #define PIN_SW_H                      GPIO_Pin_13 // PE.13
#endif

#if defined(REV9E)
  #define GPIO_PIN_SW_I_H               GPIOF->IDR
  #define PIN_SW_I_H                    GPIO_Pin_1  // PF.01
  #define GPIO_PIN_SW_I_L               GPIOF->IDR
  #define PIN_SW_I_L                    GPIO_Pin_2  // PF.02
  #define GPIO_PIN_SW_J_H               GPIOF->IDR
  #define PIN_SW_J_H                    GPIO_Pin_3  // PF.03
  #define GPIO_PIN_SW_J_L               GPIOF->IDR
  #define PIN_SW_J_L                    GPIO_Pin_4  // PF.04
  #define GPIO_PIN_SW_K_H               GPIOF->IDR
  #define PIN_SW_K_H                    GPIO_Pin_5  // PF.05
  #define GPIO_PIN_SW_K_L               GPIOF->IDR
  #define PIN_SW_K_L                    GPIO_Pin_6  // PF.06
  #define GPIO_PIN_SW_L_H               GPIOF->IDR
  #define PIN_SW_L_H                    GPIO_Pin_7  // PF.07
  #define GPIO_PIN_SW_L_L               GPIOE->IDR
  #define PIN_SW_L_L                    GPIO_Pin_10 // PE.10
  #define GPIO_PIN_SW_M_H               GPIOF->IDR
  #define PIN_SW_M_H                    GPIO_Pin_11 // PF.11
  #define GPIO_PIN_SW_M_L               GPIOF->IDR
  #define PIN_SW_M_L                    GPIO_Pin_12 // PF.12
  #define GPIO_PIN_SW_N_H               GPIOF->IDR
  #define PIN_SW_N_H                    GPIO_Pin_13 // PF.13
  #define GPIO_PIN_SW_N_L               GPIOF->IDR
  #define PIN_SW_N_L                    GPIO_Pin_14 // PF.14
  #define GPIO_PIN_SW_O_H               GPIOG->IDR
  #define PIN_SW_O_H                    GPIO_Pin_13 // PG.13
  #define GPIO_PIN_SW_O_L               GPIOG->IDR
  #define PIN_SW_O_L                    GPIO_Pin_12 // PG.12
  #define GPIO_PIN_SW_P_H               GPIOG->IDR
  #define PIN_SW_P_H                    GPIO_Pin_11 // PG.11
  #define GPIO_PIN_SW_P_L               GPIOG->IDR
  #define PIN_SW_P_L                    GPIO_Pin_10 // PG.10
  #define GPIO_PIN_SW_Q_H               GPIOE->IDR
  #define PIN_SW_Q_H                    GPIO_Pin_11 // PE.11
  #define GPIO_PIN_SW_Q_L               GPIOE->IDR
  #define PIN_SW_Q_L                    GPIO_Pin_12 // PE.12
  #define GPIO_PIN_SW_R_H               GPIOG->IDR
  #define PIN_SW_R_H                    GPIO_Pin_7  // PG.07
  #define GPIO_PIN_SW_R_L               GPIOG->IDR
  #define PIN_SW_R_L                    GPIO_Pin_8  // PG.08
#endif

// ADC
#define PIN_STK_J1                      GPIO_Pin_0  // PA.00
#define PIN_STK_J2                      GPIO_Pin_1  // PA.01
#define PIN_STK_J3                      GPIO_Pin_2  // PA.02
#define PIN_STK_J4                      GPIO_Pin_3  // PA.03
#define PIN_SLD_J1                      GPIO_Pin_4  // PC.04
#define PIN_SLD_J2                      GPIO_Pin_5  // PC.05
#define PIN_FLP_J1                      GPIO_Pin_6  // PA.06
#define PIN_FLP_J2                      GPIO_Pin_0  // PB.00
#if defined(REV9E)
  #define PIN_FLP_J3                    GPIO_Pin_10 // PF.10
  #define PIN_FLP_J4                    GPIO_Pin_9  // PF.09
  #define PIN_FLP_J5                    GPIO_Pin_8  // PF.08
  #define PIN_FLP_J6                    GPIO_Pin_1  // PB.01
#elif defined(REVPLUS)
  #define PIN_FLP_J3                    GPIO_Pin_1  // PB.01
#endif
#define PIN_MVOLT                       GPIO_Pin_0  // PC.00
#define RCC_AHB1Periph_GPIOADC          RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC

// Power_OFF Delay and LED
#define PIN_PWR_LED                     GPIO_Pin_6  // PC.06
#define PIN_PWR_STATUS                  GPIO_Pin_1  // PD.01
#define PIN_MCU_PWR                     GPIO_Pin_0  // PD.00
#define RCC_AHB1Periph_GPIOPWR          RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD
#define GPIOPWRLED                      GPIOC
#define GPIOPWR                         GPIOD
#define GPIO_EXT_RF_PWR                 GPIOD
#define PIN_EXT_RF_PWR                  GPIO_Pin_8
#if defined(REVPLUS)
  #define GPIO_INT_RF_PWR               GPIOC
  #define PIN_INT_RF_PWR                GPIO_Pin_6
#else
  #define GPIO_INT_RF_PWR               GPIOD
  #define PIN_INT_RF_PWR                GPIO_Pin_15
#endif

// Trainer Port
#define GPIO_TR_INOUT                   GPIOC
#define PIN_TR_PPM_IN                   GPIO_Pin_8  // PC.08
#define PIN_TR_PPM_OUT                  GPIO_Pin_9  // PC.09
#define GPIOTRNDET                      GPIOA
#define PIN_TRNDET                      GPIO_Pin_8

// Cppm
#define RCC_AHB1Periph_GPIO_INTPPM      RCC_AHB1Periph_GPIOA
#define PIN_INTPPM_OUT                  GPIO_Pin_10 // PA.10
#define GPIO_INTPPM                     GPIOA
#define GPIO_PinSource_INTPPM           GPIO_PinSource10
#define RCC_AHB1Periph_GPIO_EXTPPM      RCC_AHB1Periph_GPIOA
#define PIN_EXTPPM_OUT                  GPIO_Pin_7  // PA.07
#define GPIO_EXTPPM                     GPIOA
#define GPIO_PinSource_EXTPPM           GPIO_PinSource7

// Heart Beat
#define PIN_HEART_BEAT                  GPIO_Pin_7  // PC.07

// UART3
#define RCC_AHB1Periph_GPIO_UART3       RCC_AHB1Periph_GPIOB
#define RCC_APB1Periph_UART3            RCC_APB1Periph_USART3
#define GPIO_UART3                      GPIOB
#define GPIO_PIN_UART3_TX               GPIO_Pin_10 // PB.10
#define GPIO_PIN_UART3_RX               GPIO_Pin_11 // PB.11
#define GPIO_PinSource_UART3_TX         GPIO_PinSource10
#define GPIO_PinSource_UART3_RX         GPIO_PinSource11
#define GPIO_AF_UART3                   GPIO_AF_USART3
#define UART3                           USART3

// Smart-Port
#define RCC_AHB1Periph_GPIO_SPORT       RCC_AHB1Periph_GPIOD
#define RCC_APB1Periph_SPORT            RCC_APB1Periph_USART2
#define GPIO_PIN_SPORT_ON               GPIOD
#define PIN_SPORT_ON                    GPIO_Pin_4  // PD.04
#define GPIO_PIN_SPORT_TXRX             GPIOD
#define PIN_SPORT_TX                    GPIO_Pin_5  // PD.05
#define PIN_SPORT_RX                    GPIO_Pin_6  // PD.06
#define GPIO_PinSource_SPORT_TX         GPIO_PinSource5
#define GPIO_PinSource_SPORT_RX         GPIO_PinSource6
#define GPIO_AF_SPORT                   GPIO_AF_USART2
#define SPORT_USART                     USART2
#define SPORT_IRQHandler                USART2_IRQHandler
#define SPORT_IRQn                      USART2_IRQn

// USB_OTG
#define PIN_FS_VBUS                     GPIO_Pin_9  // PA.09
#define PIN_FS_DM                       GPIO_Pin_11 // PA.11
#define PIN_FS_DP                       GPIO_Pin_12 // PA.12

// BackLight
#if defined(REV9E)
  #define TIM_BL                        TIM9
  #define GPIOBL                        GPIOE
  #define RCC_AHB1Periph_GPIOBL         RCC_AHB1Periph_GPIOE
  #define Pin_BL_AF                     GPIO_AF_TIM9
  #define GPIO_Pin_BL                   GPIO_Pin_6 // PE.06
  #define GPIO_PinSource_BL             GPIO_PinSource6
  #define Pin_BLW_AF                    GPIO_AF_TIM9
  #define GPIO_Pin_BLW                  GPIO_Pin_5 // PE.05
  #define GPIO_PinSource_BLW            GPIO_PinSource5
#elif defined (REVPLUS)
  #define TIM_BL                        TIM4
  #define GPIOBL                        GPIOD
  #define RCC_AHB1Periph_GPIOBL         RCC_AHB1Periph_GPIOD
  #define Pin_BL_AF                     GPIO_AF_TIM4
  #define GPIO_Pin_BL                   GPIO_Pin_15 // PD.15
  #define GPIO_PinSource_BL             GPIO_PinSource15
  #define Pin_BLW_AF                    GPIO_AF_TIM4
  #define GPIO_Pin_BLW                  GPIO_Pin_13 // PD.13
  #define GPIO_PinSource_BLW            GPIO_PinSource13
#else
  #define RCC_AHB1Periph_GPIOBL         RCC_AHB1Periph_GPIOB
  #define GPIO_Pin_BL                   GPIO_Pin_8  // PB.08
  #define Pin_BL_AF                     GPIO_AF_TIM10
  #define GPIOBL                        GPIOB
  #define GPIO_PinSource_BL             GPIO_PinSource8
#endif

// LCD
#if defined(REV9E)
  #define RCC_AHB1Periph_LCD            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD)
  #define RCC_AHB1Periph_LCD_RST        RCC_AHB1Periph_GPIOD
  #define RCC_AHB1Periph_LCD_NCS        RCC_AHB1Periph_GPIOA
  #define GPIO_LCD_SPI                  GPIOC
  #define GPIO_LCD_NCS                  GPIOA
  #define GPIO_LCD_RST                  GPIOD
  #define PIN_LCD_MOSI                  GPIO_Pin_12 // PC.12
  #define PIN_LCD_CLK                   GPIO_Pin_10 // PC.10
  #define PIN_LCD_NCS                   GPIO_Pin_15 // PA.15
  #define PIN_LCD_A0                    GPIO_Pin_11 // PC.11
  #define PIN_LCD_RST                   GPIO_Pin_15 // PD.15
#elif defined(REVPLUS)
  #define RCC_AHB1Periph_LCD            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD)
  #define RCC_AHB1Periph_LCD_RST        RCC_AHB1Periph_GPIOD
  #define RCC_AHB1Periph_LCD_NCS        RCC_AHB1Periph_GPIOA
  #define GPIO_LCD_SPI                  GPIOC
  #define GPIO_LCD_NCS                  GPIOA
  #define GPIO_LCD_RST                  GPIOD
  #define PIN_LCD_MOSI                  GPIO_Pin_12 // PC.12
  #define PIN_LCD_CLK                   GPIO_Pin_10 // PC.10
  #define PIN_LCD_NCS                   GPIO_Pin_15 // PA.15
  #define PIN_LCD_A0                    GPIO_Pin_11 // PC.11
  #define PIN_LCD_RST                   GPIO_Pin_12 // PD.12
#else
  #define RCC_AHB1Periph_LCD            RCC_AHB1Periph_GPIOD
  #define GPIO_LCD_SPI                  GPIOD
  #define GPIO_LCD_NCS                  GPIOD
  #define GPIO_LCD_RST                  GPIOD
  #define PIN_LCD_MOSI                  GPIO_Pin_10 // PD.10
  #define PIN_LCD_CLK                   GPIO_Pin_11 // PD.11
  #define PIN_LCD_NCS                   GPIO_Pin_14 // PD.14
  #define PIN_LCD_A0                    GPIO_Pin_13 // PD.13
  #define PIN_LCD_RST                   GPIO_Pin_12 // PD.12
#endif 

// I2C Bus: EEPROM and CAT5137
#define I2C_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOB
#define I2C_RCC_APB1Periph_I2C          RCC_APB1Periph_I2C1
#define I2C                             I2C1
#define I2C_GPIO                        GPIOB
#define I2C_GPIO_PIN_SCL                GPIO_Pin_6  // PB.06
#define I2C_GPIO_PIN_SDA                GPIO_Pin_7  // PB.07
#define I2C_GPIO_PIN_WP                 GPIO_Pin_9  // PB.09
#define I2C_GPIO_AF                     GPIO_AF_I2C1
#define I2C_GPIO_PinSource_SCL          GPIO_PinSource6
#define I2C_GPIO_PinSource_SDA          GPIO_PinSource7
#define I2C_ADDRESS_EEPROM              0xA2
#define I2C_ADDRESS_CAT5137             0x5C
#define I2C_FLASH_PAGESIZE              64

// SD - SPI2
#define SD_RCC_AHB1Periph_GPIO          (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB)
#define SD_RCC_APB1Periph_SPI           RCC_APB1Periph_SPI2
#define SD_RCC_AHB1Periph_DMA           RCC_AHB1Periph_DMA1
#define SD_GPIO_PRESENT                 GPIOD
#define SD_GPIO_PIN_PRESENT             GPIO_Pin_9  // PD.09
#define SD_GPIO                         GPIOB
#define SD_GPIO_PIN_CS                  GPIO_Pin_12 // PB.12
#define SD_GPIO_PIN_SCK                 GPIO_Pin_13 // PB.13
#define SD_GPIO_PIN_MISO                GPIO_Pin_14 // PB.14
#define SD_GPIO_PIN_MOSI                GPIO_Pin_15 // PB.15
#define SD_GPIO_AF                      GPIO_AF_SPI2
#define SD_GPIO_PinSource_CS            GPIO_PinSource12
#define SD_GPIO_PinSource_SCK           GPIO_PinSource13
#define SD_GPIO_PinSource_MISO          GPIO_PinSource14
#define SD_GPIO_PinSource_MOSI          GPIO_PinSource15
#define SD_SPI                          SPI2
#define SD_SPI_BaudRatePrescaler        SPI_BaudRatePrescaler_4 // 10.5<20MHZ, make sure < 20MHZ

#if !defined(BOOT)
  #define SD_USE_DMA                    // Enable the DMA for SD
  #define SD_DMA_Stream_SPI_RX          DMA1_Stream3
  #define SD_DMA_Stream_SPI_TX          DMA1_Stream4
  #define SD_DMA_FLAG_SPI_TC_RX         DMA_FLAG_TCIF3
  #define SD_DMA_FLAG_SPI_TC_TX         DMA_FLAG_TCIF4
  #define SD_DMA_Channel_SPI            DMA_Channel_0
#endif

// Haptic
#if defined(REVPLUS)
  #define RCC_AHB1Periph_GPIOHAPTIC     RCC_AHB1Periph_GPIOB
  #define GPIO_PinSource_HAPTIC         GPIO_PinSource8
  #define GPIO_HAPTIC                   GPIOB
  #define PIN_HAPTIC                    GPIO_Pin_8
#else
  #define RCC_AHB1Periph_GPIOHAPTIC     RCC_AHB1Periph_GPIOC
  #define GPIO_HAPTIC                   GPIOC
  #define PIN_HAPTIC                    GPIO_Pin_12
#endif

// Top LCD on X9E
#if defined(REV9E)
  #define GPIO_VA                       GPIOG
  #define PIN_VA_DATA                   GPIO_Pin_5
  #define PIN_VA_WR                     GPIO_Pin_4
  #define PIN_VA_CS                     GPIO_Pin_3
  #define PIN_VA_CS2                    GPIO_Pin_15
  #define PIN_VA_BL                     GPIO_Pin_2
  #define RCC_AHB1Periph_VA             RCC_AHB1Periph_GPIOG
  #define CS1_HIGH                      GPIO_VA->BSRRL = PIN_VA_CS
  #define CS1_LOW                       GPIO_VA->BSRRH = PIN_VA_CS
  #define CS2_HIGH                      GPIO_VA->BSRRL = PIN_VA_CS2
  #define CS2_LOW                       GPIO_VA->BSRRH = PIN_VA_CS2
  #define WR_HIGH                       GPIO_VA->BSRRL = PIN_VA_WR
  #define WR_LOW                        GPIO_VA->BSRRH = PIN_VA_WR
  #define DATA_HIGH                     GPIO_VA->BSRRL = PIN_VA_DATA
  #define DATA_LOW                      GPIO_VA->BSRRH = PIN_VA_DATA
  #define VA_BL_ON                      GPIO_VA->BSRRL = PIN_VA_BL
  #define VA_BL_OFF                     GPIO_VA->BSRRH = PIN_VA_BL
#endif

#endif
