/**
  ******************************************************************************
  * @file    stm32100e_eval.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains definitions for STM32100E_EVAL's Leds, push-buttons
  *          COM ports, sFLASH (on SPI) and Temperature Sensor LM75 (on I2C)
  *          hardware resources.  
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32100E_EVAL_H
#define __STM32100E_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_EVAL
  * @{
  */  
  
/** @addtogroup STM32100E_EVAL
  * @{
  */ 

/** @addtogroup STM32100E_EVAL_LOW_LEVEL
  * @{
  */ 
  
/** @defgroup STM32100E_EVAL_LOW_LEVEL_Exported_Types
  * @{
  */
/**
  * @}
  */ 

/** @defgroup STM32100E_EVAL_LOW_LEVEL_Exported_Constants
  * @{
  */ 
/** @addtogroup STM32100E_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_Pin_6
#define LED1_GPIO_PORT                   GPIOF
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOF  
  
#define LED2_PIN                         GPIO_Pin_7
#define LED2_GPIO_PORT                   GPIOF
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOF  

#define LED3_PIN                         GPIO_Pin_8  
#define LED3_GPIO_PORT                   GPIOF
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOF  

#define LED4_PIN                         GPIO_Pin_9
#define LED4_GPIO_PORT                   GPIOF
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOF

/**
  * @}
  */
  
/** @addtogroup STM32100E_EVAL_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          8

/**
 * @brief Wakeup push-button
 */
#define WAKEUP_BUTTON_PIN                GPIO_Pin_0
#define WAKEUP_BUTTON_GPIO_PORT          GPIOA
#define WAKEUP_BUTTON_GPIO_CLK           RCC_APB2Periph_GPIOA
#define WAKEUP_BUTTON_EXTI_LINE          EXTI_Line0
#define WAKEUP_BUTTON_EXTI_PORT_SOURCE   GPIO_PortSourceGPIOA
#define WAKEUP_BUTTON_EXTI_PIN_SOURCE    GPIO_PinSource0
#define WAKEUP_BUTTON_EXTI_IRQn          EXTI0_IRQn 
/**
 * @brief Tamper push-button
 */
#define TAMPER_BUTTON_PIN                GPIO_Pin_13
#define TAMPER_BUTTON_GPIO_PORT          GPIOC
#define TAMPER_BUTTON_GPIO_CLK           RCC_APB2Periph_GPIOC
#define TAMPER_BUTTON_EXTI_LINE          EXTI_Line13
#define TAMPER_BUTTON_EXTI_PORT_SOURCE   GPIO_PortSourceGPIOC
#define TAMPER_BUTTON_EXTI_PIN_SOURCE    GPIO_PinSource13
#define TAMPER_BUTTON_EXTI_IRQn          EXTI15_10_IRQn 
/**
 * @brief Key push-button
 */
#define KEY_BUTTON_PIN                   GPIO_Pin_8
#define KEY_BUTTON_GPIO_PORT             GPIOG
#define KEY_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOG
#define KEY_BUTTON_EXTI_LINE             EXTI_Line8
#define KEY_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOG
#define KEY_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource8
#define KEY_BUTTON_EXTI_IRQn             EXTI9_5_IRQn
/**
 * @brief Joystick Right push-button
 */
#define RIGHT_BUTTON_PIN                 GPIO_Pin_13
#define RIGHT_BUTTON_GPIO_PORT           GPIOG
#define RIGHT_BUTTON_GPIO_CLK            RCC_APB2Periph_GPIOG
#define RIGHT_BUTTON_EXTI_LINE           EXTI_Line13
#define RIGHT_BUTTON_EXTI_PORT_SOURCE    GPIO_PortSourceGPIOG
#define RIGHT_BUTTON_EXTI_PIN_SOURCE     GPIO_PinSource13
#define RIGHT_BUTTON_EXTI_IRQn           EXTI15_10_IRQn
/**
 * @brief Joystick Left push-button
 */    
#define LEFT_BUTTON_PIN                  GPIO_Pin_14
#define LEFT_BUTTON_GPIO_PORT            GPIOG
#define LEFT_BUTTON_GPIO_CLK             RCC_APB2Periph_GPIOG
#define LEFT_BUTTON_EXTI_LINE            EXTI_Line14
#define LEFT_BUTTON_EXTI_PORT_SOURCE     GPIO_PortSourceGPIOG
#define LEFT_BUTTON_EXTI_PIN_SOURCE      GPIO_PinSource14
#define LEFT_BUTTON_EXTI_IRQn            EXTI15_10_IRQn  
/**
 * @brief Joystick Up push-button
 */
#define UP_BUTTON_PIN                    GPIO_Pin_15
#define UP_BUTTON_GPIO_PORT              GPIOG
#define UP_BUTTON_GPIO_CLK               RCC_APB2Periph_GPIOG
#define UP_BUTTON_EXTI_LINE              EXTI_Line15
#define UP_BUTTON_EXTI_PORT_SOURCE       GPIO_PortSourceGPIOG
#define UP_BUTTON_EXTI_PIN_SOURCE        GPIO_PinSource15
#define UP_BUTTON_EXTI_IRQn              EXTI15_10_IRQn  
/**
 * @brief Joystick Down push-button
 */   
#define DOWN_BUTTON_PIN                  GPIO_Pin_11
#define DOWN_BUTTON_GPIO_PORT            GPIOG
#define DOWN_BUTTON_GPIO_CLK             RCC_APB2Periph_GPIOG
#define DOWN_BUTTON_EXTI_LINE            EXTI_Line11
#define DOWN_BUTTON_EXTI_PORT_SOURCE     GPIO_PortSourceGPIOG
#define DOWN_BUTTON_EXTI_PIN_SOURCE      GPIO_PinSource11
#define DOWN_BUTTON_EXTI_IRQn            EXTI15_10_IRQn  
/**
 * @brief Joystick Sel push-button
 */  
#define SEL_BUTTON_PIN                   GPIO_Pin_7
#define SEL_BUTTON_GPIO_PORT             GPIOG
#define SEL_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOG
#define SEL_BUTTON_EXTI_LINE             EXTI_Line7
#define SEL_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOG
#define SEL_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource7
#define SEL_BUTTON_EXTI_IRQn             EXTI9_5_IRQn          
/**
  * @}
  */ 

/** @addtogroup STM32100E_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             2

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_IRQn                   USART2_IRQn

/**
  * @}
  */ 

/** @addtogroup STM32100E_EVAL_LOW_LEVEL_SD_FLASH
  * @{
  */
/**
  * @brief  SD SPI Interface pins
  */
#define SD_SPI                           SPI2
#define SD_SPI_CLK                       RCC_APB1Periph_SPI2
#define SD_SPI_SCK_PIN                   GPIO_Pin_13                 /* PC.13 */
#define SD_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOB */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
#define SD_SPI_MISO_PIN                  GPIO_Pin_14                 /* PC.14 */
#define SD_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SD_SPI_MOSI_PIN                  GPIO_Pin_15                 /* PB.15 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SD_CS_PIN                        GPIO_Pin_6                  /* PG.06 */
#define SD_CS_GPIO_PORT                  GPIOG                       /* GPIOG */
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOG
#define SD_DETECT_PIN                    GPIO_Pin_11                 /* PF.11 */
#define SD_DETECT_GPIO_PORT              GPIOF                       /* GPIOF */
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOF

/**
  * @}
  */

/** @addtogroup STM3210C_EVAL_LOW_LEVEL_I2C_EE
  * @{
  */
/**
  * @brief  I2C EEPROM Interface pins
  */  
#define sEE_I2C                          I2C2
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C2
#define sEE_I2C_SCL_PIN                  GPIO_Pin_10                 /* PB.10 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sEE_I2C_SDA_PIN                  GPIO_Pin_11                  /* PB.11 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sEE_M24C64_32

#define sEE_I2C_DMA                      DMA1   
#define sEE_I2C_DMA_CHANNEL_TX           DMA1_Channel4
#define sEE_I2C_DMA_CHANNEL_RX           DMA1_Channel5 
#define sEE_I2C_DMA_FLAG_TX_TC           DMA1_IT_TC4   
#define sEE_I2C_DMA_FLAG_TX_GL           DMA1_IT_GL4 
#define sEE_I2C_DMA_FLAG_RX_TC           DMA1_IT_TC5 
#define sEE_I2C_DMA_FLAG_RX_GL           DMA1_IT_GL5    
#define sEE_I2C_DMA_CLK                  RCC_AHBPeriph_DMA1
#define sEE_I2C_DR_Address               ((uint32_t)0x40005810)
#define sEE_USE_DMA
   
#define sEE_I2C_DMA_TX_IRQn              DMA1_Channel4_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Channel5_IRQn
#define sEE_I2C_DMA_TX_IRQHandler        DMA1_Channel4_IRQHandler
#define sEE_I2C_DMA_RX_IRQHandler        DMA1_Channel5_IRQHandler  
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0
 
#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1 

/* Time constant for the delay caclulation allowing to have a millisecond 
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 24MHz then sEE_TIME_CONST should be 24. */
#define sEE_TIME_CONST          24 

/**
  * @}
  */ 
  
/** @addtogroup STM32100E_EVAL_LOW_LEVEL_M25P_FLASH_SPI
  * @{
  */
/**
  * @brief  M25P FLASH SPI Interface pins
  */  
#define sFLASH_SPI                       SPI1
#define sFLASH_SPI_CLK                   RCC_APB2Periph_SPI1
#define sFLASH_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define sFLASH_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define sFLASH_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.06 */
#define sFLASH_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define sFLASH_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.07 */
#define sFLASH_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define sFLASH_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sFLASH_CS_PIN                    GPIO_Pin_6                  /* PE.06 */
#define sFLASH_CS_GPIO_PORT              GPIOE                       /* GPIOE */
#define sFLASH_CS_GPIO_CLK               RCC_APB2Periph_GPIOE

/**
  * @}
  */


/** @addtogroup STM32100E_EVAL_LOW_LEVEL_TSENSOR_I2C
  * @{
  */
/**
  * @brief  LM75 Temperature Sensor I2C Interface pins
  */  
#define LM75_I2C                         I2C2
#define LM75_I2C_CLK                     RCC_APB1Periph_I2C2
#define LM75_I2C_SCL_PIN                 GPIO_Pin_10                 /* PB.10 */
#define LM75_I2C_SCL_GPIO_PORT           GPIOB                       /* GPIOB */
#define LM75_I2C_SCL_GPIO_CLK            RCC_APB2Periph_GPIOB
#define LM75_I2C_SDA_PIN                 GPIO_Pin_11                 /* PB.11 */
#define LM75_I2C_SDA_GPIO_PORT           GPIOB                       /* GPIOB */
#define LM75_I2C_SDA_GPIO_CLK            RCC_APB2Periph_GPIOB
#define LM75_I2C_SMBUSALERT_PIN          GPIO_Pin_12                 /* PB.12 */
#define LM75_I2C_SMBUSALERT_GPIO_PORT    GPIOB                       /* GPIOB */
#define LM75_I2C_SMBUSALERT_GPIO_CLK     RCC_APB2Periph_GPIOB
#define LM75_I2C_DR                      ((uint32_t)0x40005810)

#define LM75_DMA_CLK                     RCC_AHBPeriph_DMA1
#define LM75_DMA_TX_CHANNEL              DMA1_Channel4
#define LM75_DMA_RX_CHANNEL              DMA1_Channel5
#define LM75_DMA_TX_TCFLAG               DMA1_FLAG_TC4
#define LM75_DMA_RX_TCFLAG               DMA1_FLAG_TC5   
   
/**
  * @}
  */
  
/**
  * @}
  */
  
/** @defgroup STM32100E_EVAL_LOW_LEVEL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32100E_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */ 
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
void sEE_LowLevel_DeInit(void);
void sEE_LowLevel_Init(void);
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction); 
void sFLASH_LowLevel_DeInit(void);
void sFLASH_LowLevel_Init(void); 
void LM75_LowLevel_DeInit(void);
void LM75_LowLevel_Init(void); 
/**
  * @}
  */
#ifdef __cplusplus
}
#endif
  
#endif /* __STM32100E_EVAL_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */
  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
