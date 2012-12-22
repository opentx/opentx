/**
  ******************************************************************************
  * @file    stm32100e_eval_cec.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains all the functions prototypes for the stm32100e_eval_cec
  *          firmware driver.
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
#ifndef __STM32100E_EVAL_CEC_H
#define __STM32100E_EVAL_CEC_H

#ifdef __cplusplus
extern "C"
{
#endif

  /* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM32100E_EVAL
  * @{
  */  

/** @defgroup STM32100E_EVAL_CEC
  * @{
  */

/** @defgroup STM32100E_EVAL_CEC_Exported_Types
  * @{
  */
typedef enum
{
  HDMI_CEC_BIT_TIMING              = (1), /*!< CEC Bit Timing Error */
  HDMI_CEC_BIT_PERIOD              = (2), /*!< CEC Bit Period Error */
  HDMI_CEC_RX_BLOCK_FINISHED       = (3), /*!< CEC Receive Block finished Error */
  HDMI_CEC_START_BIT               = (4), /*!< CEC Start Bit Error */
  HDMI_CEC_BLOCK_ACKNOWLEDGE       = (5), /*!< CEC Block Acknowledge Error */
  HDMI_CEC_LINE                    = (6), /*!< CEC Line Error */
  HDMI_CEC_TX_BLOCK_FINISHED       = (7), /*!< CEC Transmit Block Transfer finished Error */
  HDMI_CEC_DEVICE_UNREGISTRED      = (8), /*!< CEC Device Unregistred */
  HDMI_CEC_TIMEOUT                 = (9), /*!< CEC TimeOut */
  HDMI_CEC_OK                      = (10)  /*!< CEC OK */
}HDMI_CEC_Error;

typedef struct
{
  __IO uint8_t PhysicalAddress_A;
  __IO uint8_t PhysicalAddress_B;
  __IO uint8_t PhysicalAddress_C;
  __IO uint8_t PhysicalAddress_D;
  __IO uint8_t LogicalAddress;
  __IO uint8_t DeviceType;
}HDMI_CEC_Map;


#define HDMI_CEC_TX_MESSAGE_LENGTH_MAX       ((uint32_t)0x0000000E)
#define HDMI_CEC_TIMEOUT_VALUE               ((uint32_t)0x001FFFFF)

/**
* @brief  CEC Init Structure definition
*/
typedef struct
{
  __IO uint8_t Header;
  __IO uint8_t Opcode;
  __IO uint8_t Operande[HDMI_CEC_TX_MESSAGE_LENGTH_MAX];
  __IO uint8_t TxMessageLength;
  __IO uint8_t RxMessageLength;
}HDMI_CEC_Message;

/**
* @}
*/

/** @defgroup STM32100E_EVAL_CEC_Exported_Constants
  * @{
  */

/**
  * @brief CEC device types
  */
#define HDMI_CEC_TV                                         0x00
#define HDMI_CEC_RECORDING                                  0x01
#define HDMI_CEC_TUNER                                      0x03
#define HDMI_CEC_PLAYBACK                                   0x04
#define HDMI_CEC_AUDIOSYSTEM                                0x05


/**
  * @brief  HDMI CEC I2C Interface pins
  */
#define HDMI_CEC_I2C_SCL_PIN                                GPIO_Pin_6
#define HDMI_CEC_I2C_SDA_PIN                                GPIO_Pin_7
#define HDMI_CEC_I2C_GPIO_PORT                              GPIOB
#define HDMI_CEC_I2C_GPIO_CLK                               RCC_APB2Periph_GPIOB
#define HDMI_CEC_I2C                                        I2C1
#define HDMI_CEC_I2C_CLK                                    RCC_APB1Periph_I2C1

/**
  * @brief  HDMI CEC HPD (Hot Plug Detect) Interface pin
  */
#define HDMI_CEC_HPD_PIN                                    GPIO_Pin_9
#define HDMI_CEC_HPD_GPIO_PORT                              GPIOB
#define HDMI_CEC_HPD_GPIO_CLK                               RCC_APB2Periph_GPIOB

/**
  * @brief  HDMI CEC Interface pin
  */
#define HDMI_CEC_LINE_PIN                                   GPIO_Pin_8
#define HDMI_CEC_LINE_GPIO_PORT                             GPIOB
#define HDMI_CEC_LINE_GPIO_CLK                              RCC_APB2Periph_GPIOB

#define HDMI_CEC_I2C_SLAVE_ADDRESS7                         0xA0
#define HDMI_CEC_I2C_CLOCK_SPEED                            100000

/**
  * @brief  HDMI CEC Root (Mainly for TV with a fixed physical address (0.0.0.0))
  *         If you want to configure the STM32100E-EVAL board as CEC Root (Sink)
  *         change the following define to 0x1    
  */
#define HDMI_CEC_ROOT                                       0x00

/**
  * @brief  To select if the DDC Channel will be used for physical address discovery
  *         or not. To use the DDC Channel to read the EDID structure uncomment
  *         the following line.   
  *         If the device is configured as HMDI source it should read his own physical
  *         address from the sink that is connected to.         
  */
/* #define HDMI_CEC_USE_DDC */                                    

/**
  * @brief  CEC version: V1.3a
  */
#define HDMI_CEC_VERSION                                    0x04

/**
  * @brief  Reason for Abort feature
  */
#define HDMI_CEC_UNRECOGNIZED_OPCODE                        0x00
#define HDMI_CEC_NOT_CORRECT_MODETORESPOND                  0x01
#define HDMI_CEC_CANNOTPROVIDE_SOURCE                       0x02
#define HDMI_CEC_INVALID_OPERAND                            0x03
#define HDMI_CEC_REFUSED                                    0x04

/**
  * @brief  HDMI CEC specific commands
  */
#define HDMI_CEC_OPCODE_ACTIVE_SOURCE                       ((uint8_t) 0x82)
#define HDMI_CEC_OPCODE_IMAGE_VIEW_ON                       ((uint8_t) 0x04)
#define HDMI_CEC_OPCODE_TEXT_VIEW_ON                        ((uint8_t) 0x0D)
#define HDMI_CEC_OPCODE_INACTIVE_SOURCE                     ((uint8_t) 0x9D)
#define HDMI_CEC_OPCODE_REQUEST_ACTIVE_SOURCE               ((uint8_t) 0x85)
#define HDMI_CEC_OPCODE_ROUTING_CHANGE                      ((uint8_t) 0x80)
#define HDMI_CEC_OPCODE_ROUTING_INFORMATION                 ((uint8_t) 0x81)
#define HDMI_CEC_OPCODE_SET_STREAM_PATH                     ((uint8_t) 0x86)
#define HDMI_CEC_OPCODE_STANDBY                             ((uint8_t) 0x36)
#define HDMI_CEC_OPCODE_RECORD_OFF                          ((uint8_t) 0x0B)
#define HDMI_CEC_OPCODE_RECORD_ON                           ((uint8_t) 0x09)
#define HDMI_CEC_OPCODE_RECORD_STATUS                       ((uint8_t) 0x0A)
#define HDMI_CEC_OPCODE_RECORD_TV_SCREEN                    ((uint8_t) 0x0F)
#define HDMI_CEC_OPCODE_CLEAR_ANALOGUE_TIMER                ((uint8_t) 0x33)
#define HDMI_CEC_OPCODE_CLEAR_DIGITAL_TIMER                 ((uint8_t) 0x99)
#define HDMI_CEC_OPCODE_CLEAR_EXTERNAL_TIMER                ((uint8_t) 0xA1)
#define HDMI_CEC_OPCODE_SET_ANALOGUE_TIMER                  ((uint8_t) 0x34)
#define HDMI_CEC_OPCODE_SET_DIGITAL_TIMER                   ((uint8_t) 0x97)
#define HDMI_CEC_OPCODE_SET_EXTERNAL_TIMER                  ((uint8_t) 0xA2)
#define HDMI_CEC_OPCODE_SET_TIMER_PROGRAM_TITLE             ((uint8_t) 0x67)
#define HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS                ((uint8_t) 0x43)
#define HDMI_CEC_OPCODE_TIMER_STATUS                        ((uint8_t) 0x35)
#define HDMI_CEC_OPCODE_CEC_VERSION                         ((uint8_t) 0x9E)
#define HDMI_CEC_OPCODE_GET_CEC_VERSION                     ((uint8_t) 0x9F)
#define HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS               ((uint8_t) 0x83)
#define HDMI_CEC_OPCODE_GET_MENU_LANGUAGE                   ((uint8_t) 0x91)
#define HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS             ((uint8_t) 0x84)
#define HDMI_CEC_OPCODE_SET_MENU_LANGUAGE                   ((uint8_t) 0x32)
#define HDMI_CEC_OPCODE_DECK_CONTROL                        ((uint8_t) 0x42)
#define HDMI_CEC_OPCODE_DECK_STATUS                         ((uint8_t) 0x1B)
#define HDMI_CEC_OPCODE_GIVE_DECK_STATUS                    ((uint8_t) 0x1A)
#define HDMI_CEC_OPCODE_PLAY                                ((uint8_t) 0x41)
#define HDMI_CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS            ((uint8_t) 0x08)
#define HDMI_CEC_OPCODE_SELECT_ANALOGUE_SERVICE             ((uint8_t) 0x92)
#define HDMI_CEC_OPCODE_SELECT_DIGITAL_SERVICE              ((uint8_t) 0x93)
#define HDMI_CEC_OPCODE_TUNER_DEVICE_STATUS                 ((uint8_t) 0x07)
#define HDMI_CEC_OPCODE_TUNER_STEP_DECREMENT                ((uint8_t) 0x06)
#define HDMI_CEC_OPCODE_TUNER_STEP_INCREMENT                ((uint8_t) 0x05)
#define HDMI_CEC_OPCODE_DEVICE_VENDOR_ID                    ((uint8_t) 0x87)
#define HDMI_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID               ((uint8_t) 0x8C)
#define HDMI_CEC_OPCODE_VENDOR_COMMAND                      ((uint8_t) 0x89)
#define HDMI_CEC_OPCODE_VENDOR_COMMAND_WITH_ID              ((uint8_t) 0xA0)
#define HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN           ((uint8_t) 0x8A)
#define HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP             ((uint8_t) 0x8B)
#define HDMI_CEC_OPCODE_SET_OSD_STRING                      ((uint8_t) 0x64)
#define HDMI_CEC_OPCODE_GIVE_OSD_NAME                       ((uint8_t) 0x46)
#define HDMI_CEC_OPCODE_SET_OSD_NAME                        ((uint8_t) 0x47)
#define HDMI_CEC_OPCODE_MENU_REQUEST                        ((uint8_t) 0x8D)
#define HDMI_CEC_OPCODE_MENU_STATUS                         ((uint8_t) 0x8E)
#define HDMI_CEC_OPCODE_USER_CONTROL_PRESSED                ((uint8_t) 0x44)
#define HDMI_CEC_OPCODE_USER_CONTROL_RELEASED               ((uint8_t) 0x45)
#define HDMI_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS            ((uint8_t) 0x8F)
#define HDMI_CEC_OPCODE_REPORT_POWER_STATUS                 ((uint8_t) 0x90)
#define HDMI_CEC_OPCODE_FEATURE_ABORT                       ((uint8_t) 0x00)
#define HDMI_CEC_OPCODE_ABORT                               ((uint8_t) 0xFF)
#define HDMI_CEC_OPCODE_GIVE_AUDIO_STATUS                   ((uint8_t) 0x71)
#define HDMI_CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS       ((uint8_t) 0x7D)
#define HDMI_CEC_OPCODE_REPORT_AUDIO_STATUS                 ((uint8_t) 0x7A)
#define HDMI_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE               ((uint8_t) 0x72)
#define HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST           ((uint8_t) 0x70)
#define HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS            ((uint8_t) 0x7E)
#define HDMI_CEC_OPCODE_SET_AUDIO_RATE                      ((uint8_t) 0x9A)


/**
  * @}
  */

/** @defgroup STM32100E_EVAL_CEC_Exported_Macros
  * @{
  */
/* HDMI_CEC_HPD: HDMI HPD pin low  */
#define HDMI_CEC_HPD_LOW()       GPIO_ResetBits(HDMI_CEC_HPD_GPIO_PORT, HDMI_CEC_HPD_PIN)
/* HDMI_CEC_HPD: HDMI HPD pin high  */
#define HDMI_CEC_HPD_HIGH()      GPIO_SetBits(HDMI_CEC_HPD_GPIO_PORT, HDMI_CEC_HPD_PIN)

/**
  * @}
  */

/** @defgroup STM32100E_EVAL_CEC_Exported_Functions
  * @{
  */
HDMI_CEC_Error HDMI_CEC_Init(void);
HDMI_CEC_Error HDMI_CEC_TransmitMessage(HDMI_CEC_Message *HDMI_CEC_TX_MessageStructure);
HDMI_CEC_Error HDMI_CEC_GetErrorStatus (void);
void HDMI_CEC_ProcessIRQSrc(void);
HDMI_CEC_Error HDMI_CEC_ReportPhysicalAddress(void);
void HDMI_CEC_CommandCallBack(void);
HDMI_CEC_Error HDMI_CEC_CheckConnectedDevices(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32100E_EVAL_CEC_H */

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

