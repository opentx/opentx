/**
  ******************************************************************************
  * @file    TIM/6Steps/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
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
#include "stm32f2xx_it.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM_6Steps
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t step = 1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* Generate TIM1 COM event by software */
  TIM_GenerateEvent(TIM1, TIM_EventSource_COM);
}

/******************************************************************************/
/*            STM32F2xx Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles TIM1 Trigger and commutation and TIM11 
  *         global interrupts requests.
  * @param  None
  * @retval None
  */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  /* Clear TIM1 COM pending bit */
  TIM_ClearITPendingBit(TIM1, TIM_IT_COM);

  if (step == 1)
  {
    /* Next step: Step 2 Configuration -------------------------------------- */
    /*  Channel3 configuration */
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

    /*  Channel1 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

    /*  Channel2 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1 );
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
    step++;
  }
  else if (step == 2)
  {
    /* Next step: Step 3 Configuration -------------------------------------- */
    /*  Channel2 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
    
    /*  Channel3 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
     TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

    /*  Channel1 configuration */
    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
    step++;
  }
  else if (step == 3)
  {
    /* Next step: Step 4 Configuration -------------------------------------- */
    /*  Channel3 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

    /*  Channel2 configuration */
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

    /*  Channel1 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
    step++;
  }
  else if (step == 4)
  {
    /* Next step: Step 5 Configuration -------------------------------------- */
    /*  Channel3 configuration */
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

    /*  Channel1 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);

    /*  Channel2 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
    step++;
  }
  else if (step == 5)
  {
    /* Next step: Step 6 Configuration -------------------------------------- */
    /*  Channel3 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);

    /*  Channel1 configuration */
    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

    /*  Channel2 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
    step++;
  }
  else
  {
    /* Next step: Step 1 Configuration -------------------------------------- */
    /*  Channel1 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

    /*  Channel3 configuration */
    TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);

    /*  Channel2 configuration */
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
    step = 1;
  }
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
