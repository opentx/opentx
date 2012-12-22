/**
  ******************************************************************************
  * @file    stm3210e_eval_fsmc_nor.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains all the functions prototypes for the 
  *          stm3210e_eval_fsmc_nor firmware driver.
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
#ifndef __STM3210E_EVAL_FSMC_NOR_H
#define __STM3210E_EVAL_FSMC_NOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM3210E_EVAL
  * @{
  */
  
/** @addtogroup STM3210E_EVAL_FSMC_NOR
  * @{
  */  

/** @defgroup STM3210E_EVAL_FSMC_NOR_Exported_Types
  * @{
  */
typedef struct
{
  uint16_t Manufacturer_Code;
  uint16_t Device_Code1;
  uint16_t Device_Code2;
  uint16_t Device_Code3;
}NOR_IDTypeDef;

/* NOR Status */
typedef enum
{
  NOR_SUCCESS = 0,
  NOR_ONGOING,
  NOR_ERROR,
  NOR_TIMEOUT
}NOR_Status;  
/**
  * @}
  */
  
/** @defgroup STM3210E_EVAL_FSMC_NOR_Exported_Constants
  * @{
  */ 
/**
  * @}
  */ 
  
/** @defgroup STM3210E_EVAL_FSMC_NOR_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM3210E_EVAL_FSMC_NOR_Exported_Functions
  * @{
  */ 
void NOR_Init(void);
void NOR_ReadID(NOR_IDTypeDef* NOR_ID);
NOR_Status NOR_EraseBlock(uint32_t BlockAddr);
NOR_Status NOR_EraseChip(void);
NOR_Status NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data);
NOR_Status NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
NOR_Status NOR_ProgramBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
uint16_t NOR_ReadHalfWord(uint32_t ReadAddr);
void NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);
NOR_Status NOR_ReturnToReadMode(void);
NOR_Status NOR_Reset(void);
NOR_Status NOR_GetStatus(uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#endif /* __STM3210E_EVAL_FSMC_NOR_H */
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
