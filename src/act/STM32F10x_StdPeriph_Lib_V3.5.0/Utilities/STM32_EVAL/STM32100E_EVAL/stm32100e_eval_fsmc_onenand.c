/**
  ******************************************************************************
  * @file    stm32100e_eval_fsmc_onenand.c
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file provides a set of functions needed to drive the
  *          KFG1216U2A/B-DIB6 OneNAND memory mounted on STM32100E-EVAL board.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32100e_eval_fsmc_onenand.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM32100E_EVAL
  * @{
  */
  
/** @addtogroup STM32100E_EVAL_FSMC_ONENAND
  * @brief      This file provides a set of functions needed to drive the
  *             KFG1216x2A-xxB5 OneNAND memory mounted on STM32100E-EVAL board.
  * @{
  */ 

/** @defgroup STM32100E_EVAL_FSMC_ONENAND_Private_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32100E_EVAL_FSMC_ONENAND_Private_Defines
  * @{
  */ 
#define BANK1_ONENAND1_ADDR           ((uint32_t)0x60000000)
#define ONENAND_BOOTPARTITION_ADDR    ((uint32_t)BANK1_ONENAND1_ADDR)

 
/**
  * @}
  */ 

/** @defgroup STM32100E_EVAL_FSMC_ONENAND_Private_Macros
  * @{
  */
#define OneNAND_WRITE(Address, Data)  (*(__IO uint16_t *)(Address) = (Data))
  
/**
  * @}
  */ 
  

/** @defgroup STM32100E_EVAL_FSMC_ONENAND_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32100E_EVAL_FSMC_ONENAND_Private_Function_Prototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32100E_EVAL_FSMC_ONENAND_Private_Functions
  * @{
  */ 

/**
  * @brief  Configures the FSMC and GPIOs to interface with the OneNAND memory.
  *         This function must be called before any write/read operation on the 
  *         OneNAND.
  * @param  None
  * @retval None
  */
void OneNAND_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure;


  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE); 
  
/*-- GPIO Configuration ------------------------------------------------------*/
  /* OneNAND Data lines configuration */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  
/*-- GPIO Configuration ------------------------------------------------------*/
  /*!< OneNAND Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /*!< OneNAND Address lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | 
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
     
  /*!< CLK, NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /*!< NE1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /*!< NL configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinRemapConfig(GPIO_Remap_FSMC_NADV, DISABLE);

  /*!< NWAIT configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
    
  /*-- FSMC Configuration ----------------------------------------------------*/
  p.FSMC_AddressSetupTime = 0x01;
  p.FSMC_AddressHoldTime = 0x00;
  p.FSMC_DataSetupTime = 0x05;
  p.FSMC_BusTurnAroundDuration = 0x02;
  p.FSMC_CLKDivision = 0x1;
  p.FSMC_DataLatency = 0x01;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Enable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Enable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 
}

/**
  * @brief  Resets the OneNAND memory.
  * @param  None
  * @retval None
  */
void OneNAND_Reset(void)
{
  OneNAND_WRITE(ONENAND_BOOTPARTITION_ADDR, OneNAND_CMD_RESET);
}

/**
  * @brief  Reads OneNAND memory's Manufacturer and Device Code.
  * @param  OneNAND_ID: pointer to a OneNAND_IDTypeDef structure which will hold
  *         the Manufacturer and Device Code.  
  * @retval None
  */
void OneNAND_ReadID(OneNAND_IDTypeDef* OneNAND_ID)
{
  uint16_t status = 0x0;
  
  /* Wait till no ongoing operation */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  while((status & 0x8000) == 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  }
  
  /* Read ID data */
  OneNAND_ID->Manufacturer_ID = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_MANUFACTERID);
  OneNAND_ID->Device_ID = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_DEVICEID);

  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_SYSTEMCONFIGURATION) = 0x40E0;
}

/**
  * @brief  Unlocks the specified OneNAND memory block (128Kb).
  * @param  BlockNumber: specifies the block number to be erased. This parameter
  *         should be between 0 and 511.
  * @retval OneNAND memory Interrupt Status.
  */
uint16_t OneNAND_UnlockBlock(uint32_t BlockNumber)
{
  uint16_t  status = 0;
  
  /* Wait till no ongoing operation */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  while((status & 0x8000) == 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  }
  
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTBLOCKADDRESS) = BlockNumber;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT) = 0x0000;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_COMMAND) = OneNAND_CMD_UNLOCK;

  /* Wait till the command is completed */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);

  while((status & 0x8000) != 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);
  }

  /* Get the Controller Status */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  return (status);
}

/**
  * @brief  Erases the specified OneNAND memory block (128Kb).
  * @param  BlockNumber: specifies the block number to be erased. This parameter
  *         should be between 0 and 511.
  * @retval OneNAND memory Interrupt Status.
  */
uint16_t OneNAND_EraseBlock(uint32_t BlockNumber)
{
  uint16_t status = 0x0;
  
  /* Wait till no ongoing operation */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  while((status & 0x8000) == 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  }

  /* Erase operation */
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESS1) = BlockNumber;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT) = 0x0000;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_COMMAND) = OneNAND_CMD_ERASE;

  /* Wait till no error is generated */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  while((status & 0x0400) == 0x0400)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  }
  
  /* Wait till the command is completed */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);

  while((status & 0x8000) != 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);
  }

  /* Get the Controller Status */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  return (status);
}

/**
  * @brief  Writes a Half-word buffer to the OneNAND memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  WriteAddr: OneNAND memory internal address from which the data will be 
  *         written.
  * @param  NumHalfwordToWrite: number of half-words to write. 
  * @retval OneNAND memory Controller Status.
  */
uint16_t OneNAND_WriteBuffer(uint16_t* pBuffer, OneNAND_ADDRESS Address, uint32_t NumHalfwordToWrite)
{
  uint32_t datacounter = 0;
  uint16_t status = 0;

  /* Load the buffer to be written into the DATA RAM0*/ 
  for(datacounter = 0; datacounter < NumHalfwordToWrite; datacounter++)
  {
    *(__IO uint16_t *)((BANK1_ONENAND1_ADDR + OneNAND_DATA_RAM_0_0_ADD) + (2*datacounter)) = pBuffer[datacounter];
  } 

  /* Write operation from DATA RAM0 to NAND address*/ 
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESS1) = Address.Block; /* NAND Flash block address*/
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESINT8_T) = (uint16_t)(Address.Page << 2);  /* NAND Flash start page address */
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTBUFFER) = OneNAND_DATA_RAM_0_0_REG;/* BufferRAM Sector Count (BSC) and BufferRAM Sector Address (BSA).*/
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT) = 0x0000;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_COMMAND) = OneNAND_CMD_PROGRAM; /* Command */ 

  /* Wait till the command is completed */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);

  while((status & 0x8000) != 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);
  }

  /* Wait till the write interrupt is set */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);

  while((status & 0x40) != 0x40)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);
  }

  /* Get the Controller Status */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);
  
  return (status);
}

/**
  * @brief  Reads a block of data from the OneNAND memory using asynchronous mode.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *         OneNAND memory.
  * @param  ReadAddr: OneNAND memory internal address to read from.
  * @param  NumHalfwordToRead: number of half-words to read.
  * @retval None
  */
void OneNAND_AsynchronousRead(uint16_t* pBuffer, OneNAND_ADDRESS Address, uint32_t NumHalfwordToRead)
{
  uint16_t datatmp = 0x0, index = 0;
  uint16_t status = 0;

  datatmp = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_SYSTEMCONFIGURATION); 

  /* Set the asynchronous read mode */
  OneNAND_WRITE(BANK1_ONENAND1_ADDR + OneNAND_REG_SYSTEMCONFIGURATION, (datatmp& 0x7FFF));

  /* Load data from the read address to the DATA RAM 1 setor 1 */
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESS1) = Address.Block; /* NAND Flash block address*/
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESINT8_T) = (uint16_t)(Address.Page << 2);
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTBUFFER) = OneNAND_DATA_RAM_1_0_REG;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT) = 0x0000;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_COMMAND) = OneNAND_CMD_LOAD; /* Command */

  /* Wait till the command is completed */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);

  while((status & 0x8000) != 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);
  }

  /* Read Controller status */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);

  /* Read data */
  for(; NumHalfwordToRead != 0x00; NumHalfwordToRead--) /* While there is data to read */
  {
    /* Read a Halfword from the memory */
    *pBuffer++ = *(__IO uint16_t *)((BANK1_ONENAND1_ADDR + OneNAND_DATA_RAM_1_0_ADD)+ 2*index);
    index++;
  } 
}

/**
  * @brief  Reads a block of data from the OneNAND memory using synchronous mode.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *         OneNAND memory.
  * @param  ReadAddr: OneNAND memory internal address to read from.
  * @param  NumHalfwordToRead: number of half-words to read.
  * @retval None
  */
void OneNAND_SynchronousRead(uint16_t* pBuffer, OneNAND_ADDRESS Address, uint32_t NumHalfwordToRead)
{
  uint16_t index = 0;
  uint16_t status = 0;

  /* Set the asynchronous read mode */
  OneNAND_WRITE(BANK1_ONENAND1_ADDR + OneNAND_REG_SYSTEMCONFIGURATION, 0xB4C0);
  

  /* Load data from the read address to the DATA RAM 1 setor 1 */
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESS1) = Address.Block; /* NAND Flash block address*/
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTADDRESINT8_T) = (uint16_t)(Address.Page << 2);
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_STARTBUFFER) = OneNAND_DATA_RAM_1_0_REG;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT) = 0x0000;
  *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_COMMAND) = OneNAND_CMD_LOAD; /* Command */

  /* Wait till the command is completed */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);

  while((status & 0x8000) != 0x8000)
  {
    status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT);
  }

  /* Read Controller status */
  status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS);

  /* Read data */
  for(; NumHalfwordToRead != 0x00; NumHalfwordToRead--) /* While there is data to read */
  {
   *pBuffer++ = *(__IO uint16_t *)((BANK1_ONENAND1_ADDR + OneNAND_DATA_RAM_1_0_ADD + 2*index));
    index++;
  }
}

/**
  * @brief  Reads the OneNAND memory Interrupt status. 
  * @param  None
  * @retval OneNAND memory Interrupt Status.
  */
uint16_t OneNAND_ReadStatus(void)
{
  __IO uint16_t status = 0x0;

  /* Read Status */
  return (status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_INTERRUPT));
}

/**
  * @brief  Reads the OneNAND Controller status. 
  * @param  None
  * @retval OneNAND Controller Status.
  */
uint16_t OneNAND_ReadControllerStatus(void)
{
  __IO uint16_t status = 0x0;

  /* Read Controller Status */
  return (status = *(__IO uint16_t *)(BANK1_ONENAND1_ADDR + OneNAND_REG_CONTROLSTATUS));
}

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
