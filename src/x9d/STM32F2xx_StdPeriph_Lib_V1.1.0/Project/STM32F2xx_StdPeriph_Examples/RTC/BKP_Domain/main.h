/**
  ******************************************************************************
  * @file    RTC/BKP_Domain/main.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Header for main.c module
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "stm322xg_eval.h"
#include "lcd_log.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RTC_Config(void);
uint8_t ReadDigit(uint16_t LineBegin, uint16_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin);
void Time_Regulate(void);
void Time_Adjust(void);
void Time_Display(void);
void Date_Regulate(void);
void Date_Adjust(void);
void Date_Display(void);
void Calendar_Show(void);
void WriteToBackupReg(uint16_t FirstBackupData);
uint32_t CheckBackupReg(uint16_t FirstBackupData);
Button_TypeDef ReadKey(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
