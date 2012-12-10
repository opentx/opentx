/**
  ******************************************************************************
  * @file    Project/Audio/Audio.h
  * @author  X9D Application Team
  * @version V 0.2
  * @date    12-JULY-2011
  * @brief   Header for Audio.c.
  ******************************************************************************
*/
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "stdint.h"
#include "Macro_define.h"

extern const uint16_t AUDIO_SAMPLE[];
//
void I2S2_Init();


typedef	struct{
	uint32_t size;		//number of chars per font
	uint16_t *base;				//font addr
}A_SAMPLE;

extern A_SAMPLE demo_sample;

#endif