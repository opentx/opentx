/**
  ******************************************************************************
  * @file    usbd_customhid_core.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   header file for the usbd_customhid_core.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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

/* Includes ------------------------------------------------------------------*/

#ifndef __USB_CUSTOMHID_CORE_H_
#define __USB_CUSTOMHID_CORE_H_

#include  "usbd_ioreq.h"
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_CUSTOMHID
  * @brief This file is the Header file for usbd_customhid_core.c
  * @{
  */ 


/** @defgroup USBD_CUSTOMHID_Exported_Defines
  * @{
  */ 
#define USBD_CUSTOM_HID_REPORT_DESC_SIZE     163

#define USB_CUSTOM_HID_CONFIG_DESC_SIZ       41
#define USB_CUSTOM_HID_DESC_SIZ              9

#define CUSTOM_HID_DESCRIPTOR_TYPE           0x21
#define CUSTOM_HID_REPORT_DESC               0x22


#define CUSTOM_HID_REQ_SET_PROTOCOL          0x0B
#define CUSTOM_HID_REQ_GET_PROTOCOL          0x03

#define CUSTOM_HID_REQ_SET_IDLE              0x0A
#define CUSTOM_HID_REQ_GET_IDLE              0x02

#define CUSTOM_HID_REQ_SET_REPORT            0x09
#define CUSTOM_HID_REQ_GET_REPORT            0x01

#define LED1_REPORT_ID                       0x01
#define LED1_REPORT_COUNT                    0x01

#define LED2_REPORT_ID                       0x02
#define LED2_REPORT_COUNT                    0x01

#define LED3_REPORT_ID                       0x03
#define LED3_REPORT_COUNT                    0x01

#define LED4_REPORT_ID                       0x04
#define LED4_REPORT_COUNT                    0x01

#define KEY_REPORT_ID                        0x05
#define TAMPER_REPORT_ID                     0x06
#define ADC_REPORT_ID                        0x07
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 
extern USBD_Class_cb_TypeDef  USBD_CUSTOMHID_cb;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
uint8_t USBD_CUSTOM_HID_SendReport (USB_OTG_CORE_HANDLE  *pdev, 
                                 uint8_t *report,
                                 uint16_t len);
/**
  * @}
  */ 

#endif  /* __USB_CUSTOMHID_CORE_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
