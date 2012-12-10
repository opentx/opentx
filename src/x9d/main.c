/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  X9D Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   This file provides the main fuction.
  * *
  ******************************************************************************
*/
#include "stm32f2xx.h"
#include "intrinsics.h"
#include "stdint.h"
#include "stdio.h"
#include "SYS_NVIC.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lcd.h"
#include "gui_tsk.h"
#include "item.h"
#include "Audio.h"
#include "sys_man.h"
#include "Macro_define.h"

#ifdef I2C_TEST
#include "i2c_ee.h"
    
#endif

#ifdef ADC_TEST
#include "adc.h"
#endif




int main()
{
  
  Delay(10000); //delay 200+ ms
#ifdef DEBUG 
  debug();
#endif
    __disable_interrupt();
    
/*HCLK=120M,SYSCLK=120M,PCLK1=30M,PCLK2=60M,SO APB1=60M(div=4),APB2=120M(div=2)*/
    
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);//add to watch to see the values

//clock init ok ,other peripherals init code;

    __enable_interrupt();
    
    while(1)
    {
    }
    //return 0;
}
 
