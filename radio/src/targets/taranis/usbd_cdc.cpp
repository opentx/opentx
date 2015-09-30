/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Generic media access Layer.
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

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

#include "opentx.h"

extern "C" {

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern volatile uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */
extern volatile uint32_t APP_Rx_ptr_out;

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);

// static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  0,
  VCP_DataRx
};

}   // extern "C"

bool cdcConnected = false;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  VCP_Init
  *         Initializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Init(void)
{
  cdcConnected = true;
  return USBD_OK;
}

/**
  * @brief  VCP_DeInit
  *         DeInitializes the Media on the STM32
  * @param  None
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_DeInit(void)
{
  cdcConnected = false;
  return USBD_OK;
}


/**
  * @brief  VCP_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion (USBD_OK in all cases)
  */
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    /* Not  needed for this driver */
    break;

  case GET_LINE_CODING:
    /* Not  needed for this driver */
    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}

// some debug vars
uint16_t usbWraps = 0;
uint16_t charsWritten = 0;

void usbSerialPutc(uint8_t c)
{
  if (!cdcConnected) return;

  uint32_t txDataLen;
  do {
    txDataLen = APP_RX_DATA_SIZE + APP_Rx_ptr_in - APP_Rx_ptr_out;
    if (txDataLen >= APP_RX_DATA_SIZE) {
      txDataLen -= APP_RX_DATA_SIZE;
    }
  } while (txDataLen >= (APP_RX_DATA_SIZE - CDC_DATA_MAX_PACKET_SIZE));

  APP_Rx_Buffer[APP_Rx_ptr_in] = c;
  ++charsWritten;
  /* To avoid buffer overflow */
  if (APP_Rx_ptr_in >= APP_RX_DATA_SIZE-1) {
    APP_Rx_ptr_in = 0;
    ++usbWraps;
  }
  else {
    APP_Rx_ptr_in++;
  }  
}

/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint is available here
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *          
  *         @note
  *         This function is executed inside the USBD_OTG_ISR_Handler() interrupt handler!

  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else VCP_FAIL
  */
static uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
  // TODO: try implementing inbound flow control:
  //        if the cliRxFifo does not have enough free space to receive all
  //        available characters, return VCP_FAIL. Maybe that will throttle down
  //        the sender and we will receive the same packet at a later time.

#if defined(CLI)
  //copy data to the application FIFO
  for (uint32_t i = 0; i < Len; i++)
  {
    cliRxFifo.push(Buf[i]);
  } 
#endif

  return USBD_OK;
}



// /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
