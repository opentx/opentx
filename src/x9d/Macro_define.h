/**
  ******************************************************************************
  * @file    Project/Macro_define.h 
  * @author  X9D Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   This file provides Macro defines for testing or working.
  * *
  ******************************************************************************
*/
#ifndef _Macro_Define_H_
#define _Macro_Define_H_
#include "stm32f2xx.h"

#define assert(x)	do{ if(!(x)) while(1);}while(0);

#define	udelay(x)	do{	uint32_t temp;	temp=((x+0)<<3);	while(--temp);}while(0)	
#define delayUsec(x)	udelay(x)

#define LCD_TEST
#define AUDIO_TEST
//#define I2C_TEST
//#define ADC_TEST



#endif