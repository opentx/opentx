/**
  ******************************************************************************
  * @file    stm3210c_eval.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains definitions for STM3210C_EVAL's Leds, push-buttons
  *          COM ports, SD Card on SPI and sEE on I2C hardware resources.
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
  * <h2><center>&copy; COPYRIGHT 210 STMicroelectronics</center></h2>
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM3210C_EVAL_H
#define __STM3210C_EVAL_H

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
    
/** @addtogroup STM3210C_EVAL
  * @{
  */ 

/** @addtogroup STM3210C_EVAL_LOW_LEVEL
  * @{
  */ 
  
/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Types
  * @{
  */
/**
  * @}
  */ 

/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** @addtogroup STM3210C_EVAL_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             4

#define LED1_PIN                         GPIO_Pin_7
#define LED1_GPIO_PORT                   GPIOD
#define LED1_GPIO_CLK                    RCC_APB2Periph_GPIOD  
  
#define LED2_PIN                         GPIO_Pin_13
#define LED2_GPIO_PORT                   GPIOD
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOD  

#define LED3_PIN                         GPIO_Pin_3  
#define LED3_GPIO_PORT                   GPIOD
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOD  

#define LED4_PIN                         GPIO_Pin_4  
#define LED4_GPIO_PORT                   GPIOD
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOD  


/**
  * @}
  */ 
  
/** @addtogroup STM3210C_EVAL_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          3 /*!< Joystick pins are connected to 
                                                an IO Expander (accessible through 
                                                I2C1 interface) */

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
#define KEY_BUTTON_PIN                   GPIO_Pin_9
#define KEY_BUTTON_GPIO_PORT             GPIOB
#define KEY_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOB
#define KEY_BUTTON_EXTI_LINE             EXTI_Line9
#define KEY_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOB
#define KEY_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource9
#define KEY_BUTTON_EXTI_IRQn             EXTI9_5_IRQn
/**
  * @}
  */ 

/** @addtogroup STM3210C_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             1

/**
 * @brief Definition for COM port1, connected to USART2 (USART2 pins remapped on GPIOD)
 */ 
#define EVAL_COM1                        USART2
#define EVAL_COM1_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM1_TX_PIN                 GPIO_Pin_5
#define EVAL_COM1_TX_GPIO_PORT           GPIOD
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM1_RX_PIN                 GPIO_Pin_6
#define EVAL_COM1_RX_GPIO_PORT           GPIOD
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM1_IRQn                   USART2_IRQn

/**
  * @}
  */ 

/** @addtogroup STM3210B_EVAL_SD_SPI
  * @{
  */
/**
  * @brief  SD SPI Interface pins
  */
#define SD_SPI                           SPI3
#define SD_SPI_CLK                       RCC_APB1Periph_SPI3
#define SD_SPI_SCK_PIN                   GPIO_Pin_10                 /* PC.10 */
#define SD_SPI_SCK_GPIO_PORT             GPIOC                       /* GPIOC */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOC
#define SD_SPI_MISO_PIN                  GPIO_Pin_11                 /* PC.11 */
#define SD_SPI_MISO_GPIO_PORT            GPIOC                       /* GPIOC */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOC
#define SD_SPI_MOSI_PIN                  GPIO_Pin_12                 /* PC.12 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOC                       /* GPIOC */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOC
#define SD_CS_PIN                        GPIO_Pin_4                  /* PA.04 */
#define SD_CS_GPIO_PORT                  GPIOA                       /* GPIOA */
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOA
#define SD_DETECT_PIN                    GPIO_Pin_0                  /* PE.00 */
#define SD_DETECT_GPIO_PORT              GPIOE                       /* GPIOE */
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOE

/**
  * @}
  */

/** @addtogroup STM3210C_EVAL_LOW_LEVEL_I2C_EE
  * @{
  */
/**
  * @brief  I2C EEPROM Interface pins
  */  
#define sEE_I2C                          I2C1
#define sEE_I2C_CLK                      RCC_APB1Periph_I2C1
#define sEE_I2C_SCL_PIN                  GPIO_Pin_6                  /* PB.06 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sEE_I2C_SDA_PIN                  GPIO_Pin_7                  /* PB.07 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define sEE_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB
#define sEE_M24C64_32

#define sEE_I2C_DMA                      DMA1   
#define sEE_I2C_DMA_CHANNEL_TX           DMA1_Channel6
#define sEE_I2C_DMA_CHANNEL_RX           DMA1_Channel7 
#define sEE_I2C_DMA_FLAG_TX_TC           DMA1_IT_TC6   
#define sEE_I2C_DMA_FLAG_TX_GL           DMA1_IT_GL6 
#define sEE_I2C_DMA_FLAG_RX_TC           DMA1_IT_TC7 
#define sEE_I2C_DMA_FLAG_RX_GL           DMA1_IT_GL7    
#define sEE_I2C_DMA_CLK                  RCC_AHBPeriph_DMA1
#define sEE_I2C_DR_Address               ((uint32_t)0x40005410)
#define sEE_USE_DMA
   
#define sEE_I2C_DMA_TX_IRQn              DMA1_Channel6_IRQn
#define sEE_I2C_DMA_RX_IRQn              DMA1_Channel7_IRQn
#define sEE_I2C_DMA_TX_IRQHandler        DMA1_Channel6_IRQHandler
#define sEE_I2C_DMA_RX_IRQHandler        DMA1_Channel7_IRQHandler   
#define sEE_I2C_DMA_PREPRIO              0
#define sEE_I2C_DMA_SUBPRIO              0   
   
#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1   

/* Time constant for the delay caclulation allowing to have a millisecond 
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 72MHz then sEE_TIME_CONST should be 72. */
#define sEE_TIME_CONST                   72 
   
/**
  * @}
  */
   
/**
  * @}
  */ 
  
/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210C_EVAL_LOW_LEVEL_Exported_Functions
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
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM3210C_EVAL_H */
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
