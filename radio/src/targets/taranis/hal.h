#ifndef _HAL_
#define _HAL_

// Keys
#if defined(REV9E)
  #define KEYS_RCC_AHB1Periph_GPIO      (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG)
#else
  #define KEYS_RCC_AHB1Periph_GPIO      (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
#endif
#define KEYS_GPIO_REG_PLUS              GPIOE->IDR
#define KEYS_GPIO_REG_MINUS             GPIOE->IDR
#define KEYS_GPIO_REG_MENU              GPIOD->IDR
#define KEYS_GPIO_REG_EXIT              GPIOD->IDR
#define KEYS_GPIO_REG_PAGE              GPIOD->IDR
#if defined(REV9E)
  #if defined(SIMU)
    #define KEYS_GPIO_PIN_PLUS          GPIO_Pin_5  // This is for SIMU: reuse rotary encoder pins to map UP and DOWN keyboard keys
    #define KEYS_GPIO_PIN_MINUS         GPIO_Pin_6  // This is for SIMU: reuse rotary encoder pins to map UP and DOWN keyboard keys
  #endif
  #define KEYS_GPIO_REG_ENTER           GPIOF->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_0  // PF.00
#else
  #define KEYS_GPIO_PIN_PLUS            GPIO_Pin_10 // PE.10
  #define KEYS_GPIO_PIN_MINUS           GPIO_Pin_11 // PE.11
  #define KEYS_GPIO_REG_ENTER           GPIOE->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_12 // PE.12
#endif
#define KEYS_GPIO_PIN_MENU              GPIO_Pin_7  // PD.07
#define KEYS_GPIO_PIN_PAGE              GPIO_Pin_3  // PD.02
#define KEYS_GPIO_PIN_EXIT              GPIO_Pin_2  // PD.03

// Trims
#define TRIMS_GPIO_REG_RVD              GPIOC->IDR
#define TRIMS_GPIO_PIN_RVD              GPIO_Pin_3  // PC.03
#define TRIMS_GPIO_REG_RVU              GPIOC->IDR
#define TRIMS_GPIO_PIN_RVU              GPIO_Pin_2  // PC.02
#define TRIMS_GPIO_REG_RHL              GPIOC->IDR
#define TRIMS_GPIO_PIN_RHL              GPIO_Pin_1  // PC.01
#define TRIMS_GPIO_REG_RHR              GPIOC->IDR
#define TRIMS_GPIO_PIN_RHR              GPIO_Pin_13 // PC.13
#define TRIMS_GPIO_REG_LHL              GPIOE->IDR
#define TRIMS_GPIO_PIN_LHL              GPIO_Pin_4  // PE.04
#define TRIMS_GPIO_REG_LHR              GPIOE->IDR
#define TRIMS_GPIO_PIN_LHR              GPIO_Pin_3  // PE.03
#if defined(REV9E)
  #define TRIMS_GPIO_REG_LVD            GPIOG->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_1  // PG.01
  #define TRIMS_GPIO_REG_LVU            GPIOG->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_0  // PG.00
#else
  #define TRIMS_GPIO_REG_LVD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_6  // PE.06
  #define TRIMS_GPIO_REG_LVU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_5  // PE.05
#endif

// Switches
#define SWITCHES_GPIO_REG_A_H           GPIOB->IDR
#define SWITCHES_GPIO_PIN_A_H           GPIO_Pin_5  // PB.05
#define SWITCHES_GPIO_REG_A_L           GPIOE->IDR
#define SWITCHES_GPIO_PIN_A_L           GPIO_Pin_0  // PE.00

#define SWITCHES_GPIO_REG_B_H           GPIOE->IDR
#define SWITCHES_GPIO_PIN_B_H           GPIO_Pin_1  // PE.01
#define SWITCHES_GPIO_REG_B_L           GPIOE->IDR
#define SWITCHES_GPIO_PIN_B_L           GPIO_Pin_2  // PE.02

#define SWITCHES_GPIO_REG_C_H           GPIOE->IDR
#define SWITCHES_GPIO_PIN_C_H           GPIO_Pin_15 // PE.15
#define SWITCHES_GPIO_REG_C_L           GPIOA->IDR
#define SWITCHES_GPIO_PIN_C_L           GPIO_Pin_5  // PA.05

#if defined(REVPLUS)
  #define SWITCHES_GPIO_REG_D_H         (GPIOE->IDR)
  #define SWITCHES_GPIO_PIN_D_H         GPIO_Pin_7  // PE.07
  #define SWITCHES_GPIO_REG_D_L         (GPIOE->IDR)
  #define SWITCHES_GPIO_PIN_D_L         GPIO_Pin_13 // PE.13
#else
  #define SWITCHES_GPIO_REG_D_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_H         GPIO_Pin_7  // PE.07
  #define SWITCHES_GPIO_REG_D_L         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_D_L         GPIO_Pin_1  // PB.01
#endif

#define SWITCHES_GPIO_REG_E_L           GPIOB->IDR
#define SWITCHES_GPIO_PIN_E_L           GPIO_Pin_3  // PB.03
#define SWITCHES_GPIO_REG_E_H           GPIOB->IDR
#define SWITCHES_GPIO_PIN_E_H           GPIO_Pin_4  // PB.04

#if defined(REV9E)
  #define SWITCHES_GPIO_REG_F_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_F_L         GPIO_Pin_15 // PF.15
  #define SWITCHES_GPIO_REG_F_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_F_H         GPIO_Pin_14 // PE.14
#else
  #define SWITCHES_GPIO_REG_F           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_F           GPIO_Pin_14 // PE.14
#endif

#define SWITCHES_GPIO_REG_G_H           GPIOE->IDR
#define SWITCHES_GPIO_PIN_G_H           GPIO_Pin_9  // PE.09
#define SWITCHES_GPIO_REG_G_L           GPIOE->IDR
#define SWITCHES_GPIO_PIN_G_L           GPIO_Pin_8  // PE.08

#if defined(REV9E)
  #define SWITCHES_GPIO_REG_H_H         GPIOD->IDR
  #define SWITCHES_GPIO_PIN_H_H         GPIO_Pin_10 // PD.10
  #define SWITCHES_GPIO_REG_H_L         GPIOD->IDR
  #define SWITCHES_GPIO_PIN_H_L         GPIO_Pin_14 // PD.14
#elif defined(REVPLUS)
  #define SWITCHES_GPIO_REG_H           GPIOD->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_14 // PD.14
#else
  #define SWITCHES_GPIO_REG_H           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_13 // PE.13
#endif

#if defined(REV9E)
  #define SWITCHES_GPIO_REG_I_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_I_H         GPIO_Pin_1  // PF.01
  #define SWITCHES_GPIO_REG_I_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_I_L         GPIO_Pin_2  // PF.02
  #define SWITCHES_GPIO_REG_J_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_J_H         GPIO_Pin_3  // PF.03
  #define SWITCHES_GPIO_REG_J_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_J_L         GPIO_Pin_4  // PF.04
  #define SWITCHES_GPIO_REG_K_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_K_H         GPIO_Pin_5  // PF.05
  #define SWITCHES_GPIO_REG_K_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_K_L         GPIO_Pin_6  // PF.06
  #define SWITCHES_GPIO_REG_L_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_L_H         GPIO_Pin_7  // PF.07
  #define SWITCHES_GPIO_REG_L_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_L_L         GPIO_Pin_10 // PE.10
  #define SWITCHES_GPIO_REG_M_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_M_H         GPIO_Pin_11 // PF.11
  #define SWITCHES_GPIO_REG_M_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_M_L         GPIO_Pin_12 // PF.12
  #define SWITCHES_GPIO_REG_N_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_N_H         GPIO_Pin_13 // PF.13
  #define SWITCHES_GPIO_REG_N_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_N_L         GPIO_Pin_14 // PF.14
  #define SWITCHES_GPIO_REG_O_H         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_O_H         GPIO_Pin_13 // PG.13
  #define SWITCHES_GPIO_REG_O_L         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_O_L         GPIO_Pin_12 // PG.12
  #define SWITCHES_GPIO_REG_P_H         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_P_H         GPIO_Pin_11 // PG.11
  #define SWITCHES_GPIO_REG_P_L         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_P_L         GPIO_Pin_10 // PG.10
  #define SWITCHES_GPIO_REG_Q_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_Q_H         GPIO_Pin_11 // PE.11
  #define SWITCHES_GPIO_REG_Q_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_Q_L         GPIO_Pin_12 // PE.12
  #define SWITCHES_GPIO_REG_R_H         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_R_H         GPIO_Pin_7  // PG.07
  #define SWITCHES_GPIO_REG_R_L         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_R_L         GPIO_Pin_8  // PG.08
#endif

// ADC
#if defined(REV9E)
  #define ADC_AHB1Periph_GPIO           (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF)
#else
  #define ADC_AHB1Periph_GPIO           (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC)
#endif
#define ADC_GPIO_PIN_STICK_RV           GPIO_Pin_0  // PA.00
#define ADC_GPIO_PIN_STICK_RH           GPIO_Pin_1  // PA.01
#define ADC_GPIO_PIN_STICK_LH           GPIO_Pin_2  // PA.02
#define ADC_GPIO_PIN_STICK_LV           GPIO_Pin_3  // PA.03
#define ADC_GPIO_PIN_SLIDER1            GPIO_Pin_4  // PC.04
#define ADC_GPIO_PIN_SLIDER2            GPIO_Pin_5  // PC.05
#define ADC_GPIO_PIN_POT1               GPIO_Pin_6  // PA.06
#define ADC_GPIO_PIN_POT2               GPIO_Pin_0  // PB.00
#if defined(REV9E)
  #define ADC_GPIO_PIN_POT3             GPIO_Pin_10 // PF.10
  #define ADC_GPIO_PIN_POT4             GPIO_Pin_9  // PF.09
  #define ADC_GPIO_PIN_POT5             GPIO_Pin_8  // PF.08
  #define ADC_GPIO_PIN_POT6             GPIO_Pin_1  // PB.01
#elif defined(REVPLUS)
  #define ADC_GPIO_PIN_POT3             GPIO_Pin_1  // PB.01
#endif
#define ADC_GPIO_PIN_BATT               GPIO_Pin_0  // PC.00

// Power_OFF Delay and LED
#define PWR_RCC_AHB1Periph_GPIO         (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD)
#define PWR_GPIO                        GPIOD
#define PWR_GPIO_PIN_SWITCH             GPIO_Pin_1  // PD.01
#define PWR_GPIO_PIN_ON                 GPIO_Pin_0  // PD.00
#define PWR_GPIO_LED                    GPIOC
#define PWR_GPIO_PIN_LED                GPIO_Pin_6  // PC.06

// Internal Module
#define INTMODULE_RCC_AHB1Periph_GPIO   RCC_AHB1Periph_GPIOA
#if defined(REVPLUS)
  #define INTMODULE_GPIO_PWR            GPIOC
  #define INTMODULE_GPIO_PIN_PWR        GPIO_Pin_6
#else
  #define INTMODULE_GPIO_PWR            GPIOD
  #define INTMODULE_GPIO_PIN_PWR        GPIO_Pin_15
#endif
#define INTMODULE_GPIO_PIN              GPIO_Pin_10 // PA.10
#define INTMODULE_GPIO                  GPIOA
#define INTMODULE_GPIO_PinSource        GPIO_PinSource10

// External Module
#define EXTMODULE_RCC_AHB1Periph_GPIO   RCC_AHB1Periph_GPIOA
#define EXTMODULE_GPIO_PWR              GPIOD
#define EXTMODULE_GPIO_PIN_PWR          GPIO_Pin_8
#define EXTMODULE_GPIO_PIN              GPIO_Pin_7  // PA.07
#define EXTMODULE_GPIO                  GPIOA
#define EXTMODULE_GPIO_PinSource        GPIO_PinSource7

// Trainer Port
#define TRAINER_GPIO                    GPIOC
#define TRAINER_GPIO_PIN_IN             GPIO_Pin_8  // PC.08
#define TRAINER_GPIO_PIN_OUT            GPIO_Pin_9  // PC.09
#define TRAINER_GPIO_DETECT             GPIOA
#define TRAINER_GPIO_PIN_DETECT         GPIO_Pin_8  // PA.08

// SERIAL_USART
#define SERIAL_RCC_AHB1Periph_GPIO      RCC_AHB1Periph_GPIOB
#define SERIAL_RCC_APB1Periph_USART     RCC_APB1Periph_USART3
#define SERIAL_GPIO                     GPIOB
#define SERIAL_GPIO_PIN_TX              GPIO_Pin_10 // PB.10
#define SERIAL_GPIO_PIN_RX              GPIO_Pin_11 // PB.11
#define SERIAL_GPIO_PinSource_TX        GPIO_PinSource10
#define SERIAL_GPIO_PinSource_RX        GPIO_PinSource11
#define SERIAL_GPIO_AF                  GPIO_AF_USART3
#define SERIAL_USART                    USART3

// Telemetry
#define TELEMETRY_RCC_AHB1Periph_GPIO   RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOD
#define TELEMETRY_RCC_APB1Periph_USART  RCC_APB1Periph_USART2
#define TELEMETRY_GPIO_DIR              GPIOA
#define TELEMETRY_GPIO_PIN_DIR          GPIO_Pin_15 // PA.15
#define TELEMETRY_GPIO                  GPIOD
#define TELEMETRY_GPIO_PIN_TX           GPIO_Pin_5  // PD.05
#define TELEMETRY_GPIO_PIN_RX           GPIO_Pin_6  // PD.06
#define TELEMETRY_GPIO_PinSource_TX     GPIO_PinSource5
#define TELEMETRY_GPIO_PinSource_RX     GPIO_PinSource6
#define TELEMETRY_GPIO_AF               GPIO_AF_USART2
#define TELEMETRY_USART                 USART2
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

// BackLight
#if defined(REV9E)
  #define BACKLIGHT_TIMER               TIM9
  #define BACKLIGHT_GPIO                GPIOE
  #define BACKLIGHT_RCC_AHB1Periph_GPIO RCC_AHB1Periph_GPIOE
  #define BACKLIGHT_GPIO_PIN_1          GPIO_Pin_6 // PE.06
  #define BACKLIGHT_GPIO_PIN_2          GPIO_Pin_5 // PE.05
  #define BACKLIGHT_GPIO_PinSource_1    GPIO_PinSource6
  #define BACKLIGHT_GPIO_PinSource_2    GPIO_PinSource5
  #define BACKLIGHT_GPIO_AF_1           GPIO_AF_TIM9
  #define BACKLIGHT_GPIO_AF_2           GPIO_AF_TIM9
#elif defined (REVPLUS)
  #define BACKLIGHT_TIMER               TIM4
  #define BACKLIGHT_GPIO                GPIOD
  #define BACKLIGHT_RCC_AHB1Periph_GPIO RCC_AHB1Periph_GPIOD
  #define BACKLIGHT_GPIO_AF_1           GPIO_AF_TIM4
  #define BACKLIGHT_GPIO_PIN_1          GPIO_Pin_15 // PD.15
  #define BACKLIGHT_GPIO_PinSource_1    GPIO_PinSource15
  #define BACKLIGHT_GPIO_AF_2           GPIO_AF_TIM4
  #define BACKLIGHT_GPIO_PIN_2          GPIO_Pin_13 // PD.13
  #define BACKLIGHT_GPIO_PinSource_2    GPIO_PinSource13
#else
  #define BACKLIGHT_RCC_AHB1Periph_GPIO RCC_AHB1Periph_GPIOB
  #define BACKLIGHT_GPIO_PIN_1          GPIO_Pin_8  // PB.08
  #define BACKLIGHT_GPIO_AF_1           GPIO_AF_TIM10
  #define BACKLIGHT_GPIO                GPIOB
  #define BACKLIGHT_GPIO_PinSource_1    GPIO_PinSource8
#endif

// LCD
#if defined(REV9E)
  #define LCD_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD)
  #define LCD_RCC_AHB1Periph_RST        RCC_AHB1Periph_GPIOD
  #define LCD_RCC_AHB1Periph_NCS        RCC_AHB1Periph_GPIOA
  #define LCD_GPIO_SPI                  GPIOC
  #define LCD_GPIO_NCS                  GPIOA
  #define LCD_GPIO_RST                  GPIOD
  #define LCD_GPIO_PIN_MOSI             GPIO_Pin_12 // PC.12
  #define LCD_GPIO_PIN_CLK              GPIO_Pin_10 // PC.10
  #define LCD_GPIO_PIN_NCS              GPIO_Pin_15 // PA.15
  #define LCD_GPIO_PIN_A0               GPIO_Pin_11 // PC.11
  #define LCD_GPIO_PIN_RST              GPIO_Pin_15 // PD.15
#elif defined(REVPLUS)
  #define LCD_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD)
  #define LCD_RCC_AHB1Periph_RST        RCC_AHB1Periph_GPIOD
  #define LCD_RCC_AHB1Periph_NCS        RCC_AHB1Periph_GPIOA
  #define LCD_GPIO_SPI                  GPIOC
  #define LCD_GPIO_NCS                  GPIOA
  #define LCD_GPIO_RST                  GPIOD
  #define LCD_GPIO_PIN_MOSI             GPIO_Pin_12 // PC.12
  #define LCD_GPIO_PIN_CLK              GPIO_Pin_10 // PC.10
  #define LCD_GPIO_PIN_NCS              GPIO_Pin_15 // PA.15
  #define LCD_GPIO_PIN_A0               GPIO_Pin_11 // PC.11
  #define LCD_GPIO_PIN_RST              GPIO_Pin_12 // PD.12
#else
  #define LCD_RCC_AHB1Periph            RCC_AHB1Periph_GPIOD
  #define LCD_GPIO_SPI                  GPIOD
  #define LCD_GPIO_NCS                  GPIOD
  #define LCD_GPIO_RST                  GPIOD
  #define LCD_GPIO_PIN_MOSI             GPIO_Pin_10 // PD.10
  #define LCD_GPIO_PIN_CLK              GPIO_Pin_11 // PD.11
  #define LCD_GPIO_PIN_NCS              GPIO_Pin_14 // PD.14
  #define LCD_GPIO_PIN_A0               GPIO_Pin_13 // PD.13
  #define LCD_GPIO_PIN_RST              GPIO_Pin_12 // PD.12
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
#if defined(REV4a)
  #define I2C_SPEED                     100000
#else
  #define I2C_SPEED                     400000
#endif
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
  #define HAPTIC_RCC_AHB1Periph_GPIO    RCC_AHB1Periph_GPIOB
  #define HAPTIC_GPIO_PinSource         GPIO_PinSource8
  #define HAPTIC_GPIO                   GPIOB
  #define HAPTIC_GPIO_PIN               GPIO_Pin_8
#else
  #define HAPTIC_RCC_AHB1Periph_GPIO    RCC_AHB1Periph_GPIOC
  #define HAPTIC_GPIO                   GPIOC
  #define HAPTIC_GPIO_PIN               GPIO_Pin_12
#endif

// Top LCD on X9E
#if defined(REV9E)
  #define TOPLCD_GPIO                   GPIOG
  #define TOPLCD_GPIO_PIN_DATA          GPIO_Pin_5
  #define TOPLCD_GPIO_PIN_WR            GPIO_Pin_4
  #define TOPLCD_GPIO_PIN_CS1           GPIO_Pin_3
  #define TOPLCD_GPIO_PIN_CS2           GPIO_Pin_15
  #define TOPLCD_GPIO_PIN_BL            GPIO_Pin_2
  #define TOPLCD_RCC_AHB1Periph_GPIO    RCC_AHB1Periph_GPIOG
#endif

#endif
