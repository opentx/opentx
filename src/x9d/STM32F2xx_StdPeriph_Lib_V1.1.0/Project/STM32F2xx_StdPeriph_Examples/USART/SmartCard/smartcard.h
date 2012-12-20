/**
  ******************************************************************************
  * @file    USART/Smartcard/smartcard.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   This file contains all the functions prototypes for the Smartcard
  *          firmware library.
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
#ifndef __SMARTCARD_H
#define __SMARTCARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"
#include "platform_config.h"

/* Exported constants --------------------------------------------------------*/
#define T0_PROTOCOL        0x00  /* T0 protocol */
#define DIRECT             0x3B  /* Direct bit convention */
#define INDIRECT           0x3F  /* Indirect bit convention */
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define LC_MAX             20
#define SC_RECEIVE_TIMEOUT 0x8000  /* Direction to reader */

/* SC Tree Structure -----------------------------------------------------------
                              MasterFile
                           ________|___________
                          |        |           |
                        System   UserData     Note
------------------------------------------------------------------------------*/

/* SC ADPU Command: Operation Code -------------------------------------------*/
#define SC_CLA_GSM11       0xA0

/*------------------------ Data Area Management Commands ---------------------*/
#define SC_SELECT_FILE     0xA4
#define SC_GET_RESPONCE    0xC0
#define SC_STATUS          0xF2
#define SC_UPDATE_BINARY   0xD6
#define SC_READ_BINARY     0xB0
#define SC_WRITE_BINARY    0xD0
#define SC_UPDATE_RECORD   0xDC
#define SC_READ_RECORD     0xB2

/*-------------------------- Administrative Commands -------------------------*/ 
#define SC_CREATE_FILE     0xE0

/*-------------------------- Safety Management Commands ----------------------*/
#define SC_VERIFY          0x20
#define SC_CHANGE          0x24
#define SC_DISABLE         0x26
#define SC_ENABLE          0x28
#define SC_UNBLOCK         0x2C
#define SC_EXTERNAL_AUTH   0x82
#define SC_GET_CHALLENGE   0x84

/*-------------------------- Answer to reset Commands ------------------------*/ 
#define SC_GET_A2R         0x00

/* SC STATUS: Status Code ----------------------------------------------------*/
#define SC_EF_SELECTED     0x9F
#define SC_DF_SELECTED     0x9F
#define SC_OP_TERMINATED   0x9000

/* Smartcard Voltage */
#define SC_VOLTAGE_5V      0
#define SC_VOLTAGE_3V      1

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SC_POWER_ON = 0x00,
  SC_RESET_LOW = 0x01,
  SC_RESET_HIGH = 0x02,
  SC_ACTIVE = 0x03,	 
  SC_ACTIVE_ON_T0 = 0x04,
  SC_POWER_OFF = 0x05
} SC_State;

/* ATR structure - Answer To Reset -------------------------------------------*/
typedef struct
{
  uint8_t TS;               /* Bit Convention */
  uint8_t T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
  uint8_t T[SETUP_LENGTH];  /* Setup array */
  uint8_t H[HIST_LENGTH];   /* Historical array */
  uint8_t Tlength;          /* Setup array dimension */
  uint8_t Hlength;          /* Historical array dimension */
} SC_ATR;

/* ADPU-Header command structure ---------------------------------------------*/
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
} SC_Header;

/* ADPU-Body command structure -----------------------------------------------*/
typedef struct 
{
  uint8_t LC;           /* Data field length */
  uint8_t Data[LC_MAX];  /* Command parameters */
  uint8_t LE;           /* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct
{
  SC_Header Header;
  SC_Body Body;
} SC_ADPU_Commands;

/* SC response structure -----------------------------------------------------*/
typedef struct
{
  uint8_t Data[LC_MAX];  /* Data returned from the card */
  uint8_t SW1;          /* Command Processing status */
  uint8_t SW2;          /* Command Processing qualification */
} SC_ADPU_Responce;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* APPLICATION LAYER ---------------------------------------------------------*/
void SC_Handler(SC_State *SCState, SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_Response);
void SC_PowerCmd(FunctionalState NewState);
void SC_Reset(BitAction ResetState);
void SC_IOConfig(void);
void SC_ParityErrorHandler(void);
void SC_PTSConfig(void);

#endif /* __SMARTCARD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
