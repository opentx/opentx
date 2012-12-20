/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/DataExchangeDMA/main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_DataExchangeDMA
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
uint8_t HEADER_ADDRESS_Write = (((SLAVE_ADDRESS & 0xFF00) >> 7) | 0xF0);
uint8_t HEADER_ADDRESS_Read = (((SLAVE_ADDRESS & 0xFF00) >> 7) | 0xF1);
#define countof(a)   (sizeof(a) / sizeof(*(a)))  

/* Private variables ---------------------------------------------------------*/
I2C_InitTypeDef  I2C_InitStructure;
DMA_InitTypeDef  DMA_InitStructure;
uint8_t TxBuffer[] = "I2C DMA Example: Communication between two I2C using DMA";
uint8_t RxBuffer [256];
__IO uint32_t TimeOut = 0x0;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
static void I2C_Config(void);
static void SysTickConfig(void);
static void TimeOut_UserCallback(void);
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */     

  /* I2C configuration ---------------------------------------------------------*/
  I2C_Config();
  
  /* Initialize LEDs mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* SysTick configuration -----------------------------------------------------*/
  SysTickConfig();
  
  /* Clear the RxBuffer */
  Fill_Buffer(RxBuffer, RXBUFFERSIZE);
  
  /*************************************Master Code******************************/
#if defined (I2C_MASTER)
 
  /* I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  /* I2C Initialize */
  I2C_Init(I2Cx, &I2C_InitStructure);
  
  /* Master Transmitter --------------------------------------------------------*/   
  
  /* Generate the Start condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  
#ifdef I2C_10BITS_ADDRESS  
  /* Test on I2C1 EV5 and clear it */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send Header to I2Cx for write or time out */
  I2C_SendData(I2Cx, HEADER_ADDRESS_Write);
  /* Test on I2Cx EV9 and clear it */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx slave Address for write */
  I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);
  
 #else /* I2C_7BITS_ADDRESS */
      
  /* Test on I2Cx EV5 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx slave Address for write */
  I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Transmitter);
      
#endif /* I2C_10BITS_ADDRESS */
  
  /* Test on I2Cx EV6 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE);
  
  /* Enable DMA TX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Wait until DMA Transfer Complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_TX,I2Cx_TX_DMA_TCFLAG) == RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* I2Cx DMA Disable */
  I2C_DMACmd(I2Cx, DISABLE);
  
  /* Wait until BTF Flag is set before generating STOP or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_GetFlagStatus(I2Cx,I2C_FLAG_BTF))&&(TimeOut != 0x00))  
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Send I2Cx STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Disable DMA TX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
  
  /* Master Receiver -----------------------------------------------------------*/ 
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(I2Cx, ENABLE);
  
  /* Send I2Cx START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  
#ifdef I2C_10BITS_ADDRESS  
  /* Test on EV5 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send Header to Slave for write */
  I2C_SendData(I2Cx, HEADER_ADDRESS_Write);

  /* Test on EV9 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send slave Address */
  I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);

  /* Test on I2Cx EV6 and clear it or time out*/
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Repeated Start */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Test on EV5 and clear it or time out*/
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {} 
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send Header to Slave for Read */
  I2C_SendData(I2Cx, HEADER_ADDRESS_Read);

#else /* I2C_7BITS_ADDRESS */
      
  /* Test on I2Cx EV5 and clear it or time out*/
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx slave Address for write */
  I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Receiver);
      
#endif /* I2C_10BITS_ADDRESS */
  
  /* Test on I2Cx EV6 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE);
  
  /* Enable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Transfer complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_RX,I2Cx_RX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  /* Send I2Cx STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  /* Disable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2Cx,DISABLE);
  
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
  
  if (Buffercmp(TxBuffer, RxBuffer, RXBUFFERSIZE) == PASSED)
  {
    /* LED2, LED3 and LED4 Toggle */
    STM_EVAL_LEDOn(LED2);
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
  }
  else 
  {   /* ED2, LED3 and LED4 On */
    STM_EVAL_LEDOff(LED2);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
  }
  
#endif /* I2C_MASTER */
  
/**********************************Slave Code**********************************/
#if defined (I2C_SLAVE)
  
  /* Initialize I2C peripheral */
  /* I2C Init */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddress1 = SLAVE_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  I2C_Init(I2Cx, &I2C_InitStructure);
 
  /* Slave Receiver ------------------------------------------------------------*/

  /* Test on I2C EV1 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED))
  {}

  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE); 
  
  /* Enable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Transfer complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_RX,I2Cx_RX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }  
  /* Test on I2Cx EV4 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_STOP_DETECTED))&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }  
  /* Disable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_RX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2Cx,DISABLE);
  
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
  
/* Slave Transmitter ---------------------------------------------------------*/   
  
  /* Test on I2C EV1 and clear it or time out*/
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED))
  {}
  /* I2Cx DMA Enable */
  I2C_DMACmd(I2Cx, ENABLE); 
  
  /* Enable DMA RX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, ENABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX enabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= ENABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Transfer complete or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_TX,I2Cx_TX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
  {}
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Test on I2Cx EV3-2 and clear it or time out */
  TimeOut = USER_TIMEOUT;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_SLAVE_ACK_FAILURE))&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable DMA TX Channel */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
  
  /* Wait until I2Cx_DMA_STREAM_TX disabled or time out */
  TimeOut = USER_TIMEOUT;
  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= DISABLE)&&(TimeOut != 0x00))
  {}  
  if(TimeOut == 0)
  {
    TimeOut_UserCallback();
  }
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2Cx,DISABLE);
  
    /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
  
  if (Buffercmp(TxBuffer, RxBuffer, RXBUFFERSIZE) == PASSED)
  {
    /* LED2, LED3 and LED4 are On */
    STM_EVAL_LEDOn(LED2);
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
  }
  else 
  {   /* LED2, LED3 and LED4 are Off */
    STM_EVAL_LEDOff(LED2);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
  } 
  
#endif /* I2C_SLAVE */
  while(1)
  {}
}

/**
  * @brief  Enables the I2C Clock and configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void I2C_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
    
  /* RCC Configuration */
  /*I2C Peripheral clock enable */
  RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);
  
  /*SDA GPIO clock enable */
  RCC_AHB1PeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);
  
  /*SCL GPIO clock enable */
  RCC_AHB1PeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);
  
  /* Reset I2Cx IP */
  RCC_APB1PeriphResetCmd(I2Cx_CLK, ENABLE);
  
  /* Release reset signal of I2Cx IP */
  RCC_APB1PeriphResetCmd(I2Cx_CLK, DISABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(DMAx_CLK, ENABLE);
  
  /* GPIO Configuration */
  /*Configure I2C SCL pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);
  
  /*Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);
    
  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, I2Cx_SCL_AF);
  
  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, I2Cx_SDA_AF);

  /* DMA Configuration */
  /* Clear any pending flag on Tx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
                                       
  /* Clear any pending flag on Rx Stream  */
  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
  
  /* Disable the I2C Tx DMA stream */
  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
  /* Configure the DMA stream for the I2C peripheral TX direction */
  DMA_DeInit(I2Cx_DMA_STREAM_TX);
  
  /* Disable the I2C Rx DMA stream */
  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
  /* Configure the DMA stream for the I2C peripheral RX direction */
  DMA_DeInit(I2Cx_DMA_STREAM_RX);
  
  /* Initialize the DMA_Channel member */
  DMA_InitStructure.DMA_Channel = I2Cx_DMA_CHANNEL;
  
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = I2Cx_DR_ADDRESS;
  
  /* Initialize the DMA_PeripheralInc member */         
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  
  /* Initialize the DMA_FIFOMode member */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  
  /* Initialize the DMA_FIFOThreshold member */
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  
  /* Initialize the DMA_MemoryBurst member */
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  
  /* Initialize the DMA_PeripheralBurst member */
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
  /* Init DMA for Reception */
   /* Initialize the DMA_DIR member */
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
   /* Initialize the DMA_Memory0BaseAddr member */
   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RxBuffer;
   /* Initialize the DMA_BufferSize member */
   DMA_InitStructure.DMA_BufferSize = RXBUFFERSIZE;
   DMA_DeInit(I2Cx_DMA_STREAM_RX);
   DMA_Init(I2Cx_DMA_STREAM_RX, &DMA_InitStructure);
  
  /* Init DMA for Transmission */
   /* Initialize the DMA_DIR member */
   DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
   /* Initialize the DMA_Memory0BaseAddr member */
   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuffer;
   /* Initialize the DMA_BufferSize member */
   DMA_InitStructure.DMA_BufferSize = TXBUFFERSIZE;
   DMA_DeInit(I2Cx_DMA_STREAM_TX);
   DMA_Init(I2Cx_DMA_STREAM_TX, &DMA_InitStructure);
    
  /* I2C ENABLE */
  I2C_Cmd(I2Cx, ENABLE);
}

/**
  * @brief  Configure a SysTick Base time to 10 ms.
  * @param  None
  * @retval None
  */
static void SysTickConfig(void)
{
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
  /* Configure the SysTick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  
  return PASSED;
}

/**
  * @brief  Fills buffer.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @retval None
  */
static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLength)
{
  uint16_t index = 0;
  
  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = 0x00;
  }
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static void TimeOut_UserCallback(void)
{
  /* User can add his own implementation to manage TimeOut Communication failure */
  /* Block communication and all processes */
  while (1)
  {   
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
