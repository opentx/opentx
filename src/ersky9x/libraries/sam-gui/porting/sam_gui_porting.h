#ifndef _SAM_GUI_PORTING_
#define _SAM_GUI_PORTING_

#include <stdint.h>

#define SAM_PORTING_FREERTOS           1
#define SAM_PORTING_EMBOS              2
#define SAM_PORTING_NUTTX              3

//#define SAM_PORTING SAM_PORTING_FREERTOS

#if !defined SAM_PORTING
	#error SAM_PORTING must be defined
#endif

/*
 * Definitions for FreeRTOs
 */
#if SAM_PORTING == SAM_PORTING_FREERTOS
// FreeRTOS includes
#  include "FreeRTOS.h"
#  include "task.h"

#  define SAMGUI_TaskHandle xTaskHandle
#endif // SAM_PORTING == SAM_PORTING_FREERTOS


typedef void (*SAMGUI_fnTask)( void* pvParam ) ;

extern void SAMGUI_TaskDelay( uint32_t dwDelayMs ) ;

extern int SAMGUI_TaskCreate( SAMGUI_fnTask fnTask, const uint8_t* pucName, uint8_t* pucStack, uint32_t dwStackSize,
                              uint32_t dwPriority, void* pvParameters, SAMGUI_TaskHandle* pHandle ) ;

void SAMGUI_TaskDelete( SAMGUI_TaskHandle Handle ) ;

#endif // _SAM_GUI_PORTING_
