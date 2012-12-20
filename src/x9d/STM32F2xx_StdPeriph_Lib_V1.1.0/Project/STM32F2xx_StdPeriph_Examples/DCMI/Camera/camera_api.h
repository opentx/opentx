/**
  ******************************************************************************
  * @file    DCMI/Camera/camera_api.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Header for camera_api.c module
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
#ifndef __CAMERA_API_H
#define __CAMERA_API_H

/* Includes ------------------------------------------------------------------*/
#include "lcd_log.h"
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define DCMI_DR_ADDRESS       0x50050028
#define FSMC_LCD_ADDRESS      0x68000002
#define NOKEY                 0
#define SEL                   1
#define UP                    2
#define DOWN                  3

/* Exported types ------------------------------------------------------------*/
/* Camera devices enumeration */
typedef enum   
{
  OV9655_CAMERA            =   0x00,	 /* Use OV9655 Camera */
  OV2640_CAMERA            =   0x01      /* Use OV2640 Camera */
}Camera_TypeDef;

/* Image Sizes enumeration */
typedef enum   
{
  BMP_QQVGA             =   0x00,	    /* BMP Image QQVGA 160x120 Size */
  BMP_QVGA              =   0x01,           /* BMP Image QVGA 320x240 Size */
  JPEG_160x120          =   0x02,	    /* JPEG Image 160x120 Size */
  JPEG_176x144          =   0x03,	    /* JPEG Image 176x144 Size */
  JPEG_320x240          =   0x04,	    /* JPEG Image 320x240 Size */
  JPEG_352x288          =   0x05	    /* JPEG Image 352x288 Size */
}ImageFormat_TypeDef;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t Demo_Init(void);
void Display_Menu(uint8_t ForematIndex, uint8_t MaxForematIndex);
void Camera_Config(void);
void OV2640_SpecialEffects(uint8_t index);
void Delay(uint32_t nTime);
void TimingDelay_Decrement(void);
void Demo_LCD_Clear(void);

#endif /* __CAMERA_API_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
