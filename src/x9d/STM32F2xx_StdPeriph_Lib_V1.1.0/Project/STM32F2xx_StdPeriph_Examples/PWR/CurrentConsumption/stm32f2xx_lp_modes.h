/**
  ******************************************************************************
  * @file    PWR/CurrentConsumption/stm32f2xx_lp_modes.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Header for stm32f2xx_lp_modes.c module
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
#ifndef __STM32F2xx_LP_MODES_H
#define __STM32F2xx_LP_MODES_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm322xg_eval.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_RTC_BKPSRAM_MODE)
/* Uncomment the corresponding line to select the STM32F2xx Low Power mode */
#define SLEEP_MODE
//#define STOP_MODE
//#define STANDBY_MODE
//#define STANDBY_RTC_MODE
//#define STANDBY_RTC_BKPSRAM_MODE
#endif

#if !defined (SLEEP_MODE) && !defined (STOP_MODE) && !defined (STANDBY_MODE)\
 && !defined (STANDBY_RTC_MODE) && !defined (STANDBY_RTC_BKPSRAM_MODE)
 #error "Please select first the target STM32F2xx Low Power mode to be measured (in stm32f2xx_lp_modes.h file)"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SleepMode_Measure(void);
void StopMode_Measure(void);
void StandbyMode_Measure(void);
void StandbyRTCMode_Measure(void);
void StandbyRTCBKPSRAMMode_Measure(void);

#endif /* __STM32F2xx_LP_MODES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
