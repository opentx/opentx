/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "Macro_define.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
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
  {
  }
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
  {
  }
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
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{

//}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

#ifdef AUDIO_TEST
#include "Audio.h"
void SPI3_IRQHandler(void)
{


}
#endif
//all the available peripheral interrupt handler's name are followed 
/*
         ; External Interrupts
        DCD     WWDG_IRQHandler                   ; Window WatchDog                                        
        DCD     PVD_IRQHandler                    ; PVD through EXTI Line detection                        
        DCD     TAMP_STAMP_IRQHandler             ; Tamper and TimeStamps through the EXTI line            
        DCD     RTC_WKUP_IRQHandler               ; RTC Wakeup through the EXTI line                       
        DCD     FLASH_IRQHandler                  ; FLASH                                           
        DCD     RCC_IRQHandler                    ; RCC                                             
        DCD     EXTI0_IRQHandler                  ; EXTI Line0                                             
        DCD     EXTI1_IRQHandler                  ; EXTI Line1                                             
        DCD     EXTI2_IRQHandler                  ; EXTI Line2                                             
        DCD     EXTI3_IRQHandler                  ; EXTI Line3                                             
        DCD     EXTI4_IRQHandler                  ; EXTI Line4                                             
        DCD     DMA1_Stream0_IRQHandler           ; DMA1 Stream 0                                   
        DCD     DMA1_Stream1_IRQHandler           ; DMA1 Stream 1                                   
        DCD     DMA1_Stream2_IRQHandler           ; DMA1 Stream 2                                   
        DCD     DMA1_Stream3_IRQHandler           ; DMA1 Stream 3                                   
        DCD     DMA1_Stream4_IRQHandler           ; DMA1 Stream 4                                   
        DCD     DMA1_Stream5_IRQHandler           ; DMA1 Stream 5                                   
        DCD     DMA1_Stream6_IRQHandler           ; DMA1 Stream 6                                   
        DCD     ADC_IRQHandler                    ; ADC1, ADC2 and ADC3s                            
        DCD     CAN1_TX_IRQHandler                ; CAN1 TX                                                
        DCD     CAN1_RX0_IRQHandler               ; CAN1 RX0                                               
        DCD     CAN1_RX1_IRQHandler               ; CAN1 RX1                                               
        DCD     CAN1_SCE_IRQHandler               ; CAN1 SCE                                               
        DCD     EXTI9_5_IRQHandler                ; External Line[9:5]s                                    
        DCD     TIM1_BRK_TIM9_IRQHandler          ; TIM1 Break and TIM9                   
        DCD     TIM1_UP_TIM10_IRQHandler          ; TIM1 Update and TIM10                 
        DCD     TIM1_TRG_COM_TIM11_IRQHandler     ; TIM1 Trigger and Commutation and TIM11
        DCD     TIM1_CC_IRQHandler                ; TIM1 Capture Compare                                   
        DCD     TIM2_IRQHandler                   ; TIM2                                            
        DCD     TIM3_IRQHandler                   ; TIM3                                            
        DCD     TIM4_IRQHandler                   ; TIM4                                            
        DCD     I2C1_EV_IRQHandler                ; I2C1 Event                                             
        DCD     I2C1_ER_IRQHandler                ; I2C1 Error                                             
        DCD     I2C2_EV_IRQHandler                ; I2C2 Event                                             
        DCD     I2C2_ER_IRQHandler                ; I2C2 Error                                               
        DCD     SPI1_IRQHandler                   ; SPI1                                            
        DCD     SPI2_IRQHandler                   ; SPI2                                            
        DCD     USART1_IRQHandler                 ; USART1                                          
        DCD     USART2_IRQHandler                 ; USART2                                          
        DCD     USART3_IRQHandler                 ; USART3                                          
        DCD     EXTI15_10_IRQHandler              ; External Line[15:10]s                                  
        DCD     RTC_Alarm_IRQHandler              ; RTC Alarm (A and B) through EXTI Line                  
        DCD     OTG_FS_WKUP_IRQHandler            ; USB OTG FS Wakeup through EXTI line                        
        DCD     TIM8_BRK_TIM12_IRQHandler         ; TIM8 Break and TIM12                  
        DCD     TIM8_UP_TIM13_IRQHandler          ; TIM8 Update and TIM13                 
        DCD     TIM8_TRG_COM_TIM14_IRQHandler     ; TIM8 Trigger and Commutation and TIM14
        DCD     TIM8_CC_IRQHandler                ; TIM8 Capture Compare                                   
        DCD     DMA1_Stream7_IRQHandler           ; DMA1 Stream7                                           
        DCD     FSMC_IRQHandler                   ; FSMC                                            
        DCD     SDIO_IRQHandler                   ; SDIO                                            
        DCD     TIM5_IRQHandler                   ; TIM5                                            
        DCD     SPI3_IRQHandler                   ; SPI3                                            
        DCD     UART4_IRQHandler                  ; UART4                                           
        DCD     UART5_IRQHandler                  ; UART5                                           
        DCD     TIM6_DAC_IRQHandler               ; TIM6 and DAC1&2 underrun errors                   
        DCD     TIM7_IRQHandler                   ; TIM7                   
        DCD     DMA2_Stream0_IRQHandler           ; DMA2 Stream 0                                   
        DCD     DMA2_Stream1_IRQHandler           ; DMA2 Stream 1                                   
        DCD     DMA2_Stream2_IRQHandler           ; DMA2 Stream 2                                   
        DCD     DMA2_Stream3_IRQHandler           ; DMA2 Stream 3                                   
        DCD     DMA2_Stream4_IRQHandler           ; DMA2 Stream 4                                   
        DCD     ETH_IRQHandler                    ; Ethernet                                        
        DCD     ETH_WKUP_IRQHandler               ; Ethernet Wakeup through EXTI line                      
        DCD     CAN2_TX_IRQHandler                ; CAN2 TX                                                
        DCD     CAN2_RX0_IRQHandler               ; CAN2 RX0                                               
        DCD     CAN2_RX1_IRQHandler               ; CAN2 RX1                                               
        DCD     CAN2_SCE_IRQHandler               ; CAN2 SCE                                               
        DCD     OTG_FS_IRQHandler                 ; USB OTG FS                                      
        DCD     DMA2_Stream5_IRQHandler           ; DMA2 Stream 5                                   
        DCD     DMA2_Stream6_IRQHandler           ; DMA2 Stream 6                                   
        DCD     DMA2_Stream7_IRQHandler           ; DMA2 Stream 7                                   
        DCD     USART6_IRQHandler                 ; USART6                                           
        DCD     I2C3_EV_IRQHandler                ; I2C3 event                                             
        DCD     I2C3_ER_IRQHandler                ; I2C3 error                                             
        DCD     OTG_HS_EP1_OUT_IRQHandler         ; USB OTG HS End Point 1 Out                      
        DCD     OTG_HS_EP1_IN_IRQHandler          ; USB OTG HS End Point 1 In                       
        DCD     OTG_HS_WKUP_IRQHandler            ; USB OTG HS Wakeup through EXTI                         
        DCD     OTG_HS_IRQHandler                 ; USB OTG HS                                      
        DCD     DCMI_IRQHandler                   ; DCMI                                            
        DCD     CRYP_IRQHandler                   ; CRYP crypto                                     
        DCD     HASH_RNG_IRQHandler               ; Hash and Rng
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
