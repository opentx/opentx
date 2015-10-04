#ifndef _HAL_
#define _HAL_

// Keys
#define KEYS_RCC_AHB1Periph_GPIO        (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOJ)
#define KEYS_GPIO_REG_MENU              GPIOC->IDR
#define KEYS_GPIO_PIN_MENU              GPIO_Pin_13 // PC.13
#define KEYS_GPIO_REG_EXIT              GPIOI->IDR
#define KEYS_GPIO_PIN_EXIT              GPIO_Pin_8  // PI.08
#define KEYS_GPIO_REG_LEFT              GPIOI->IDR
#define KEYS_GPIO_PIN_LEFT              GPIO_Pin_7  // PI.07
#define KEYS_GPIO_REG_ENTER             GPIOC->IDR
#define KEYS_GPIO_PIN_ENTER             GPIO_Pin_1  // PC.01
#define KEYS_GPIO_REG_UP                GPIOG->IDR
#define KEYS_GPIO_PIN_UP                GPIO_Pin_13 // PG.13
#define KEYS_GPIO_REG_DOWN              GPIOI->IDR
#define KEYS_GPIO_PIN_DOWN              GPIO_Pin_6  // PI.06
#define KEYS_GPIO_REG_RIGHT             GPIOC->IDR
#define KEYS_GPIO_PIN_RIGHT             GPIO_Pin_4  // PC.04

// Rotary Encoder
#define ENC_GPIO                        GPIOH
#define ENC_GPIO_PIN_A                  GPIO_Pin_11 // PH.11
#define ENC_GPIO_PIN_B                  GPIO_Pin_10 // PH.10

// Switches
#define SWITCHES_GPIO_REG_A_H           GPIOI->IDR
#define SWITCHES_GPIO_PIN_A_H           GPIO_Pin_15 // PI.15
#define SWITCHES_GPIO_REG_A_L           GPIOH->IDR
#define SWITCHES_GPIO_PIN_A_L           GPIO_Pin_9  // PH.09
#define SWITCHES_GPIO_REG_B_H           GPIOH->IDR
#define SWITCHES_GPIO_PIN_B_H           GPIO_Pin_12 // PH.12
#define SWITCHES_GPIO_REG_B_L           GPIOB->IDR
#define SWITCHES_GPIO_PIN_B_L           GPIO_Pin_12 // PB.12
#define SWITCHES_GPIO_REG_C_H           GPIOB->IDR
#define SWITCHES_GPIO_PIN_C_H           GPIO_Pin_15 // PB.15
#define SWITCHES_GPIO_REG_C_L           GPIOD->IDR
#define SWITCHES_GPIO_PIN_C_L           GPIO_Pin_11 // PD.11
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
#define SWITCHES_GPIO_PIN_G_H           GPIO_Pin_3  // PG.03
#define SWITCHES_GPIO_REG_G_L           GPIOG->IDR
#define SWITCHES_GPIO_PIN_G_L           GPIO_Pin_6  // PG.06
#define SWITCHES_GPIO_REG_H             GPIOG->IDR
#define SWITCHES_GPIO_PIN_H             GPIO_Pin_7  // PG.07

// Trims
#define TRIMS_GPIO_REG_LHL              GPIOC->IDR
#define TRIMS_GPIO_PIN_LHL              GPIO_Pin_0  // PC.00
#define TRIMS_GPIO_REG_LHR              GPIOI->IDR
#define TRIMS_GPIO_PIN_LHR              GPIO_Pin_4  // PI.04
#define TRIMS_GPIO_REG_LVD              GPIOG->IDR
#define TRIMS_GPIO_PIN_LVD              GPIO_Pin_12 // PG.12
#define TRIMS_GPIO_REG_LVU              GPIOJ->IDR
#define TRIMS_GPIO_PIN_LVU              GPIO_Pin_14 // PJ.14
#define TRIMS_GPIO_REG_RVD              GPIOJ->IDR
#define TRIMS_GPIO_PIN_RVD              GPIO_Pin_13 // PJ.13
#define TRIMS_GPIO_REG_RHL              GPIOD->IDR
#define TRIMS_GPIO_PIN_RHL              GPIO_Pin_3  // PD.03
#define TRIMS_GPIO_REG_RVU              GPIOJ->IDR
#define TRIMS_GPIO_PIN_RVU              GPIO_Pin_12 // PJ.12
#define TRIMS_GPIO_REG_RHR              GPIOD->IDR
#define TRIMS_GPIO_PIN_RHR              GPIO_Pin_7  // PD.07
#define TRIMS_GPIO_REG_RSD              GPIOJ->IDR
#define TRIMS_GPIO_PIN_RSD              GPIO_Pin_8  // PJ.08
#define TRIMS_GPIO_REG_RSU              GPIOD->IDR
#define TRIMS_GPIO_PIN_RSU              GPIO_Pin_13 // PD.13
#define TRIMS_GPIO_REG_LSD              GPIOB->IDR
#define TRIMS_GPIO_PIN_LSD              GPIO_Pin_14 // PB.14
#define TRIMS_GPIO_REG_LSU              GPIOB->IDR
#define TRIMS_GPIO_PIN_LSU              GPIO_Pin_13 // PB.13

// Index of all keys
#define KEYS_GPIOB_PINS                 (SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_H | TRIMS_GPIO_PIN_LSD | TRIMS_GPIO_PIN_LSU)
#define KEYS_GPIOC_PINS                 (KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_ENTER | KEYS_GPIO_PIN_RIGHT | TRIMS_GPIO_PIN_LHL)
#define KEYS_GPIOD_PINS                 (SWITCHES_GPIO_PIN_C_L | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR | TRIMS_GPIO_PIN_RSU)
#define KEYS_GPIOE_PINS                 (SWITCHES_GPIO_PIN_E_L)
#define KEYS_GPIOG_PINS                 (KEYS_GPIO_PIN_UP | SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_H | TRIMS_GPIO_PIN_LVD)
#define KEYS_GPIOH_PINS                 (SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_F | ENC_GPIO_PIN_A | ENC_GPIO_PIN_B)
#define KEYS_GPIOI_PINS                 (KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_LEFT | KEYS_GPIO_PIN_DOWN | SWITCHES_GPIO_PIN_A_H | TRIMS_GPIO_PIN_LHR)
#define KEYS_GPIOJ_PINS                 (SWITCHES_GPIO_PIN_D_H | TRIMS_GPIO_PIN_LVU | TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_RSD)

// ADC
#define ADC_RCC_AHB1Periph_GPIO         (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC)
#define ADC_GPIO_STICKS                 GPIOA
#define ADC_GPIO_PIN_STICK_LV           GPIO_Pin_0  // PA.00
#define ADC_GPIO_PIN_STICK_LH           GPIO_Pin_1  // PA.01
#define ADC_GPIO_PIN_STICK_RV           GPIO_Pin_2  // PA.02
#define ADC_GPIO_PIN_STICK_RH           GPIO_Pin_3  // PA.03
#define ADC_GPIO_POT_L1                 GPIOC
#define ADC_GPIO_PIN_POT_L1             GPIO_Pin_3  // PC.03
#define ADC_GPIO_POT_R1                 GPIOA
#define ADC_GPIO_PIN_POT_R1             GPIO_Pin_5  // PA.05
#define ADC_GPIO_POT_L2                 GPIOB
#define ADC_GPIO_PIN_POT_L2             GPIO_Pin_1  // PB.01
#define ADC_GPIO_POT_R2                 GPIOB
#define ADC_GPIO_PIN_POT_R2             GPIO_Pin_0  // PB.00
#define ADC_GPIO_SWITCHES_L3            GPIOC
#define ADC_GPIO_SWITCHES_PIN_L3        GPIO_Pin_1  // PC.01
#define ADC_GPIO_SWITCHES_R3            GPIOA
#define ADC_GPIO_SWITCHES_PIN_R3        GPIO_Pin_7  // PA.07
#define ADC_GPIO_SWITCHES_L4            GPIOC
#define ADC_GPIO_SWITCHES_PIN_L4        GPIO_Pin_0  // PC.00
#define ADC_GPIO_SWITCHES_R4            GPIOA
#define ADC_GPIO_SWITCHES_PIN_R4        GPIO_Pin_6  // PA.06
#define ADC_GPIO_BATT                   GPIOC
#define ADC_GPIO_PIN_BATT               GPIO_Pin_4  // PC.04

// Power
#define PWR_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOJ
#define PWR_GPIO                        GPIOJ
#define PWR_GPIO_PIN_ON                 GPIO_Pin_0  // PJ.00
#define PWR_GPIO_PIN_SWITCH             GPIO_Pin_1  // PJ.01

// Led
#define LED_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOI
#define LED_GPIO                        GPIOI
#define LED_GPIO_PIN                    GPIO_Pin_5  // PI.05

// Serial Port (DEBUG)
#define SERIAL_RCC_AHB1Periph_GPIO      RCC_AHB1Periph_GPIOB
#define SERIAL_RCC_APB1Periph_USART     RCC_APB1Periph_USART3
#define SERIAL_GPIO                     GPIOB
#define SERIAL_GPIO_PIN_TX              GPIO_Pin_10 // PB.10
#define SERIAL_GPIO_PIN_RX              GPIO_Pin_11 // PB.11
#define SERIAL_GPIO_PinSource_TX        GPIO_PinSource10
#define SERIAL_GPIO_PinSource_RX        GPIO_PinSource11
#define SERIAL_GPIO_AF                  GPIO_AF_USART3
#define SERIAL_USART                    USART3
#define SERIAL_USART_IRQHandler         USART3_IRQHandler
#define SERIAL_USART_IRQn               USART3_IRQn

// Telemetry
#define TELEMETRY_RCC_AHB1Periph_GPIO   RCC_AHB1Periph_GPIOD
#define TELEMETRY_RCC_APB1Periph_USART  RCC_APB1Periph_USART2
#define TELEMETRY_GPIO_DIR              GPIOD
#define TELEMETRY_GPIO_PIN_DIR          GPIO_Pin_4  // PD.04
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

// LCD
#define LCD_RCC_AHB1Periph              (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOJ | RCC_AHB1Periph_GPIOK | RCC_AHB1Periph_DMA2D)
#define LCD_RCC_APB2Periph              (RCC_APB2Periph_TIM8 | RCC_APB2Periph_LTDC)
#define LCD_GPIO_NRST                   GPIOF
#define LCD_GPIO_PIN_NRST               GPIO_Pin_10 // PF.10
#define LCD_GPIO_BL                     GPIOA
#define LCD_GPIO_PIN_BL                 GPIO_Pin_5  // PA.05
#define LCD_GPIO_PinSource_BL           GPIO_PinSource5
#define LCD_GPIO_AF_BL                  GPIO_AF_TIM8
#define LTDC_IRQ_PRIO                   4
#define DMA_SCREEN_IRQ_PRIO             6

// SD
#define SD_RCC_AHB1Periph               (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2)
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

// Audio

// I2C Bus: CAT5137
#define I2C_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOB
#define I2C_RCC_APB1Periph_I2C          RCC_APB1Periph_I2C1
#define I2C                             I2C1
#define I2C_GPIO                        GPIOB
#define I2C_GPIO_PIN_SCL                GPIO_Pin_8  // PB.08
#define I2C_GPIO_PIN_SDA                GPIO_Pin_9  // PB.09
#define I2C_GPIO_AF                     GPIO_AF_I2C1
#define I2C_GPIO_PinSource_SCL          GPIO_PinSource8
#define I2C_GPIO_PinSource_SDA          GPIO_PinSource9
#define I2C_SPEED                       400000
#define I2C_ADDRESS_CAT5137             0x5C

// Haptic
#define HAPTIC_RCC_AHB1Periph_GPIO      RCC_AHB1Periph_GPIOA
#define HAPTIC_GPIO                     GPIOA
#define HAPTIC_GPIO_PIN                 GPIO_Pin_2
#define HAPTIC_RCC_APB2Periph_TIMER     RCC_APB2ENR_TIM11EN
#define HAPTIC_GPIO_TIMER               TIM11
#define HAPTIC_GPIO_AF                  GPIO_AF_TIM11
#define HAPTIC_GPIO_PinSource           GPIO_PinSource2

// Internal Module
#define INTMODULE_RCC_AHB1Periph_GPIO   (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB)
#define INTMODULE_GPIO_PWR              GPIOA
#define INTMODULE_GPIO_PIN_PWR          GPIO_Pin_8   // PA.08
#define INTMODULE_GPIO                  GPIOB
#define INTMODULE_GPIO_PIN              GPIO_Pin_6   // PB.06
#define INTMODULE_GPIO_PinSource        GPIO_PinSource6

// External Module
#define EXTMODULE_RCC_AHB1Periph_GPIO   (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB)
#define EXTMODULE_GPIO_PWR              GPIOB
#define EXTMODULE_GPIO_PIN_PWR          GPIO_Pin_3   // PB.03
#define EXTMODULE_GPIO                  GPIOA
#define EXTMODULE_GPIO_PIN              GPIO_Pin_15  // PA.15
#define EXTMODULE_GPIO_PinSource        GPIO_PinSource15

// Trainer Port
#define TRAINER_RCC_AHB1Periph          (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC)
#define TRAINER_RCC_APB1Periph          RCC_APB1Periph_TIM3
#define TRAINER_GPIO                    GPIOC
#define TRAINER_GPIO_PIN_IN             GPIO_Pin_6  // PC.06
#define TRAINER_GPIO_PIN_OUT            GPIO_Pin_7  // PC.07
#define TRAINER_GPIO_DETECT             GPIOB
#define TRAINER_GPIO_PIN_DETECT         GPIO_Pin_4  // PB.04
#define TRAINER_TIMER                   TIM3
#define TRAINER_TIMER_IRQn              TIM3_IRQn
#define TRAINER_GPIO_PinSource_IN       GPIO_PinSource6
#define TRAINER_GPIO_AF                 GPIO_AF_TIM3

// 5ms Interrupt
#define INTERRUPT_5MS_APB1Periph        RCC_APB1Periph_TIM14
#define INTERRUPT_5MS_TIMER             TIM14
#define INTERRUPT_5MS_IRQn              TIM8_TRG_COM_TIM14_IRQn
#define INTERRUPT_5MS_IRQHandler        TIM8_TRG_COM_TIM14_IRQHandler

// 2MHz Timer
#define TIMER_2MHz_APB1Periph           RCC_APB1Periph_TIM7
#define TIMER_2MHz_TIMER                TIM7

#endif
