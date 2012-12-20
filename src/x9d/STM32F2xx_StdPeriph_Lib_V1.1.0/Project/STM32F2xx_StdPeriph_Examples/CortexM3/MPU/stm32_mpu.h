/**
  ******************************************************************************
  * @file    CortexM3/MPU/stm32_mpu.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Header for stm32_mpu.c module
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
#ifndef __STM32_MPU_H
#define __STM32_MPU_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MPU_RASR_XN_Pos                          28  
#define MPU_RASR_XN_Msk                          (1UL << MPU_RASR_XN_Pos)                       
#define MPU_RASR_AP_Pos                          24  
#define MPU_RASR_AP_Msk                          (7UL << MPU_RASR_AP_Pos)                      
#define RAM_ADDRESS_START                        (0x20000000UL)
#define RAM_SIZE                                 (0x19UL << 0UL)
#define PERIPH_ADDRESS_START                     (0x40000000)
#define PERIPH_SIZE                              (0x39UL << 0UL)
#define FLASH_ADDRESS_START                      (0x08000000)
#define FLASH_SIZE                               (0x27UL << 0UL)
#define portMPU_REGION_READ_WRITE                (0x03UL << MPU_RASR_AP_Pos)
#define portMPU_REGION_PRIVILEGED_READ_ONLY      (0x05UL << MPU_RASR_AP_Pos)
#define portMPU_REGION_READ_ONLY                 (0x06UL << MPU_RASR_AP_Pos)
#define portMPU_REGION_PRIVILEGED_READ_WRITE     (0x01UL << MPU_RASR_AP_Pos)
#define RAM_REGION_NUMBER                        (0x00UL << MPU_RNR_REGION_Pos)
#define FLASH_REGION_NUMBER                      (0x01UL << MPU_RNR_REGION_Pos)
#define PERIPH_REGION_NUMBER                     (0x02UL << MPU_RNR_REGION_Pos)


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void MPU_Config(void);
void MPU_AccessPermConfig(void);

#endif /* __STM32_MPU_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
