#ifndef _HAL_
#define _HAL_

/*
 * GPIOS
 */

#define RCC_AHB1Periph_GPIOBUTTON       (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)

// Keys
#define	PIN_BUTTON_PLUS       GPIO_Pin_10	//SW3 PE.10
#define PIN_BUTTON_MINUS      GPIO_Pin_11 //SW1 PE.12
#define	PIN_BUTTON_ENTER      GPIO_Pin_12	//SW2 PE.11
#define PIN_BUTTON_MENU       GPIO_Pin_7	//SW4 PD.07
#define PIN_BUTTON_PAGE       GPIO_Pin_3  //SW6 PD.02
#define PIN_BUTTON_EXIT       GPIO_Pin_2  //SW5 PD.03

// Trims
#define	PIN_TRIM_RV_DN        GPIO_Pin_3	//PC.03
#define	PIN_TRIM_RV_UP        GPIO_Pin_2	//PC.02
#define	PIN_TRIM_RH_L         GPIO_Pin_1	//PC.01
#define PIN_TRIM_RH_R         GPIO_Pin_13	//PC.13
#define	PIN_TRIM_LH_R         GPIO_Pin_4	//PE.04
#define PIN_TRIM_LH_L         GPIO_Pin_3  //PE.03
#define PIN_TRIM_LV_DN        GPIO_Pin_6  //PE.06
#define PIN_TRIM_LV_UP        GPIO_Pin_5  //PE.05

// Switchs
#define	PIN_SW_F		          GPIO_Pin_14	//PE.14
#define	PIN_SW_E_L		        GPIO_Pin_3	//PB.03
#define	PIN_SW_E_H		        GPIO_Pin_4	//PB.04
#define	PIN_SW_B_H		        GPIO_Pin_5	//PB.05
#define	PIN_SW_B_L		        GPIO_Pin_6	//PB.06
#define	PIN_SW_A_H		        GPIO_Pin_7	//PB.07
#define	PIN_SW_A_L		        GPIO_Pin_2	//PE.02
#define	PIN_SW_G_L		        GPIO_Pin_0	//PB.00
#define	PIN_SW_G_H		        GPIO_Pin_5	//PA.05
#define	PIN_SW_C_L		        GPIO_Pin_7	//PE.07
#define	PIN_SW_C_H		        GPIO_Pin_1	//PB.01
#define	PIN_SW_D_H		        GPIO_Pin_9	//PE.09
#define	PIN_SW_D_L		        GPIO_Pin_8	//PE.08
#define	PIN_SW_H		          GPIO_Pin_13	//PE.13

// ADC
#define PIN_STK_J1                      GPIO_Pin_0  //PA.00              
#define PIN_STK_J2                      GPIO_Pin_1  //PA.01
#define PIN_STK_J3                      GPIO_Pin_2  //PA.02
#define PIN_STK_J4                      GPIO_Pin_3  //PA.03
#define PIN_SLD_J1                      GPIO_Pin_4  //PC.04
#define PIN_SLD_J2                      GPIO_Pin_5  //PC.05
#define PIN_FLP_J1                      GPIO_Pin_6  //PA.06
#define PIN_FLP_J2                      GPIO_Pin_7  //PA.07
#define PIN_MVOLT                       GPIO_Pin_0  //PC.00  
#define RCC_AHB1Periph_GPIOADC          RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC

// DAC
#define PIN_AUDIO_DAC                   GPIO_Pin_4  //PA.04

// Power_OFF Delay and LED
#define PIN_PWR_LED                     GPIO_Pin_6  //PC.06
#define PIN_PWR_STATUS                  GPIO_Pin_1  //PD.01
#define PIN_MCU_PWR                     GPIO_Pin_0  //PD.00
#define RCC_AHB1Periph_GPIOPWR          RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD
#define GPIOPWRLED                      GPIOC
#define GPIOPWR                         GPIOD

// Smart-Port
#define PIN_SPORT_ON                    GPIO_Pin_4  //PD.04
#define PIN_SPORT_TX                    GPIO_Pin_5  //PD.05
#define PIN_SPORT_RX                    GPIO_Pin_6  //PD.06

// Trainer Port
#define PIN_TR_PPM_IN                   GPIO_Pin_5  //PC.08
#define PIN_TR_PPM_OUT                  GPIO_Pin_9  //PC.09

// TODO merge these constants
#define PIN_PER_3                       0x030
// #define PIN_OS2                         0x0000
#define PIN_OS25                        0x2000
// #define PIN_OS50                        0x4000
// #define PIN_OS100                       0x6000

// Cppm
#define RCC_AHB1Periph_GPIOCPPM         RCC_AHB1Periph_GPIOA
#define PIN_CPPM_OUT                    GPIO_Pin_8  //PA.08
#define GPIOCPPM                        GPIOA
#define GPIO_PinSource_CPPM             GPIO_PinSource8

// Heart Beat
#define PIN_HEART_BEAT                  GPIO_Pin_7  //PC.07

// Debug UART
#define RCC_AHB1Periph_GPIO_UART        RCC_AHB1Periph_GPIOB
#define RCC_APB1Periph_UART_DEBUG       RCC_APB1Periph_USART3
#define GPIO_UART                       GPIOB
#define GPIO_PIN_UART_TX                GPIO_Pin_10 //PB.10
#define GPIO_PIN_UART_RX                GPIO_Pin_11 //PB.11
#define GPIO_PinSource_UART_TX          GPIO_PinSource10
#define GPIO_PinSource_UART_RX          GPIO_PinSource11
#define GPIO_AF_UART                    GPIO_AF_USART3
#define UART_DEBUG                      USART3

// USB_OTG
#define PIN_FS_VBUS                     GPIO_Pin_9  //PA.09
#define PIN_FS_ID                       GPIO_Pin_10 //PA.10
#define PIN_FS_DM                       GPIO_Pin_11 //PA.11
#define PIN_FS_DP                       GPIO_Pin_12 //PA.12

// BackLight PB8
#define RCC_AHB1Periph_GPIOBL           RCC_AHB1Periph_GPIOB
#define GPIO_Pin_BL                     GPIO_Pin_8  //PB.08
#define GPIOBL                          GPIOB
#define GPIO_PinSource_BL               GPIO_PinSource8

// LCD GPIOD 10-14
#define RCC_AHB1Periph_LCD              RCC_AHB1Periph_GPIOD
#define GPIO_LCD                        GPIOD
#define PIN_LCD_MOSI                    GPIO_Pin_10 //PD.10
#define PIN_LCD_CLK                     GPIO_Pin_11 //PD.11
#define PIN_LCD_NCS                     GPIO_Pin_14 //PD.14
#define PIN_LCD_A0                      GPIO_Pin_13 //PD.13
#define PIN_LCD_RST                     //GPIO_Pin_12  //pd12 test //RESET occurs when powered up,but should delay before initialize

// Audio - I2S3
#define CODEC_I2S                       SPI3
#define CODEC_I2S_CLK                   RCC_APB1Periph_SPI3
#define CODEC_I2S_GPIO_AF               GPIO_AF_SPI3
#define CODEC_I2S_IRQ                   SPI3_IRQn
#define CODEC_I2S_GPIO_CLOCK            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC )//| RCC_AHB1Periph_GPIOE)
#define CODEC_I2S_WS_PIN                GPIO_Pin_15 //PA.15
#define CODEC_I2S_SCK_PIN               GPIO_Pin_10 //PC.10
#define CODEC_I2S_SD_PIN                GPIO_Pin_12 //PC.12
#define CODEC_I2S_MCK_PIN               GPIO_Pin_6  //DISABLED
#define CODEC_I2S_WS_PINSRC             GPIO_PinSource15
#define CODEC_I2S_SCK_PINSRC            GPIO_PinSource10
#define CODEC_I2S_SD_PINSRC             GPIO_PinSource12
#define CODEC_I2S_MCK_PINSRC            GPIO_PinSource6
#define CODEC_I2S_GPIO                  GPIOC
#define CODEC_I2S_MCK_GPIO              GPIOC
#define CODEC_I2S_WS_GPIO               GPIOA
#define CODEC_I2S_MUTE                  GPIO_Pin_11 //PC.11
//#define CODEC_I2S_FLT                  GPIO_Pin_  //IIR/FIR
//#define CODEC_I2S_MODE                 GPIO_Pin_  //Connect to GND=I2S_STANDARD

// Eeprom 5137
#define I2C_EE_GPIO                     GPIOE
#define I2C_EE_WP_GPIO                  GPIOB
#define I2C_EE_GPIO_CLK                 RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOE
#define I2C_EE_SCL                      GPIO_Pin_1  //PE1
#define I2C_EE_SDA                      GPIO_Pin_0  //PE0
#define I2C_EE_WP                       GPIO_Pin_9  //PB9

// SD - SPI2
#define SPI_SD                          SPI2
#define GPIO_AF_SD                      GPIO_AF_SPI2
#define RCC_AHB1Periph_GPIO_CS          RCC_AHB1Periph_GPIOB
#define GPIO_SPI_SD                     GPIOB
#define GPIO_Pin_SPI_SD_CS              GPIO_Pin_12 //PB.12
#define GPIO_Pin_SPI_SD_SCK             GPIO_Pin_13 //PB.13
#define GPIO_Pin_SPI_SD_MISO            GPIO_Pin_14 //PB.14
#define GPIO_Pin_SPI_SD_MOSI            GPIO_Pin_15 //PB.15
#define GPIO_PinSource_CS               GPIO_PinSource12
#define GPIO_PinSource_SCK              GPIO_PinSource13
#define GPIO_PinSource_MISO             GPIO_PinSource14
#define GPIO_PinSource_MOSI             GPIO_PinSource15
#define RCC_APBPeriphClockCmd_SPI_SD    RCC_APB1PeriphClockCmd
#define RCC_APBPeriph_SPI_SD            RCC_APB1Periph_SPI2
#define RCC_AHBxPeriph_GPIO_WP          RCC_AHB1Periph_GPIOD
#define RCC_AHBxPeriph_GPIO_CP          RCC_AHB1Periph_GPIOD
#define GPIO_Mode_WP                    GPIO_Mode_OUT//lock?
#define GPIO_Mode_CP                    GPIO_Mode_IN //
#define GPIO_Pin_WP                     GPIO_Pin_8  //PD.08
#define GPIO_Pin_CP                     GPIO_Pin_9  //PD.09
#define GPIO_CTL_SD                     GPIOD

// Audio----I2S3-----SPI3
#define CODEC_MCLK_DISABLED
#define CODEC_USE_INT
//#define CODEC_USE_DMA 
#define AudioFreq                       I2S_AudioFreq_44k 

// EEPROM and CAT5137
#define I2C_Speed                       200000
#define I2C_FLASH_PAGESIZE              64
#define I2C_EEPROM_ADDRESS              0xA2
#define I2C_CAT5137_ADDRESS             0x5C //0101110

// SD card----SPI2
// demo uses a command line option to define this (see Makefile):
// #define STM32_SD_USE_DMA
#ifdef STM32_SD_USE_DMA
// #warning "Information only: using DMA"
#pragma message "*** Using DMA ***"
#endif

/* set to 1 to provide a disk_ioctrl function even if not needed by the FatFs */
#define STM32_SD_DISK_IOCTRL_FORCE      (0)
#define CARD_SUPPLY_SWITCHABLE          (0) // power on not depend any IO
#define SOCKET_WP_CONNECTED             (0)
#define SOCKET_CP_CONNECTED             (0)

#define SPI_BaudRatePrescaler_SPI_SD    SPI_BaudRatePrescaler_2 // - for SPI2 and full-speed APB1: 30MHz/2 =15MHZ < 20MHZ

// Selectable
// DMA
#define DMA_Channel_SPI_SD_RX           DMA1_Channel2
#define DMA_Channel_SPI_SD_TX           DMA1_Channel3
#define DMA_FLAG_SPI_SD_TC_RX           DMA1_FLAG_TC2
#define DMA_FLAG_SPI_SD_TC_TX           DMA1_FLAG_TC3

#endif
