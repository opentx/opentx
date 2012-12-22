/**
  ******************************************************************************
  * @file    stm32l152_eval_glass_lcd.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   Header file for stm32l152_eval_glass_lcd.c module.
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
#ifndef __STM32L152_EVAL_GLASS_LCD_H
#define __STM32L152_EVAL_GLASS_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM32L152_EVAL
  * @{
  */
  
/** @addtogroup STM32L152_EVAL_GLASS_LCD
  * @{
  */ 


/** @defgroup STM32L152_EVAL_GLASS_LCD_Exported_Types
  * @{
  */
typedef enum
{
  POINT_OFF = 0,
  POINT_ON = 1
}Point_Typedef;

typedef enum
{
  APOSTROPHE_OFF = 0,
  APOSTROPHE_ON = 1
}Apostrophe_Typedef;   
/**
  * @}
  */ 

/** @defgroup STM32L152_EVAL_GLASS_LCD_Exported_Constants
  * @{
  */ 

/**
  * @}
  */
  
/** @defgroup STM32L152_EVAL_GLASS_LCD_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32L152_EVAL_GLASS_LCD_Exported_Functions
  * @{
  */
void LCD_GLASS_Init(void);
void LCD_GLASS_DisplayChar(uint8_t* ch, Point_Typedef point, Apostrophe_Typedef apostrophe,uint8_t position);
void LCD_GLASS_DisplayString(uint8_t* ptr);
void LCD_GLASS_WriteChar(uint8_t* ch, Point_Typedef point, Apostrophe_Typedef apostrophe,uint8_t position);
void LCD_GLASS_ClearChar(uint8_t position);
void LCD_GLASS_Clear(void);
void LCD_GLASS_ScrollString(uint8_t* ptr, uint16_t nScroll, uint16_t ScrollSpeed);

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32L152_EVAL_GLASS_LCD_H */

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
 
/**
  * @}
  */   
  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
