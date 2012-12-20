/**
  ******************************************************************************
  * @file    stm322xg_eval_fsmc_onenand.h
  * @author  MCD Application Team
  * @version V5.0.3
  * @date    09-March-2012
  * @brief   This file contains all the functions prototypes for the 
  *          stm322xg_eval_fsmc_onenand.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
   
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM322xG_EVAL_FSMC_ONENAND_H
#define __STM322xG_EVAL_FSMC_ONENAND_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm322xg_eval.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM322xG_EVAL
  * @{
  */
  
/** @addtogroup STM322xG_EVAL_FSMC_ONENAND
  * @{
  */  

/** @defgroup STM322xG_EVAL_FSMC_ONENAND_Exported_Types
  * @{
  */
typedef struct
{
  uint16_t Manufacturer_ID;
  uint16_t Device_ID;
}OneNAND_IDTypeDef;

/* OneNand Status */
typedef enum
{
  OneNAND_SUCCESS = 0,
  OneNAND_ONGOING,
  OneNAND_ERROR,
  OneNAND_TIMEOUT
}OneNAND_Status;

typedef struct 
{
  uint16_t Block;
  uint16_t Page;
} OneNAND_ADDRESS; 
 
/**
  * @}
  */
  
/** @defgroup STM322xG_EVAL_FSMC_ONENAND_Exported_Constants
  * @{
  */
/** 
  * @brief  OneNAND memory command  
  */  
#define OneNAND_CMD_ERASE                    ((uint16_t)0x0094)
#define OneNAND_CMD_PROGRAM                  ((uint16_t)0x0080)
#define OneNAND_CMD_RESET                    ((uint16_t)0x00F0)
#define OneNAND_CMD_READ_ID                  ((uint16_t)0x0090)
#define OneNAND_CMD_UNLOCK                   ((uint16_t)0x0023)
#define OneNAND_CMD_LOAD                     ((uint16_t)0x0000)

/** 
  * @brief OneNand Register description  
  */  
#define OneNAND_REG_MANUFACTERID             ((uint32_t)0x1E000) /* Manufacturer identification */
#define OneNAND_REG_DEVICEID                 ((uint32_t)0x1E002) /* Device identification */
#define OneNAND_REG_DATABUFFERSIZE           ((uint32_t)0x1E006) /* Data Buffer size */
#define OneNAND_REG_BOOTBUFFERSIZE           ((uint32_t)0x1E008) /* Boot Buffer size */
#define OneNAND_REG_AMOUNTOFBUFFERS          ((uint32_t)0x1E00A) /* Amount of data/boot buffers */
#define OneNAND_REG_TECHNOLOGY               ((uint32_t)0x1E00C) /* Info about technology */
#define OneNAND_REG_STARTADDRESS1            ((uint32_t)0x1E200) /* Nand Flash Block Address */
#define OneNAND_REG_STARTADDRESS3            ((uint32_t)0x1E204) /* Destination Block address for copy back program */
#define OneNAND_REG_STARTADDRESS4            ((uint32_t)0x1E206) /* Destination Page & sector address for copy back program */
#define OneNAND_REG_STARTADDRESINT8_T        ((uint32_t)0x1E20E) /* Nand Flash Page & sector address */
#define OneNAND_REG_STARTBUFFER              ((uint32_t)0x1E400) /* The meaning is with which buffer to start and how many
                                                                     buffers to use for the data transfer */
#define OneNAND_REG_COMMAND                  ((uint32_t)0x1E440) /* Host control and memory operation commands*/
#define OneNAND_REG_SYSTEMCONFIGURATION      ((uint32_t)0x1E442) /* Memory and host interface configuration */
#define OneNAND_REG_CONTROLSTATUS            ((uint32_t)0x1E480) /* Control status and result of memory operation */
#define OneNAND_REG_INTERRUPT                ((uint32_t)0x1E482) /* Memory Command Completion Interrupt Status */
#define OneNAND_REG_STARTBLOCKADDRESS        ((uint32_t)0x1E498) /* Start memory block address in Write Protection mode */
#define OneNAND_REG_WRITEPROTECTIONSTATUS    ((uint32_t)0x1E49C) /* Current memory Write Protection status */

/** 
  * @brief OneNand Memory partition description  
  */
#define OneNAND_DATA_RAM_0_0_ADD            ((uint32_t)0x0400) /* DataRAM Main page0/sector0 */
#define OneNAND_DATA_RAM_0_0_REG            ((uint32_t)0x0800) /* DataRAM 0_0 is selected with 4 sector */

#define OneNAND_DATA_RAM_0_1_ADD            ((uint32_t)0x0600) /* DataRAM Main page0/sector1 */
#define OneNAND_DATA_RAM_0_1_REG            ((uint32_t)0x0900) /* DataRAM 0_1 is selected with 4 sector */

#define OneNAND_DATA_RAM_0_2_ADD            ((uint32_t)0x0800) /* DataRAM Main page0/sector2 */
#define OneNAND_DATA_RAM_0_2_REG            ((uint32_t)0x0A00) /* DataRAM 0_2 is selected with 4 sector */

#define OneNAND_DATA_RAM_0_3_ADD            ((uint32_t)0x0A00) /* DataRAM Main page0/sector3 */
#define OneNAND_DATA_RAM_0_3_REG            ((uint32_t)0x0B00) /* DataRAM 0_3 is selected with 4 sector */

#define OneNAND_DATA_RAM_1_0_ADD            ((uint32_t)0x0C00) /* DataRAM Main page1/sector0 */
#define OneNAND_DATA_RAM_1_0_REG            ((uint32_t)0x0C00) /* DataRAM 1_0 is selected with 4 sector */

#define OneNAND_DATA_RAM_1_1_ADD            ((uint32_t)0x0E00) /* DataRAM Main page1/sector1 */
#define OneNAND_DATA_RAM_1_1_REG            ((uint32_t)0x0D00) /* DataRAM 1_1 is selected with 4 sector */

#define OneNAND_DATA_RAM_1_2_ADD            ((uint32_t)0x1000) /* DataRAM Main page1/sector2 */
#define OneNAND_DATA_RAM_1_2_REG            ((uint32_t)0x0E00) /* DataRAM 1_2 is selected with 4 sector */

#define OneNAND_DATA_RAM_1_3_ADD            ((uint32_t)0x1200) /* DataRAM Main page1/sector3 */
#define OneNAND_DATA_RAM_1_3_REG            ((uint32_t)0x0F00) /* DataRAM 1_3 is selected with 4 sector */

/**
  * @}
  */ 
  
/** @defgroup STM322xG_EVAL_FSMC_ONENAND_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM322xG_EVAL_FSMC_ONENAND_Exported_Functions
  * @{
  */ 
void OneNAND_Init(void);
void OneNAND_Reset(void);
void OneNAND_ReadID(OneNAND_IDTypeDef* OneNAND_ID);
uint16_t OneNAND_UnlockBlock(uint16_t BlockNumber);
uint16_t OneNAND_EraseBlock(uint16_t BlockNumber);
uint16_t OneNAND_WriteBuffer(uint16_t* pBuffer, OneNAND_ADDRESS Address, uint32_t NumHalfwordToWrite);
void OneNAND_AsynchronousRead(uint16_t* pBuffer, OneNAND_ADDRESS Address, uint32_t NumHalfwordToRead);
void OneNAND_SynchronousRead(uint16_t* pBuffer, OneNAND_ADDRESS Address, uint32_t NumHalfwordToRead);
uint16_t OneNAND_ReadStatus(void);
uint16_t OneNAND_ReadControllerStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM322xG_EVAL_FSMC_ONENAND_H */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
