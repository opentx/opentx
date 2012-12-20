/**
  ******************************************************************************
  * @file    USART/Smartcard/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body.
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

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_SmartCard
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "smartcard.h"
#include "stm322xg_eval.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Directories & Files ID */
const uint8_t MasterRoot[2] = {0x3F, 0x00};
const uint8_t GSMDir[2] = {0x7F, 0x20};
const uint8_t ICCID[2] = {0x2F, 0xE2};
const uint8_t IMSI[2] = {0x6F, 0x07};
const uint8_t CHV1[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/* APDU Transport Structures */
SC_ADPU_Commands SC_ADPU;
SC_ADPU_Responce SC_Responce;
__IO uint32_t TimingDelay = 0;
static __IO ErrorStatus HSEStartUpStatus = SUCCESS;
__IO uint32_t CardInserted = 0;
uint32_t CHV1Status = 0, i = 0;
__IO uint8_t ICCID_Content[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__IO uint8_t IMSI_Content[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Private function prototypes -----------------------------------------------*/
static void Delay(uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  SC_State SCState = SC_POWER_OFF;
     
  /* Setup SysTick Timer for 1 msec interrupts  */
  SysTick_Config(SystemCoreClock / 1000);

  /* Initialize LEDs available on STM322xG-EVAL board *************************/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Configure Smartcard Interface GPIO pins */
  SC_IOConfig();
	  
/*-------------------------------- Idle task ---------------------------------*/
  while(1)
  {
    /* Loop while no Smartcard is detected */  
    while(CardInserted == 0)
    {
    }
       
    /* Start SC Demo ---------------------------------------------------------*/
    
    /* Wait A2R --------------------------------------------------------------*/
    SCState = SC_POWER_ON;

    SC_ADPU.Header.CLA = 0x00;
    SC_ADPU.Header.INS = SC_GET_A2R;
    SC_ADPU.Header.P1 = 0x00;
    SC_ADPU.Header.P2 = 0x00;
    SC_ADPU.Body.LC = 0x00;
   
    while(SCState != SC_ACTIVE_ON_T0) 
    {
      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Apply the Procedure Type Selection (PTS) */
    SC_PTSConfig();

    /* Inserts delay(400ms) for Smartcard clock resynchronisation */
    Delay(400);

    /* Select MF -------------------------------------------------------------*/
    SC_ADPU.Header.CLA = SC_CLA_GSM11;
    SC_ADPU.Header.INS = SC_SELECT_FILE;
    SC_ADPU.Header.P1 = 0x00;
    SC_ADPU.Header.P2 = 0x00;
    SC_ADPU.Body.LC = 0x02;

    for(i = 0; i < SC_ADPU.Body.LC; i++)
    {
      SC_ADPU.Body.Data[i] = MasterRoot[i];
    }
    while(i < LC_MAX) 
    {    
      SC_ADPU.Body.Data[i++] = 0;
    }
    SC_ADPU.Body.LE = 0;

    SC_Handler(&SCState, &SC_ADPU, &SC_Responce);

    /* Get Response on MF ----------------------------------------------------*/
    if(SC_Responce.SW1 == SC_DF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_GET_RESPONCE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;
      SC_ADPU.Body.LE = SC_Responce.SW2;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Select ICCID ----------------------------------------------------------*/
    if(((SC_Responce.SW1 << 8) | (SC_Responce.SW2)) == SC_OP_TERMINATED)
    {
      /* Check if the CHV1 is enabled */   
      if((SC_Responce.Data[13] & 0x80) == 0x00)
      {
        CHV1Status = 0x01;
      }
      /* Send APDU Command for ICCID selection */
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_SELECT_FILE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x02;

      for(i = 0; i < SC_ADPU.Body.LC; i++)
      {
        SC_ADPU.Body.Data[i] = ICCID[i];
      }
      while(i < LC_MAX) 
      {    
        SC_ADPU.Body.Data[i++] = 0;
      }
      SC_ADPU.Body.LE = 0;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Read Binary in ICCID --------------------------------------------------*/
    if(SC_Responce.SW1 == SC_EF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_READ_BINARY;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;

      SC_ADPU.Body.LE = 10;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Select GSMDir ---------------------------------------------------------*/
    if(((SC_Responce.SW1 << 8) | (SC_Responce.SW2)) == SC_OP_TERMINATED)
    {
      /* Copy the ICCID File content into ICCID_Content buffer */
      for(i = 0; i < SC_ADPU.Body.LE; i++)
      {
        ICCID_Content[i] =  SC_Responce.Data[i];
      }
      /* Send APDU Command for GSMDir selection */ 
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_SELECT_FILE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x02;

      for(i = 0; i < SC_ADPU.Body.LC; i++)
      {
        SC_ADPU.Body.Data[i] = GSMDir[i];
      }
      while(i < LC_MAX) 
      {    
        SC_ADPU.Body.Data[i++] = 0;
      }
      SC_ADPU.Body.LE = 0;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Select IMSI -----------------------------------------------------------*/
    if(SC_Responce.SW1 == SC_DF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_SELECT_FILE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x02;

      for(i = 0; i < SC_ADPU.Body.LC; i++)
      {
        SC_ADPU.Body.Data[i] = IMSI[i];
      }
      while(i < LC_MAX) 
      {    
        SC_ADPU.Body.Data[i++] = 0;
      }
      SC_ADPU.Body.LE = 0;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Get Response on IMSI File ---------------------------------------------*/
    if(SC_Responce.SW1 == SC_EF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_GET_RESPONCE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;
      SC_ADPU.Body.LE = SC_Responce.SW2;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }

    /* Read Binary in IMSI ---------------------------------------------------*/
    if(CHV1Status == 0x00)
    {
      if(((SC_Responce.SW1 << 8) | (SC_Responce.SW2)) == SC_OP_TERMINATED)
      {
        /* Enable CHV1 (PIN1) ------------------------------------------------*/
        SC_ADPU.Header.CLA = SC_CLA_GSM11;
        SC_ADPU.Header.INS = SC_ENABLE;
        SC_ADPU.Header.P1 = 0x00;
        SC_ADPU.Header.P2 = 0x01;
        SC_ADPU.Body.LC = 0x08;
 
        for(i = 0; i < SC_ADPU.Body.LC; i++)
        {
          SC_ADPU.Body.Data[i] = CHV1[i];
        }
        while(i < LC_MAX) 
        {    
          SC_ADPU.Body.Data[i++] = 0;
        }
        SC_ADPU.Body.LE = 0;

        SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
      }
    }
    else
    {
      if(((SC_Responce.SW1 << 8) | (SC_Responce.SW2)) == SC_OP_TERMINATED)
      {
        /* Verify CHV1 (PIN1) ------------------------------------------------*/
        SC_ADPU.Header.CLA = SC_CLA_GSM11;
        SC_ADPU.Header.INS = SC_VERIFY;
        SC_ADPU.Header.P1 = 0x00;
        SC_ADPU.Header.P2 = 0x01;
        SC_ADPU.Body.LC = 0x08;

        for(i = 0; i < SC_ADPU.Body.LC; i++)
        {
          SC_ADPU.Body.Data[i] = CHV1[i];
        }
        while(i < LC_MAX) 
        {    
          SC_ADPU.Body.Data[i++] = 0;
        }
        SC_ADPU.Body.LE = 0;

        SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
      }
    }
    /* Read Binary in IMSI ---------------------------------------------------*/
    if(((SC_Responce.SW1 << 8) | (SC_Responce.SW2)) == SC_OP_TERMINATED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_READ_BINARY;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;

      SC_ADPU.Body.LE = 9;

      SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    }
    if(((SC_Responce.SW1 << 8) | (SC_Responce.SW2)) == SC_OP_TERMINATED)
    {
      /* Copy the IMSI File content into IMSI_Content buffer */
      for(i = 0; i < SC_ADPU.Body.LE; i++)
      {
        IMSI_Content[i] =  SC_Responce.Data[i];
      }
    }
    /* Disable the Smartcard interface */
    SCState = SC_POWER_OFF;
    SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
    CardInserted = 0;
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length (time base 10 ms).
  * @retval None
  */
static void Delay(uint32_t nCount)
{
  TimingDelay = nCount;
 
  while(TimingDelay != 0)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number where 
  *         the assert_param error has occurred.
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
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
