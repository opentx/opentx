#include "sam_gui_porting.h"

#if SAM_PORTING == SAM_PORTING_FREERTOS

extern void SAMGUI_TaskDelay( uint32_t dwDelayMs )
{
    vTaskDelay( dwDelayMs/portTICK_RATE_MS ) ;
}

extern int SAMGUI_TaskCreate( SAMGUI_fnTask fnTask, const uint8_t* pucName, uint8_t* pucStack, uint32_t dwStackSize,
                              uint32_t dwPriority, void* pvParameters, SAMGUI_TaskHandle* pHandle )
{
    xTaskCreate( fnTask, (signed char const*)pucName, dwStackSize, pvParameters, tskIDLE_PRIORITY, pHandle ) ;

    return (pHandle != NULL) ;
}

extern void SAMGUI_TaskDelete( SAMGUI_TaskHandle Handle )
{
    vTaskDelete( Handle ) ;
}

#endif // SAM_PORTING == SAM_PORTING_FREERTOS
