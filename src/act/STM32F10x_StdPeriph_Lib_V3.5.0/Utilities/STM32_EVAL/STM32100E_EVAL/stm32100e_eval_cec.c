/**
  ******************************************************************************
  * @file    stm32100e_eval_cec.c
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file provides all the STM32100E-EVAL HDMI-CEC firmware functions.
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
#include "stm32100e_eval_cec.h"

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
  * @brief This file includes the CEC Stack driver for HDMI-CEC Module
  *        of STM32100E-EVAL board.
  * @{
  */

/** @defgroup STM32100E_EVAL_CEC_Private_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup STM32100E_EVAL_CEC_Private_Defines
  * @{
  */


/**
  * @}
  */

/** @defgroup STM32100E_EVAL_CEC_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32100E_EVAL_CEC_Private_Variables
  * @{
  */

__IO uint32_t ReceivedFrame = 0;
__IO uint32_t SendFrame = 0;
__IO uint32_t BufferCount = 0, TxCounter = 0, RxCounter = 0;
__IO uint8_t BufferPointer[15];
__IO uint32_t ReceiveStatus = 0;
__IO uint32_t SendStatus = 0;
__IO uint8_t TransErrorCode = 0;
__IO uint8_t RecepErrorCode = 0;
__IO uint8_t MyLogicalAddress = 0;
__IO uint16_t MyPhysicalAddress = 0;
__IO uint8_t DeviceType = 0;
#ifdef HDMI_CEC_USE_DDC
__IO uint8_t pBuffer[256];
__IO uint16_t NumByteToRead = 255;
#endif
__IO uint8_t CECDevicesNumber = 0;

HDMI_CEC_Message HDMI_CEC_TX_MessageStructPrivate;
HDMI_CEC_Message HDMI_CEC_RX_MessageStructPrivate;
HDMI_CEC_Message HDMI_CEC_TX_MessageStructure;

__IO uint8_t FeatureOpcode = 0;
__IO uint8_t AbortReason = 0;
__IO uint8_t DeviceCount = 0;

HDMI_CEC_Map HDMI_CEC_MapStruct;
HDMI_CEC_Map HDMI_CEC_DeviceMap[14];

/* CEC follower addresses */
uint8_t* HDMI_CEC_Follower_String[13][2] =
  {
    {(uint8_t*)"         TV         ", (uint8_t*)"0"},
    {(uint8_t*)"Recording Device 1  ", (uint8_t*)"0"},
    {(uint8_t*)"Recording Device 2  ", (uint8_t*)"0"},
    {(uint8_t*)"    Tuner 1         ", (uint8_t*)"0"},
    {(uint8_t*)" Playback Device 1  ", (uint8_t*)"0"},
    {(uint8_t*)"   Audio System     ", (uint8_t*)"0"},
    {(uint8_t*)"    Tuner 2         ", (uint8_t*)"0"},
    {(uint8_t*)"    Tuner 3         ", (uint8_t*)"0"},
    {(uint8_t*)" Playback Device 2  ", (uint8_t*)"0"},
    {(uint8_t*)"Recording Device 3  ", (uint8_t*)"0"},
    {(uint8_t*)"      Tuner 4       ", (uint8_t*)"0"},
    {(uint8_t*)" Playback Device 3  ", (uint8_t*)"0"},
    {(uint8_t*)"     Broadcast      ", (uint8_t*)"1"}
  };

/**
  * @}
  */


/** @defgroup STM32100E_EVAL_CEC_Private_Function_Prototypes
  * @{
  */
static HDMI_CEC_Error PhysicalAddressDiscovery(void);
static HDMI_CEC_Error LogicalAddressAllocation(void);


/**
  * @}
  */


/** @defgroup STM32100E_EVAL_CEC_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the HDMI CEC.
  * @param  None
  * @retval HDMI_CEC_Error: CEC Error code
  */
HDMI_CEC_Error HDMI_CEC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  CEC_InitTypeDef CEC_InitStructure;
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
  uint8_t sendcount = 0;

#ifdef HDMI_CEC_USE_DDC
  I2C_InitTypeDef  I2C_InitStructure;
  /* Enable CEC_I2C clocks */
  RCC_APB1PeriphClockCmd(HDMI_CEC_I2C_CLK, ENABLE);

  /* Enable CEC_I2C_GPIO and CEC_HPD_GPIO clocks */
  RCC_APB2PeriphClockCmd(HDMI_CEC_I2C_GPIO_CLK | HDMI_CEC_HPD_GPIO_CLK, ENABLE);
#endif

  /* Enable CEC clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CEC, ENABLE);

  /* Enable CEC_LINE_GPIO clocks */
  RCC_APB2PeriphClockCmd(HDMI_CEC_LINE_GPIO_CLK, ENABLE);

  /* Configure CEC_LINE_GPIO as Output open drain */
  GPIO_InitStructure.GPIO_Pin = HDMI_CEC_LINE_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(HDMI_CEC_LINE_GPIO_PORT, &GPIO_InitStructure);

#ifdef HDMI_CEC_USE_DDC
  /* Configure CEC_I2C_SCL_PIN and CEC_I2C_SDA_PIN  as Output open drain */
  GPIO_InitStructure.GPIO_Pin = HDMI_CEC_I2C_SCL_PIN | HDMI_CEC_I2C_SDA_PIN;
  GPIO_Init(HDMI_CEC_I2C_GPIO_PORT, &GPIO_InitStructure);

/* This configuration is only when the HDMI CEC is configured as source.
   The HDMI source has to provide the +5V Power signal to the sink. 
   On STM32100E-EVAL borad, you have to solder the SB4 Solder bridge.
   Then, the source will wait for HPD signal to be asserted from the sink.
   Once the HPD signal is detected the source shall read the EDID structure 
   throuhgh the DDC channel. */
  /* Configure CEC_HPD_GPIO as Input pull down */
  GPIO_InitStructure.GPIO_Pin = HDMI_CEC_HPD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(HDMI_CEC_HPD_GPIO_PORT, &GPIO_InitStructure);


/* This configuration is only when the HDMI CEC is configured as sink.
   The HDMI sink has to wait for the +5V Power signal from the source. 
   On STM32100E-EVAL borad, SB4 Solder bridge should be open (default configuration).
   Then, the sink will assert the HPD signal to inform the source that the EDID
   is ready for read through DDC channel. In this implementation, the EDID structure
   is not implemented. */
/*  GPIO_InitStructure.GPIO_Pin = HDMI_CEC_HPD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(HDMI_CEC_HPD_GPIO_PORT, &GPIO_InitStructure);

  HDMI_CEC_HPD_HIGH(); // Set the Hot plug detect signal */
  
  /* Enable CEC_I2C */
  I2C_Cmd(HDMI_CEC_I2C, ENABLE);

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = HDMI_CEC_I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = HDMI_CEC_I2C_CLOCK_SPEED;
  I2C_Init(HDMI_CEC_I2C, &I2C_InitStructure);
#endif

  /* Physical Address discovery */
  errorstatus = PhysicalAddressDiscovery();

  if (errorstatus != HDMI_CEC_OK)
  {
    /* Device not connected (Physical Address lost) */
    return(errorstatus);
  }


  /* CEC DeInit */
  CEC_DeInit();

  /* Configure CEC */
  CEC_InitStructure.CEC_BitTimingMode = CEC_BitTimingStdMode;
  CEC_InitStructure.CEC_BitPeriodMode = CEC_BitPeriodStdMode;
  CEC_Init(&CEC_InitStructure);

  /* Set Prescaler value for APB1 clock = 24MHz */
  CEC_SetPrescaler(0x4AF);

  /* Enable CEC */
  CEC_Cmd(ENABLE);

  /* Logical Address Allocation */
  sendcount = 0;
  errorstatus = LogicalAddressAllocation();

  while ((errorstatus != HDMI_CEC_OK) && sendcount < 0x5)
  {
    sendcount++;
    errorstatus = LogicalAddressAllocation();
  }

  if (errorstatus != HDMI_CEC_OK)
  {
    /* Device Unregistred */
    return(errorstatus);
  }

  HDMI_CEC_CheckConnectedDevices();
  
  /* Set the CEC initiator address */
  CEC_OwnAddressConfig(MyLogicalAddress);

  /* Activate CEC interrupts associated to the set of RBTF,RERR, TBTF, TERR flags */
  CEC_ITConfig(ENABLE);

  /* Report physical address*/
  errorstatus = HDMI_CEC_ReportPhysicalAddress();
  sendcount = 0;

  while ((errorstatus != HDMI_CEC_OK) && sendcount < 0x5)
  {
    sendcount++;
    errorstatus = HDMI_CEC_ReportPhysicalAddress();
  }

  if (errorstatus != HDMI_CEC_OK)
  {
    /* Device Unregistred */
    return(errorstatus);
  }

  return errorstatus;
}

/**
  * @brief  Transmit message by taking  data from typedef struct CEC_Meassage
  * @param  CEC_TX_MessageStructure: pointer to an CEC_Message structure that contains
  *         the message to be sent.
  * @retval HDMI_CEC_Error: CEC Error code
  */
HDMI_CEC_Error HDMI_CEC_TransmitMessage(HDMI_CEC_Message *HDMI_CEC_TX_MessageStructure)
{
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
  __IO uint32_t count = 0, j = 0;

  SendFrame = 0;
  SendStatus = 0;
  TxCounter = 0;
  BufferCount = 0;

  HDMI_CEC_TX_MessageStructPrivate =  *HDMI_CEC_TX_MessageStructure;

  /* Initialize BufferPointer */
  for (j = 0; j < 15; j++)
  {
    BufferPointer[j] = 0;
  }

  BufferPointer[0] =  HDMI_CEC_TX_MessageStructPrivate.Opcode;

  for (BufferCount = 1; BufferCount < HDMI_CEC_TX_MessageStructPrivate.TxMessageLength + 1; BufferCount++)
  {
    BufferPointer[BufferCount] =  HDMI_CEC_TX_MessageStructPrivate.Operande[BufferCount-1];
  }

  CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

  /* Write single Data in the TX Buffer to Transmit through the CEC peripheral */
  CEC_SendDataByte(HDMI_CEC_TX_MessageStructPrivate.Header);

  /* Initiate Message Transmission */
  CEC_StartOfMessage();

  while ((SendFrame == 0) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }
  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  if (SendStatus == 0)
  {
    errorstatus = (HDMI_CEC_Error) TransErrorCode;
  }

  return errorstatus;
}


/**
  * @brief  Get the ESR register status.
  * @param  None
  * @retval HDMI_CEC_Error: CEC Error code
  */
HDMI_CEC_Error HDMI_CEC_GetErrorStatus (void)
{
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;

  /* Bit timing error case*/
  if (CEC_GetFlagStatus(CEC_FLAG_BTE) != RESET)
  {
    errorstatus = HDMI_CEC_BIT_TIMING;
  }
  /* Bit period error case */
  if (CEC_GetFlagStatus(CEC_FLAG_BPE) != RESET)
  {
    errorstatus = HDMI_CEC_BIT_PERIOD;
  }
  /* Recieve error case */
  if (CEC_GetFlagStatus(CEC_FLAG_RBTFE) != RESET)
  {
    errorstatus = HDMI_CEC_RX_BLOCK_FINISHED;
  }
  /* Start bit error case*/
  if (CEC_GetFlagStatus(CEC_FLAG_SBE) != RESET)
  {
    errorstatus = HDMI_CEC_START_BIT;
  }
  /* Acknowledge error case*/
  if (CEC_GetFlagStatus(CEC_FLAG_ACKE) != RESET)
  {
    errorstatus = HDMI_CEC_BLOCK_ACKNOWLEDGE;
  }
  /* Line error case */
  if (CEC_GetFlagStatus(CEC_FLAG_LINE) != RESET)
  {
    errorstatus = HDMI_CEC_LINE;
  }
  /* Transfert error case*/
  if (CEC_GetFlagStatus(CEC_FLAG_TBTFE) != RESET)
  {
    errorstatus = HDMI_CEC_TX_BLOCK_FINISHED;
  }
  /* Clear All errors */
  CEC_ClearFlag(CEC_FLAG_RERR);
  CEC_ClearFlag(CEC_FLAG_TERR);
  return errorstatus;
}

/**
  * @brief  Allows to process all the interrupts that are high.
  * @param  None
  * @retval None
  */
void HDMI_CEC_ProcessIRQSrc(void)
{
  /********************** Reception *********************************************/
  /* Check if a reception error occured */
  if (CEC_GetFlagStatus(CEC_FLAG_RERR))
  {
    /* Set receive status bit (Error) */
    ReceiveStatus  = 0;
    ReceivedFrame  = 1;
    RecepErrorCode = HDMI_CEC_GetErrorStatus();
    CEC_ClearFlag(CEC_FLAG_RERR | CEC_FLAG_RSOM | CEC_FLAG_REOM  | CEC_FLAG_RBTF);
  }
  else if (CEC_GetFlagStatus(CEC_FLAG_RBTF))
  {
    /* Check if the byte received is the last one of the message */
    if (CEC_GetFlagStatus(CEC_FLAG_REOM))
    {
      HDMI_CEC_RX_MessageStructPrivate.Operande[RxCounter-1] = CEC_ReceiveDataByte();
      HDMI_CEC_RX_MessageStructPrivate.RxMessageLength = RxCounter;
      ReceiveStatus = SUCCESS;
      ReceivedFrame = 1;
    }
    /* Check if the byte received is a Header */
    else if (CEC_GetFlagStatus(CEC_FLAG_RSOM))
    {
      ReceiveStatus = 0;
      HDMI_CEC_RX_MessageStructPrivate.Header = CEC_ReceiveDataByte();
      RxCounter = 0;
    }
    /* Receive each byte except header in the reception buffer */
    else
    {
      if (RxCounter != 0)
      {
        HDMI_CEC_RX_MessageStructPrivate.Operande[RxCounter-1] = CEC_ReceiveDataByte();
        RxCounter++;
      }
      else
      {
        HDMI_CEC_RX_MessageStructPrivate.Opcode = CEC_ReceiveDataByte();
        RxCounter++;
      }
      
    }
    /* Clear all reception flags */
    CEC_ClearFlag(CEC_FLAG_RSOM | CEC_FLAG_REOM  | CEC_FLAG_RBTF);
  }

  /********************** Transmission ******************************************/
  /* Check if a transmission error occured */
  if (CEC_GetFlagStatus(CEC_FLAG_TERR))
  {
    TransErrorCode = HDMI_CEC_GetErrorStatus();
    CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);
    SendFrame = 1;
    SendStatus = 0;
  }
  /* Check if end of message bit is set in the data to be transmitted */
  else if (CEC_GetFlagStatus(CEC_FLAG_TEOM))
  {
    CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_RBTF);
    CEC_EndOfMessageCmd(DISABLE);
    SendFrame = 1;
    SendStatus = SUCCESS;
  }
  /* Check if data byte has been sent */
  else if (CEC_GetFlagStatus(CEC_FLAG_TBTRF))
  {
    /* Set EOM bit if the byte to be transmitted is the last one of the TransmitBuffer */
    if (TxCounter == (HDMI_CEC_TX_MessageStructPrivate.TxMessageLength))
    {
      CEC_SendDataByte(BufferPointer[TxCounter]);
      TxCounter++;
      CEC_ClearFlag(CEC_FLAG_TBTRF);
      CEC_EndOfMessageCmd(ENABLE);
    }
    else
    {
      /* Put the byte in the TX Buffer */
      CEC_SendDataByte(BufferPointer[TxCounter]);
      TxCounter++;
      CEC_ClearFlag(CEC_FLAG_TBTRF);
    }
  }
}

/**
  * @brief  Report physical address to all other devices thus allowing any
            device to create a map of the network.
  * @param  None
  * @retval HDMI_CEC_Error: CEC Error code.
  */
HDMI_CEC_Error HDMI_CEC_ReportPhysicalAddress(void)
{
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
  HDMI_CEC_Message HDMI_CEC_TX_Message;

  HDMI_CEC_TX_Message.Header = ((MyLogicalAddress << 4) | 0xF);
  HDMI_CEC_TX_Message.Opcode = HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
  HDMI_CEC_TX_Message.Operande[0] = MyPhysicalAddress >> 8;
  HDMI_CEC_TX_Message.Operande[1] = MyPhysicalAddress & 0xFF;
  HDMI_CEC_TX_Message.Operande[2] = DeviceType;
  HDMI_CEC_TX_Message.TxMessageLength = 0x03;

  errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_Message);

  return  errorstatus;
}

/**
  * @brief  Handle CEC command receive callback.
  *         When receiving the STANDBY Opcode commande, the system is entred in
  *         Stop mode and when wakeup, the PLL is configured as system clock and
  *         the HSI is selected as PLL source.      
  * @param  None
  * @retval None
  */
void HDMI_CEC_CommandCallBack(void)
{
  uint8_t i = 0, sendcount = 0;
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
  EXTI_InitTypeDef EXTI_InitStructure;

  switch (HDMI_CEC_RX_MessageStructPrivate.Opcode)
  {
    case HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:
      HDMI_CEC_MapStruct.PhysicalAddress_A = HDMI_CEC_RX_MessageStructPrivate.Operande[1] >> 4;
      HDMI_CEC_MapStruct.PhysicalAddress_B = HDMI_CEC_RX_MessageStructPrivate.Operande[1] & 0x0F;
      HDMI_CEC_MapStruct.PhysicalAddress_C = HDMI_CEC_RX_MessageStructPrivate.Operande[0] >> 4;
      HDMI_CEC_MapStruct.PhysicalAddress_D = HDMI_CEC_RX_MessageStructPrivate.Operande[0] & 0x0F;
      HDMI_CEC_MapStruct.LogicalAddress = (HDMI_CEC_RX_MessageStructPrivate.Header >> 0x4) & 0x0F;
      HDMI_CEC_MapStruct.DeviceType = HDMI_CEC_RX_MessageStructPrivate.Operande[2];
      HDMI_CEC_DeviceMap[DeviceCount] = HDMI_CEC_MapStruct;
      HDMI_CEC_Follower_String[(HDMI_CEC_DeviceMap[DeviceCount].LogicalAddress)][1] = (uint8_t*)"1";
      DeviceCount++;
      break;

    case HDMI_CEC_OPCODE_STANDBY:
      /* CEC Line */
      GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
      /* Configure the CEC Line as EXTI Line on Falling Edge */
      EXTI_ClearITPendingBit(EXTI_Line8);
      EXTI_InitStructure.EXTI_Line = EXTI_Line8;
      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);
      /* Request to enter Stop mode */
      PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);

      /* Disable the CEC EXTI Line */
      EXTI_InitStructure.EXTI_LineCmd = DISABLE;
      EXTI_Init(&EXTI_InitStructure);
      /* Configure the PLL Source */
      RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_6);

      /* Enable PLL */ 
      RCC_PLLCmd(ENABLE);

      /* Wait till PLL is ready */
      while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
      {
      }

      /* Select PLL as system clock source */
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

      /* Wait till PLL is used as system clock source */
      while(RCC_GetSYSCLKSource() != 0x08)
      {
      }
      break;

    case HDMI_CEC_OPCODE_GET_CEC_VERSION:
      /* Send the Used CEC version */
      HDMI_CEC_TX_MessageStructPrivate.Header = ((MyLogicalAddress << 4) | HDMI_CEC_RX_MessageStructPrivate.Header >> 4);
      HDMI_CEC_TX_MessageStructPrivate.Opcode = HDMI_CEC_OPCODE_CEC_VERSION;
      HDMI_CEC_TX_MessageStructPrivate.Operande[0] = HDMI_CEC_VERSION; /* CEC Version */
      HDMI_CEC_TX_MessageStructPrivate.TxMessageLength = 0x01;
      errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_MessageStructPrivate);

      /* Retransmit message until 5 time */
      while ((errorstatus != HDMI_CEC_OK) && sendcount < 0x5)
      {
        sendcount++;
        errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_MessageStructPrivate);
      }
      break;

    case HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
      /* Send the Physical address */
      errorstatus = HDMI_CEC_ReportPhysicalAddress();
      sendcount = 0;
      /* Retransmit message until 5 time */
      while ((errorstatus != HDMI_CEC_OK) && sendcount < 0x5)
      {
        sendcount++;
        errorstatus = HDMI_CEC_ReportPhysicalAddress();
      }
      break;

    case HDMI_CEC_OPCODE_FEATURE_ABORT:
      /* The device doesn't support the requested message type, or that it cannot
      execute it at the present time. */
      FeatureOpcode = HDMI_CEC_RX_MessageStructPrivate.Operande[0];
      AbortReason = HDMI_CEC_RX_MessageStructPrivate.Operande[1];
      break;

    case HDMI_CEC_OPCODE_GIVE_OSD_NAME:
      /* Send the OSD name = STM32100E CEC*/
      HDMI_CEC_TX_MessageStructPrivate.Header = ((MyLogicalAddress << 4) | HDMI_CEC_RX_MessageStructPrivate.Header >> 4);
      HDMI_CEC_TX_MessageStructPrivate.Opcode = HDMI_CEC_OPCODE_SET_OSD_NAME;
      /* STM32100E*/
      HDMI_CEC_TX_MessageStructPrivate.Operande[0] = 0x53;
      HDMI_CEC_TX_MessageStructPrivate.Operande[1] = 0x54;
      HDMI_CEC_TX_MessageStructPrivate.Operande[2] = 0x4D;
      HDMI_CEC_TX_MessageStructPrivate.Operande[3] = 0x33;
      HDMI_CEC_TX_MessageStructPrivate.Operande[4] = 0x32;
      HDMI_CEC_TX_MessageStructPrivate.Operande[5] = 0x31;
      HDMI_CEC_TX_MessageStructPrivate.Operande[6] = 0x30;
      HDMI_CEC_TX_MessageStructPrivate.Operande[7] = 0x30;
      HDMI_CEC_TX_MessageStructPrivate.Operande[8] = 0x45;
      HDMI_CEC_TX_MessageStructPrivate.Operande[9] = 0x20;
      /* CEC */
      HDMI_CEC_TX_MessageStructPrivate.Operande[10] = 0x43;
      HDMI_CEC_TX_MessageStructPrivate.Operande[11] = 0x45;
      HDMI_CEC_TX_MessageStructPrivate.Operande[12] = 0x43;
      HDMI_CEC_TX_MessageStructPrivate.TxMessageLength = 13;
      errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_MessageStructPrivate);
      sendcount = 0;
      /* Retransmit message until 5 time */
      while ((errorstatus != HDMI_CEC_OK) && sendcount < 0x5)
      {
        sendcount++;
        errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_MessageStructPrivate);
      }
      break;

    case HDMI_CEC_OPCODE_ROUTING_CHANGE:
      for (i = 0;i < 0x14;i++)
      {
        if ((HDMI_CEC_DeviceMap[i].PhysicalAddress_A == HDMI_CEC_RX_MessageStructPrivate.Operande[1] >> 4) &&
            (HDMI_CEC_DeviceMap[i].PhysicalAddress_B == HDMI_CEC_RX_MessageStructPrivate.Operande[1]&0x0F) &&
            (HDMI_CEC_DeviceMap[i].PhysicalAddress_C == HDMI_CEC_RX_MessageStructPrivate.Operande[0] >> 4) &&
            (HDMI_CEC_DeviceMap[i].PhysicalAddress_D == HDMI_CEC_RX_MessageStructPrivate.Operande[0]&0x0F))
        {
          HDMI_CEC_MapStruct.LogicalAddress = (HDMI_CEC_RX_MessageStructPrivate.Header >> 0x4) & 0x0F;
          HDMI_CEC_MapStruct.DeviceType = HDMI_CEC_RX_MessageStructPrivate.Operande[2];
          HDMI_CEC_DeviceMap[i] = HDMI_CEC_MapStruct;
        }
      }
      break;

    default:
      /* Send Abort feature*/
      HDMI_CEC_TX_MessageStructPrivate.Header = ((MyLogicalAddress << 4) | HDMI_CEC_RX_MessageStructPrivate.Header >> 4);
      HDMI_CEC_TX_MessageStructPrivate.Opcode = HDMI_CEC_OPCODE_FEATURE_ABORT;
      HDMI_CEC_TX_MessageStructPrivate.Operande[0] = 0x02; /* defines command to be performed */
      HDMI_CEC_TX_MessageStructPrivate.Operande[1] = HDMI_CEC_REFUSED; /* Reason for abort feature */
      HDMI_CEC_TX_MessageStructPrivate.TxMessageLength = 0x02;
      errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_MessageStructPrivate);
      sendcount = 0;
      /* Retransmit message until 5 time */
      while ((errorstatus != HDMI_CEC_OK) && sendcount < 0x5)
      {
        sendcount++;
        errorstatus = HDMI_CEC_TransmitMessage(&HDMI_CEC_TX_MessageStructPrivate);
      }
      break;

  }
}

/**
  * @brief  Check the connected CEC devices.
  * @param  None
  * @retval HDMI_CEC_Error
  */
HDMI_CEC_Error HDMI_CEC_CheckConnectedDevices(void)
{
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
  uint32_t count = 0, i = 1;
 
  /*----------------------------- TV device  ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x0);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[0][1] = (uint8_t*)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }

  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Recording device 1 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x1);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[1][1] = (uint8_t*)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }

  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);
  /*----------------------------- Recording device 2 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x2);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[2][1] = (uint8_t*)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }

  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Tuner 1 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x3);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[3][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);
  /*----------------------------- Playback device 1 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x4);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[4][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Audio system ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x5);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[5][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Tuner 2 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x6);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[6][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Tuner 3 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x7);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[7][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Playback device 2 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x8);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[8][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  /*----------------------------- Recording device 3 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0x9);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[9][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);
  /*----------------------------- Tuner 4 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0xA);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[10][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);
  /*----------------------------- Playback device 3 ---------------------------*/
  CEC_OwnAddressConfig(MyLogicalAddress); /* Own address = MyLogicalAddress */

  CEC_EndOfMessageCmd(ENABLE);

  CEC_SendDataByte((MyLogicalAddress << 4) | 0xB);

  /* Start of message */
  CEC_StartOfMessage();

  /* Wait till the header message is sent */
  while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
  {
    count++;
  }

  if (count >= HDMI_CEC_TIMEOUT_VALUE)
  {
    errorstatus = HDMI_CEC_TIMEOUT;
    return(errorstatus);
  }

  errorstatus = HDMI_CEC_GetErrorStatus();

  if (errorstatus == HDMI_CEC_OK)
  {
    HDMI_CEC_Follower_String[11][1] = (uint8_t *)"1";
    i++;
    errorstatus = HDMI_CEC_OK;
  }
  /* Clear CEC CSR register */
  CEC_ClearFlag(CEC_FLAG_TBTRF);
  CEC_EndOfMessageCmd(DISABLE);

  CECDevicesNumber = i - 1;

  return errorstatus;
}

/**
  * @brief  Physical address discovery.
  * @param  None
  * @retval HDMI_CEC_Error: CEC Error code.
  */
static HDMI_CEC_Error PhysicalAddressDiscovery(void)
{
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
#ifdef HDMI_CEC_USE_DDC
  uint32_t index = 0, i = 0;
#endif

  /*------------------------------ Physical address discovery -----------------*/
  if (HDMI_CEC_ROOT == 0x1)
  {
    MyPhysicalAddress = 0x0000;
    /* The HDMI-CEC here is configured as sink or as a repeater. The configuration
       of the +5V power signal and the HPD should be well configured.
       Implement here the EDID Structure to be sent to the HDMI source.
       For more details please refer to the HDMI specification. 
       The EDID structure should be sent to the device source using the DDC Channel
       and using the HPD signal. */
  }
  else
  {
  
#ifdef HDMI_CEC_USE_DDC  
    /* The HDMI-CEC here is configured as source or as a repeater. The configuration
       of the +5V power signal and the HPD should be well configured.
       The source should wait for HPD and then read the EDID structure. */
    while(GPIO_ReadInputDataBit(HDMI_CEC_HPD_GPIO_PORT, HDMI_CEC_HPD_PIN) == RESET)
    {
    }
    /* Wait for 100 ms after HPD was received */
    for(i = 0; i < 0x5FFFF; i++)
    {
    }

    /* Return the physical address using the I2C  by reading the 2 bytes 24 and
      25 form the EDID */
    /* Read the EDID Block 0 and EDID Block 1 at address 0xA0 */
    /*!< While the bus is busy */
    while(I2C_GetFlagStatus(HDMI_CEC_I2C, I2C_FLAG_BUSY))
    {
    }
  
    /*!< Send START condition */
    I2C_GenerateSTART(HDMI_CEC_I2C, ENABLE);
  
    /*!< Test on EV5 and clear it */
    while(!I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
    }
   
    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(HDMI_CEC_I2C, 0xA0, I2C_Direction_Transmitter);
 

    /*!< Test on EV6 and clear it */
    while(!I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
    }

    /*!< Send the EEPROM's internal address to read from: Only one byte address */
    I2C_SendData(HDMI_CEC_I2C, 0x00);  

    /*!< Test on EV8 and clear it */
    while(!I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
    }
  
    /*!< Send STRAT condition a second time */  
    I2C_GenerateSTART(HDMI_CEC_I2C, ENABLE);
  
    /*!< Test on EV5 and clear it */
    while(!I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {
    }
  
    /*!< Send EEPROM address for read */
    I2C_Send7bitAddress(HDMI_CEC_I2C, 0xA1, I2C_Direction_Receiver);
  
    /*!< Test on EV6 and clear it */
    while(!I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
    }

    /* While there is data to be read */
    while (NumByteToRead-- > 1)
    {
      while(I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
      {
      }
      for(i = 0; i < 0xFFF; i++)
      {
      }
      pBuffer[index++] = I2C_ReceiveData(HDMI_CEC_I2C);
    }

    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(HDMI_CEC_I2C, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(HDMI_CEC_I2C, ENABLE);

    while(I2C_CheckEvent(HDMI_CEC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));
    pBuffer[index] = I2C_ReceiveData(HDMI_CEC_I2C);

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(HDMI_CEC_I2C, ENABLE);
    MyPhysicalAddress = ((pBuffer[138] << 8) | pBuffer[137]);
#else
    MyPhysicalAddress = 0x1000;
#endif
  }

  return errorstatus;
}
/**
  * @brief  Allocate the logical address.
  * @param  None
  * @retval HDMI_CEC_Error: CEC Error code.
  */
static HDMI_CEC_Error LogicalAddressAllocation(void)
{
  HDMI_CEC_Error errorstatus = HDMI_CEC_OK;
  uint32_t count = 0;

  /*------------------ Logical address allocation -----------------------------*/
  /* Get the device type */
  /* Device type = CEC_TV */
  if (DeviceType == HDMI_CEC_TV)
  {
    if (HDMI_CEC_ROOT)
    {
      MyLogicalAddress = 0x00;
    }
    else
    {
      CEC_OwnAddressConfig(0xE); /* Own address = 0xE */

      CEC_EndOfMessageCmd(ENABLE);

      CEC_SendDataByte(0xEE);

      /* Start of message */
      CEC_StartOfMessage();

      /* Wait till the polling message is sent */
      while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
      {
        count++;
      }

      if (count >= HDMI_CEC_TIMEOUT_VALUE)
      {
        errorstatus = HDMI_CEC_TIMEOUT;
        return(errorstatus);
      }

      errorstatus = HDMI_CEC_GetErrorStatus();

      if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
      {
        MyLogicalAddress = 0x0E;
        errorstatus = HDMI_CEC_OK;
      }
      else if (errorstatus == HDMI_CEC_OK)
      {
        MyLogicalAddress = 0x0F;
        errorstatus = HDMI_CEC_DEVICE_UNREGISTRED;
      }
    }
    /* Clear CEC CSR register */
    CEC_ClearFlag(CEC_FLAG_TBTRF);
    CEC_EndOfMessageCmd(DISABLE);
  }

  /* Device type = CEC_RECORDING */
  if (DeviceType == HDMI_CEC_RECORDING)
  {
    CEC_OwnAddressConfig(0x1); /* Own address = 0x1 */

    CEC_EndOfMessageCmd(ENABLE);

    CEC_SendDataByte(0x11);

    /* Start of message */
    CEC_StartOfMessage();

    /* Wait till the header message is sent */
    while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
    {
      count++;
    }

    if (count >= HDMI_CEC_TIMEOUT_VALUE)
    {
      errorstatus = HDMI_CEC_TIMEOUT;
      return(errorstatus);
    }

    errorstatus = HDMI_CEC_GetErrorStatus();

    if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
    {
      MyLogicalAddress = 0x01;
      errorstatus = HDMI_CEC_OK;
    }
    else if (errorstatus == HDMI_CEC_OK)
    {
      /* Clear CEC CSR register */
      CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

      CEC_EndOfMessageCmd(DISABLE);

      CEC_OwnAddressConfig(0x2); /* Own address = 0x2 */

      CEC_EndOfMessageCmd(ENABLE);

      CEC_SendDataByte(0x22);

      /* Start of message */
      CEC_StartOfMessage();

      count = 0;

      /* Wait till the header message is sent */
      while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
      {
        count++;
      }

      if (count >= HDMI_CEC_TIMEOUT_VALUE)
      {
        errorstatus = HDMI_CEC_TIMEOUT;
        return(errorstatus);
      }
      errorstatus = HDMI_CEC_GetErrorStatus();

      if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
      {
        MyLogicalAddress = 0x02;
        errorstatus = HDMI_CEC_OK;
      }
      else if (errorstatus == HDMI_CEC_OK)
      {
        /* Clear CEC CSR register */
        CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

        CEC_EndOfMessageCmd(DISABLE);

        CEC_OwnAddressConfig(0x9); /* Own address = 0x9 */

        CEC_EndOfMessageCmd(ENABLE);

        CEC_SendDataByte(0x99);

        /* Start of message */
        CEC_StartOfMessage();

        count = 0;

        /* Wait till the header message is sent */
        while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
        {
          count++;
        }

        if (count >= HDMI_CEC_TIMEOUT_VALUE)
        {
          errorstatus = HDMI_CEC_TIMEOUT;
          return(errorstatus);
        }
        errorstatus = HDMI_CEC_GetErrorStatus();

        if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
        {
          MyLogicalAddress = 0x09;
          errorstatus = HDMI_CEC_OK;
        }
        else if (errorstatus == HDMI_CEC_OK)
        {
          MyLogicalAddress = 0x0F;
          errorstatus = HDMI_CEC_DEVICE_UNREGISTRED;
        }
      }
    }
    /* Clear CEC CSR register */
    CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);
    CEC_EndOfMessageCmd(DISABLE);
  }

  /* Device type = CEC_TUNER */
  if (DeviceType == HDMI_CEC_TUNER)
  {
    CEC_OwnAddressConfig(0x3); /* Own address = 0x3 */

    CEC_EndOfMessageCmd(ENABLE);

    CEC_SendDataByte(0x33);

    /* Start of message */
    CEC_StartOfMessage();

    count = 0;

    /* Wait till the header message is sent */
    while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
    {
      count++;
    }

    if (count >= HDMI_CEC_TIMEOUT_VALUE)
    {
      errorstatus = HDMI_CEC_TIMEOUT;
      return(errorstatus);
    }
    errorstatus = HDMI_CEC_GetErrorStatus();

    if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
    {
      MyLogicalAddress = 0x03;
      errorstatus = HDMI_CEC_OK;
    }
    else if (errorstatus == HDMI_CEC_OK)
    {
      /* Clear CEC CSR register */
      CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

      CEC_EndOfMessageCmd(DISABLE);

      CEC_OwnAddressConfig(0x6); /* Own address = 0x6 */

      CEC_EndOfMessageCmd(ENABLE);

      CEC_SendDataByte(0x66);

      /* Start of message */
      CEC_StartOfMessage();

      count = 0;

      /* Wait till the header message is sent */
      while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
      {
        count++;
      }

      if (count >= HDMI_CEC_TIMEOUT_VALUE)
      {
        errorstatus = HDMI_CEC_TIMEOUT;
        return(errorstatus);
      }
      errorstatus = HDMI_CEC_GetErrorStatus();

      if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
      {
        MyLogicalAddress = 0x06;
        errorstatus = HDMI_CEC_OK;
      }
      else if (errorstatus == HDMI_CEC_OK)
      {
        /* Clear CEC CSR register */
        CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

        CEC_EndOfMessageCmd(DISABLE);

        CEC_OwnAddressConfig(0x7); /* Own address = 0x7 */

        CEC_EndOfMessageCmd(ENABLE);

        CEC_SendDataByte(0x77);

        /* Start of message */
        CEC_StartOfMessage();

        count = 0;

        /* Wait till the header message is sent */
        while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
        {
          count++;
        }

        if (count >= HDMI_CEC_TIMEOUT_VALUE)
        {
          errorstatus = HDMI_CEC_TIMEOUT;
          return(errorstatus);
        }
        errorstatus = HDMI_CEC_GetErrorStatus();

        if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
        {
          MyLogicalAddress = 0x07;
          errorstatus = HDMI_CEC_OK;
        }
        else if (errorstatus == HDMI_CEC_OK)
        {
          /* Clear CEC CSR register */
          CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

          CEC_EndOfMessageCmd(DISABLE);

          CEC_OwnAddressConfig(0xA); /* Own address = 0xA */

          CEC_EndOfMessageCmd(ENABLE);

          CEC_SendDataByte(0xAA);

          /* Start of message */
          CEC_StartOfMessage();

          count = 0;

          /* Wait till the header message is sent */
          while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
          {
            count++;
          }

          if (count >= HDMI_CEC_TIMEOUT_VALUE)
          {
            errorstatus = HDMI_CEC_TIMEOUT;
            return(errorstatus);
          }
          errorstatus = HDMI_CEC_GetErrorStatus();

          if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
          {
            MyLogicalAddress = 0x0A;
            errorstatus = HDMI_CEC_OK;
          }
          else if (errorstatus == HDMI_CEC_OK)
          {
            MyLogicalAddress = 0x0F;
            errorstatus = HDMI_CEC_DEVICE_UNREGISTRED;
          }
        }
      }
    }
    /* Clear CEC CSR register */
    CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);
    CEC_EndOfMessageCmd(DISABLE);
  }

  /* Device type = CEC_PLAYBACK */
  if (DeviceType == HDMI_CEC_PLAYBACK)
  {
    CEC_OwnAddressConfig(0x4); /* Own address = 0x4 */

    CEC_EndOfMessageCmd(ENABLE);

    CEC_SendDataByte(0x44);

    /* Start of message */
    CEC_StartOfMessage();

    count = 0;

    /* Wait till the header message is sent */
    while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
    {
      count++;
    }

    if (count >= HDMI_CEC_TIMEOUT_VALUE)
    {
      errorstatus = HDMI_CEC_TIMEOUT;
      return(errorstatus);
    }

    errorstatus = HDMI_CEC_GetErrorStatus();

    if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
    {
      MyLogicalAddress = 0x04;
      errorstatus = HDMI_CEC_OK;
    }
    else if (errorstatus == HDMI_CEC_OK)
    {
      /* Clear CEC CSR register */
      CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

      CEC_EndOfMessageCmd(DISABLE);

      CEC_OwnAddressConfig(0x8); /* Own address = 0x8 */

      CEC_EndOfMessageCmd(ENABLE);

      CEC_SendDataByte(0x88);

      /* Start of message */
      CEC_StartOfMessage();

      count = 0;

      /* Wait till the header message is sent */
      while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
      {
        count++;
      }

      if (count >= HDMI_CEC_TIMEOUT_VALUE)
      {
        errorstatus = HDMI_CEC_TIMEOUT;
        return(errorstatus);
      }
      errorstatus = HDMI_CEC_GetErrorStatus();

      if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
      {
        MyLogicalAddress = 0x08;
        errorstatus = HDMI_CEC_OK;
      }
      else if (errorstatus == HDMI_CEC_OK)
      {
        /* Clear CEC CSR register */
        CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);

        CEC_EndOfMessageCmd(DISABLE);

        CEC_OwnAddressConfig(0xB); /* Own address = 0xBB */

        CEC_EndOfMessageCmd(ENABLE);

        CEC_SendDataByte(0xBB);

        /* Start of message */
        CEC_StartOfMessage();

        count = 0;

        /* Wait till the header message is sent */
        while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
        {
          count++;
        }

        if (count >= HDMI_CEC_TIMEOUT_VALUE)
        {
          errorstatus = HDMI_CEC_TIMEOUT;
          return(errorstatus);
        }

        errorstatus = HDMI_CEC_GetErrorStatus();

        if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
        {
          MyLogicalAddress = 0x0B;
          errorstatus = HDMI_CEC_OK;
        }
        else if (errorstatus == HDMI_CEC_OK)
        {
          MyLogicalAddress = 0x0F;
          errorstatus = HDMI_CEC_DEVICE_UNREGISTRED;
        }
      }
    }

    /* Clear CEC CSR register */
    CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);
    CEC_EndOfMessageCmd(DISABLE);
  }

  /* Device type = CEC Audio System */
  if (DeviceType == HDMI_CEC_AUDIOSYSTEM)
  {
    CEC_OwnAddressConfig(0x5); /* Own address = 0x5 */

    CEC_EndOfMessageCmd(ENABLE);

    CEC_SendDataByte(0x55);

    /* Start of message */
    CEC_StartOfMessage();

    count = 0;

    /* Wait till the header message is sent */
    while ((CEC_GetFlagStatus(CEC_FLAG_TBTRF) == RESET) && (CEC_GetFlagStatus(CEC_FLAG_TERR) == RESET) && (count < HDMI_CEC_TIMEOUT_VALUE))
    {
      count++;
    }

    if (count >= HDMI_CEC_TIMEOUT_VALUE)
    {
      errorstatus = HDMI_CEC_TIMEOUT;
      return(errorstatus);
    }

    errorstatus = HDMI_CEC_GetErrorStatus();

    if (errorstatus == HDMI_CEC_BLOCK_ACKNOWLEDGE)
    {
      MyLogicalAddress = 0x05;
      errorstatus = HDMI_CEC_OK;
    }
    else if (errorstatus == HDMI_CEC_OK)
    {
      MyLogicalAddress = 0x0F;
      errorstatus = HDMI_CEC_DEVICE_UNREGISTRED;
    }

    /* Clear CEC CSR register */
    CEC_ClearFlag(CEC_FLAG_TBTRF | CEC_FLAG_TERR);
    CEC_EndOfMessageCmd(DISABLE);
  }

  return errorstatus;
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


